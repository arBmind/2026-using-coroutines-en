#include "support.hpp"

auto loadProfile(string id) -> ProfileAwaitable { return ProfileAwaitable{.id = std::move(id), .suppliedProfile = {}}; }

// #region coroutine-flow
auto greeting(string id) -> task<string> {
    auto profile = co_await loadProfile(id);
    co_return format("Hello {}", profile.userName);
}

void caller() {
    auto task = greeting("C++");
    task.resume(Profile{.userName = "Meeting C++ 2026"});
    println("{}!", task.result());
}
// #endregion coroutine-flow

int main() { caller(); }
