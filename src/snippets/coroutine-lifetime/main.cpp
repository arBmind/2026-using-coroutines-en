#include "task.hpp"

#include <iostream>

void use(string const &state) { std::cout << state << '\n'; }

// This function is compiled but deliberately not executed: resuming the
// returned task would access the destroyed lambda closure.
// #region lambda-risky
auto launchRisky() -> task {
    auto job = [state = string("important")]() -> task {
        use(state); // state belongs to the closure, not the frame
        co_return;
    };
    return job();
}
// #endregion lambda-risky

// #region named-safe
auto doJob(string state) -> task {
    use(state); // value parameter belongs to the coroutine frame
    co_return;
}

auto launchSafe() -> task { return doJob("important"); }
// #endregion named-safe

int main() {
    auto job = launchSafe();
    job.resume();
}
