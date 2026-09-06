#pragma once
#include <coroutine>

template<typename... Args>
struct std::coroutine_traits<void, Args...> {
    struct promise_type {
        void get_return_object() noexcept {}
        auto initial_suspend() noexcept -> std::suspend_never { return {}; }
        auto final_suspend() noexcept -> std::suspend_never { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() {}
    };
};
