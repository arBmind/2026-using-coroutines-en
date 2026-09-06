#include "DuplicationController.h"
#include "CapturedUpdate.h"
#include "FrameUpdater.h"
#include "Model.h"
#include "renderer.h"

#include <meta/void_coroutine.h>

namespace deskdup {
namespace {

auto createRetryTimer() -> WaitableTimer {
    auto config = WaitableTimer::Config{};
    config.timerName = TEXT("Local:RetryTimer");
    return WaitableTimer{config};
}

auto createWindowConfig(const Window &parent, Dimension dim) -> win32::WindowWithMessages::Config {
    return {
        .style = win32::WindowStyle::child(),
        .name = win32::Name{L"Hello"},
        .rect = Rect{.topLeft = {}, .dimension = dim},
        .parent = &parent,
    };
}

} // namespace

DuplicationController::DuplicationController(const Args &args)
    : m_windowClass{args.windowClass}
    , m_controller{args.mainController}
    , m_mainThread{args.mainThread}
    , m_outputWindow{args.outputWindow}
    , m_renderWindow{m_windowClass.createWindow(
          createWindowConfig(m_outputWindow, m_controller.config().outputRect().dimension))}
    , m_renderThread{renderThreadConfig(m_controller.operationModeLens())}
    , m_retryTimer{createRetryTimer()} {
    m_renderWindow.setCustomHandler<&DuplicationController::forwardRenderMessage>(this);
    m_renderThread.start();
}

DuplicationController::~DuplicationController() {
    resetOnMain();
    m_renderThread.stop();
}

void DuplicationController::updateDuplicationStatus(DuplicationStatus status) {
    using enum DuplicationStatus;
    switch (status) {
    case Live: return startOnMain();
    case Pause: return pauseOnMain();
    }
}

void DuplicationController::updateOutputDimension(Dimension dimension) {
    m_renderWindow.moveClient(Rect{.topLeft = {}, .dimension = dimension});
    m_renderThread.resizeWindow(dimension);
    if (m_controller.state().duplicationStatus != DuplicationStatus::Live) {
        restart();
    }
}

void DuplicationController::updateOutputZoom(float zoom) { m_renderThread.zoomOutput(zoom); }

void DuplicationController::updateCaptureOffset(Vec2f offset) {
    m_renderThread.updateOffset(offset);
    if (m_controller.state().duplicationStatus != DuplicationStatus::Live) {
        restart();
    }
}

void DuplicationController::restart() {
    stopOnMain();
    if (m_controller.state().duplicationStatus == DuplicationStatus::Live) {
        startOnMain();
    }
}

auto DuplicationController::renderThreadConfig(OperationModeLens lens) -> RenderThread::Config {
    auto config = RenderThread::Config{
        .pointerBuffer = m_pointerUpdater.data(),
        .outputZoom = lens.outputZoom(),
        .captureOffset = lens.captureOffset(),
    };
    config.setCallbacks(this);
    return config;
}

void DuplicationController::startOnMain() {
    auto canStart = [this]() {
        switch (m_status) {
            using enum Status;
        case Paused:
            m_renderWindow.show();
            updateStatusOnMain(Status::Resuming);
            // note: trigger interrupt to read status again
            m_captureThread.thread().queueUserApc([]() {});
            return false;
        case Stopped:
        case Failed: return true;
        case Starting:
        case Live:
        case Stopping:
        case Resuming:
        case RetryStarting: return false;
        }
        return false;
    }();
    if (!canStart) return;
    try {
        try {
            auto deviceValue = renderer::createDevice();
            auto device = deviceValue.device;
            auto deviceContext = deviceValue.deviceContext;

            auto dimensionData =
                renderer::getDimensionData(device, {m_controller.operationModeLens().captureMonitor()});
            m_controller.updateScreenRect(dimensionData.rect);
            m_displayRect = dimensionData.rect;

            m_targetTexture = renderer::createSharedTexture(device, dimensionData.rect.dimension);
            m_windowClass.recreateWindow(
                m_renderWindow, createWindowConfig(m_outputWindow, m_controller.config().outputDimension));
            m_renderWindow.setCustomHandler<&DuplicationController::forwardRenderMessage>(this);
            auto initArgs = [&]() -> WindowRenderer::InitArgs {
                return {
                    .basic =
                        {
                            .device = device,
                            .deviceContext = deviceContext,
                        },
                    .windowHandle = m_renderWindow.handle(),
                    .windowDimension = m_controller.config().outputDimension,
                    .texture = m_targetTexture,
                };
            };
            m_renderThread.initializeRenderer(initArgs());
            m_renderWindow.show();

            auto handle = renderer::getSharedHandle(m_targetTexture);
            updateStatusOnMain(Status::Starting);
            startCaptureThread(handle);
            runCapture();
        }
        catch (const renderer::Error &e) {
            throw Expected{e.message};
        }
    }
    catch (const Expected &e) {
        OutputDebugStringA(e.text);
        OutputDebugStringA("\n");
        resetOnMain();

        updateStatusOnMain(Status::RetryStarting);
        awaitRetry();
    }
}

void DuplicationController::pauseOnMain() {
    auto canPause = m_status == Status::Live;
    if (!canPause) return;

    updateStatusOnMain(Status::Paused);

    if (m_controller.config().operationMode == OperationMode::CaptureArea) {
        m_renderWindow.hide();
    }
}

void DuplicationController::stopOnMain() {
    updateStatusOnMain(Status::Stopping);
    resetOnMain();
    updateStatusOnMain(Status::Stopped);
}

void DuplicationController::resetOnMain() {
    try {
        m_captureThread.stop();
        // m_renderThread.stop();
        m_frameUpdater.reset();
        m_targetTexture.Reset();
        m_renderThread.reset();
    }
    catch (Expected &e) {
        OutputDebugStringA("resetOnMain Threw: ");
        OutputDebugStringA(e.text);
        OutputDebugStringA("\n");
    }
}

void DuplicationController::updateStatusOnMain(Status status) {
    if (m_status.load() == status) return;
    m_status.store(status);
    m_controller.updateSystemStatus([status] {
        switch (status) {
        case Status::Stopped: return SystemStatus::Neutral;
        case Status::Live: return SystemStatus::Green;
        case Status::Starting:
        case Status::Stopping:
        case Status::Resuming:
        case Status::Paused: return SystemStatus::Yellow;
        case Status::RetryStarting:
        case Status::Failed: return SystemStatus::Red;
        }
        return SystemStatus::Red; // unknown
    }());
}

static void sleepForCapture() {
    const auto timeout = 1;
    const auto alertable = true;
    SleepEx(timeout, alertable);
}

void DuplicationController::sleepWhilePaused() {
    while (true) {
        auto status = m_status.load();
        if (status != Status::Paused) break;
        const auto timeout = status == Status::Paused ? INFINITE : 1;
        const auto alertable = true;
        SleepEx(timeout, alertable);
    }
}

auto DuplicationController::setIsLiveOnMain() -> Status {
    auto status = m_status.load();
    if (status == Status::Starting || status == Status::Resuming) {
        updateStatusOnMain(Status::Live);
        return Status::Live;
    }
    return status;
}

void DuplicationController::startCaptureThread(HANDLE targetHandle) {
    auto deviceValue = renderer::createDevice();
    auto device = deviceValue.device;
    auto deviceContext = deviceValue.deviceContext;

    auto updaterArgs = FrameUpdater::InitArgs{};
    updaterArgs.device = device;
    updaterArgs.deviceContext = deviceContext;
    updaterArgs.targetHandle = targetHandle;
    m_frameUpdater = FrameUpdater{std::move(updaterArgs)};

    auto setupArgs = Capture::SetupArgs{};
    setupArgs.display = m_controller.operationModeLens().captureMonitor();
    setupArgs.device = device;
    setupArgs.offset = m_displayRect.topLeft;
    m_capture.setup(std::move(setupArgs));

    m_captureThread.start();
}

// #region run-capture-coroutine
void DuplicationController::runCapture() {
    try {
        // #region run-capture-acquire-render
        co_await resumeOn(m_captureThread.thread());
        m_capture.initialize();
        while (true) { // note: we are on the capture thread
            sleepWhilePaused();
            auto optFrame = m_capture.captureUpdate();
            if (!optFrame) {
                sleepForCapture();
                continue;
            }
            { // process captured frame on render thread and trigger update
                auto context = m_capture.context();
                co_await resumeOn(m_renderThread.thread());
                m_frameUpdater->update(optFrame->frame, context);
                m_pointerUpdater.update(optFrame->pointer, context);
                m_renderThread.renderFrame();
            }
        // #endregion run-capture-acquire-render
            // #region run-capture-status-release
            {
                auto status = m_status.load();
                if (status == Status::Starting || status == Status::Resuming) {
                    // detour to update status on main thread is required
                    co_await resumeOn(m_mainThread);
                    status = setIsLiveOnMain();
                }
                if (status != Status::Live && status != Status::Paused) {
                    break; // we are done here
                }
            }
            co_await resumeOn(m_captureThread.thread());
            m_capture.releaseFrame();
            // #endregion run-capture-status-release
        }
    }
    catch (...) {
        setError(std::current_exception());
    }
}
// #endregion run-capture-coroutine

void DuplicationController::awaitRetry() {
    using namespace std::chrono_literals;
    auto timerArgs = WaitableTimer::SetArgs{};
    timerArgs.time = 250ms;
    auto success = m_retryTimer.set<&DuplicationController::retryTimeout>(timerArgs, {this});
    if (!success) throw Unexpected{"failed to arm retry timer"};
}

void DuplicationController::retryTimeout() { startOnMain(); }

auto DuplicationController::forwardRenderMessage(const win32::WindowMessage &msg) -> win32::OptLRESULT {
    return m_outputWindow.handleMessage(msg);
}

void DuplicationController::setError(const std::exception_ptr &error) {
    co_await resumeOn(m_mainThread);
    try {
        std::rethrow_exception(error);
    }
    catch (const Expected &e) {
        OutputDebugStringA(e.text);
        OutputDebugStringA("\n");
        stopOnMain();
        updateStatusOnMain(Status::Failed);
        awaitRetry();
    }
}

} // namespace deskdup
