#include <bit>
#include <coroutine>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

using std::string;

using Handle = std::coroutine_handle<>;

// #region await-suspend-handle-options
auto returnToResumer(Handle) noexcept -> Handle { return std::noop_coroutine(); }
auto continueCurrent(Handle current) noexcept -> Handle { return current; }
auto transferToOther(Handle, Handle other) noexcept -> Handle { return other; }
// #endregion await-suspend-handle-options

enum class Activity { Writing, Idle };
struct Response {
    bool success = true;
};

struct Account {
    void setActivity(Activity value) { activity = value; }
    auto applyTracker(Response response) -> bool { return response.success; }
    void applyIssue(Response) { issueLoaded = true; }
    void markStale() { stale = true; }
    auto trackerNeedsIssue() const -> bool { return true; }

    Activity activity = Activity::Idle;
    bool issueLoaded = false;
    bool stale = false;
};

struct Network {
    template<class Callback>
    void post(string, Callback callback) {
        callback(Response{});
    }

    template<class Callback>
    void get(string, Callback callback) {
        callback(Response{});
    }
};

// #region callback-flow
void startTimeTracker(Network &network, Account &account) {
    account.setActivity(Activity::Writing);
    network.post("/time_tracker", [&](Response response) {
        if (!account.applyTracker(response)) {
            account.markStale();
            return;
        }
        if (account.trackerNeedsIssue()) {
            network.get("/issue", [&](Response issue) {
                account.applyIssue(issue);
                account.setActivity(Activity::Idle);
            });
        }
        else {
            account.setActivity(Activity::Idle);
        }
    });
}
// #endregion callback-flow

using ApcParameter = std::uintptr_t;
using ApcCallback = void (*)(ApcParameter);

// #region apc-contract
auto queueUserApc(ApcCallback callback, ApcParameter parameter) -> bool {
    callback(parameter);
    return true;
}
// #endregion apc-contract

struct Work {
    void run() { completed = true; }
    bool completed = false;
};

void runWork(ApcParameter parameter) {
    auto work = std::unique_ptr<Work>{std::bit_cast<Work *>(parameter)};
    work->run();
}

// #region unique-apc-transfer
void queueWork() {
    auto work = std::make_unique<Work>();
    if (queueUserApc(runWork, std::bit_cast<ApcParameter>(work.get()))) {
        work.release(); // callback reconstructs ownership
    }
}
// #endregion unique-apc-transfer

class Thread {
public:
    template<class Functor>
    void queueUserApc(Functor &&functor) {
        auto callback = std::make_unique<std::function<void()>>(std::forward<Functor>(functor));
        auto invoke = [](ApcParameter parameter) {
            auto owner = std::unique_ptr<std::function<void()>>{
                std::bit_cast<std::function<void()> *>(parameter)};
            (*owner)();
        };
        if (::queueUserApc(invoke, std::bit_cast<ApcParameter>(callback.get()))) callback.release();
    }
};

// #region lambda-apc-call
void queueRender(Thread &thread, Work &frame) {
    thread.queueUserApc([&frame] {
        frame.run();
    });
}
// #endregion lambda-apc-call

int main() {
    auto network = Network{};
    auto account = Account{};
    startTimeTracker(network, account);

    auto thread = Thread{};
    auto frame = Work{};
    queueRender(thread, frame);
    queueWork();

    if (account.activity != Activity::Idle || !account.issueLoaded || !frame.completed) return 1;
    std::cout << "compiled patterns completed\n";
}
