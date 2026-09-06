#pragma once
#include "Handle.h"

#include "meta/callback_adapter.h"
#include <coroutine>
#include <thread>

namespace win32 {

/// Wrapper for win32 Threads
/// note: handle is owned by this!
struct Thread {
    explicit Thread() = default;
    explicit Thread(HANDLE handle)
        : m_threadHandle{handle} {}

    static auto fromCurrent() -> Thread;
    static auto fromStd(std::jthread &) -> Thread;

    auto handle() const -> HANDLE { return m_threadHandle.get(); }

    // #region queue-user-apc-adapter
    template<class Functor>
    void queueUserApc(Functor &&functor) {
        auto callback = UniqueCallbackAdapter<ULONG_PTR>(std::forward<Functor>(functor));
        const auto success = ::QueueUserAPC(callback.c_callback(), handle(), callback.c_parameter());
        if (success) callback.release();
        // if (!success) throw Unexpected{"api::setError failed to queue APC"};
    }
    // #endregion queue-user-apc-adapter

private:
    Handle m_threadHandle{};
};

// #region resume-on-awaitable
inline auto resumeOn(Thread const &thread) {
    struct awaitable {
        HANDLE handle;

        bool await_ready() noexcept { return false; }
        void await_suspend(std::coroutine_handle<> h) {
            static constexpr auto c_resume = [](ULONG_PTR address) {
                auto ch = std::coroutine_handle<>::from_address(std::bit_cast<void *>(address));
                ch.resume();
            };
            auto const address = std::bit_cast<ULONG_PTR>(h.address());
            auto const success = ::QueueUserAPC(c_resume, handle, address);
            if (!success) h.destroy();
        }
        void await_resume() noexcept {}
    };
    return awaitable{thread.handle()};
}
// #endregion resume-on-awaitable

} // namespace win32
