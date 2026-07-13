#pragma once

#include <coroutine>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#if __has_include(<format>) && __has_include(<print>)
#    include <format>
#    include <print>
using std::format;
using std::println;
#else
// The devcontainer uses libc++ 22 and therefore takes the standard-library
// branch above. These tiny fallbacks keep the example buildable with older
// Clang installations while editing outside the container.
inline auto format(std::string_view pattern, std::string const &value) -> std::string {
    if (pattern != "Hello {}") throw std::invalid_argument{"unexpected example format"};
    return "Hello " + value;
}

inline void println(std::string_view pattern, std::string const &value) {
    if (pattern != "{}!") throw std::invalid_argument{"unexpected example format"};
    std::cout << value << "!\n";
}
#endif

using std::string;

struct Profile {
    string userName;
};

struct ProfileAwaitable {
    string id;
    std::optional<Profile> suppliedProfile;

    [[nodiscard]] auto await_ready() const noexcept -> bool { return false; }

    template<class Promise>
    void await_suspend(std::coroutine_handle<Promise> handle) noexcept {
        handle.promise().profileSlot = &suppliedProfile;
    }

    auto await_resume() -> Profile {
        if (!suppliedProfile) throw std::logic_error{"profile was not supplied"};
        return std::move(*suppliedProfile);
    }
};

template<class T>
class task {
public:
    struct promise_type {
        std::optional<Profile> *profileSlot = nullptr;
        std::optional<T> value;
        std::exception_ptr error;

        auto get_return_object() noexcept -> task {
            return task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        auto initial_suspend() const noexcept -> std::suspend_never { return {}; }
        auto final_suspend() const noexcept -> std::suspend_always { return {}; }
        void unhandled_exception() noexcept { error = std::current_exception(); }

        template<class Value>
        void return_value(Value &&result) {
            value.emplace(std::forward<Value>(result));
        }
    };

    task(task const &) = delete;
    auto operator=(task const &) -> task & = delete;

    task(task &&other) noexcept
        : handle{std::exchange(other.handle, {})} {}

    ~task() {
        if (handle) handle.destroy();
    }

    void resume(Profile profile) {
        if (!handle || handle.done() || handle.promise().profileSlot == nullptr) {
            throw std::logic_error{"task is not waiting for a profile"};
        }
        *handle.promise().profileSlot = std::move(profile);
        handle.resume();
    }

    [[nodiscard]] auto result() const -> T const & {
        if (!handle || !handle.done()) throw std::logic_error{"task has not completed"};
        if (handle.promise().error) std::rethrow_exception(handle.promise().error);
        return *handle.promise().value;
    }

private:
    explicit task(std::coroutine_handle<promise_type> coroutine) noexcept
        : handle{coroutine} {}

    std::coroutine_handle<promise_type> handle;
};
