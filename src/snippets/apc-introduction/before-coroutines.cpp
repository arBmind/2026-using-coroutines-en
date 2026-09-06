// From desktop-duplication-cpp, immediately before commit d07fe0a.
// #region class-apcs
struct DuplicationController {
    win32::Thread &m_mainThread;
    RenderThread m_renderThread;
    CaptureThread m_captureThread;
    // ... many more
};

void DuplicationController::setFrame(CapturedUpdate &&update, const FrameContext &context) {
    m_renderThread.thread().queueUserApc([this, update = std::move(update), &context]() mutable {
        setFrameOnRender(std::move(update), context);
    });
}

void DuplicationController::setFrameOnRender(CapturedUpdate &&update, const FrameContext &context) {
    m_frameUpdater->update(update.frame, context);
    m_renderThread.renderFrame();
    m_mainThread.queueUserApc([this] {
        updateStatusOnMain(Status::Live);
        m_captureThread.next();
    });
}
// #endregion class-apcs
