#pragma once

#include <coroutine>
#include <exception>
#include <string>
#include <utility>

using std::string;

class task {
public:
    struct promise_type {
        auto get_return_object() noexcept -> task {
            return task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        auto initial_suspend() const noexcept -> std::suspend_always { return {}; }
        auto final_suspend() const noexcept -> std::suspend_always { return {}; }
        void return_void() const noexcept {}
        void unhandled_exception() const noexcept { std::terminate(); }
    };

    task(task const &) = delete;
    task(task &&other) noexcept
        : handle{std::exchange(other.handle, {})} {}
    ~task() {
        if (handle) handle.destroy();
    }

    void resume() {
        if (handle && !handle.done()) handle.resume();
    }

private:
    explicit task(std::coroutine_handle<promise_type> coroutine) noexcept
        : handle{coroutine} {}
    std::coroutine_handle<promise_type> handle;
};
