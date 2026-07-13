#include "support.hpp"

auto loadProfile(string const &) -> Profile { return Profile{.userName = "Meeting C++ 2026"}; }

// #region synchronous-flow
auto greeting(string id) -> string {
    auto profile = loadProfile(id);
    return format("Hello {}", profile.userName);
}

void caller() {
    auto message = greeting("C++");
    println("{}!", message);
}
// #endregion synchronous-flow

int main() { caller(); }
