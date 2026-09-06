#include "../desktop-duplication-cpp/src/meta/void_coroutine.h"
#include "../desktop-duplication-cpp/src/win32/Thread.h"

namespace {

PAPCFUNC queuedCallback{};
ULONG_PTR queuedParameter{};
bool resumed = false;

} // namespace

DWORD QueueUserAPC(PAPCFUNC callback, HANDLE, ULONG_PTR parameter) {
    queuedCallback = callback;
    queuedParameter = parameter;
    return 1;
}

void crossThread(win32::Thread const &thread) {
    co_await win32::resumeOn(thread);
    resumed = true;
}

int main() {
    auto thread = win32::Thread{reinterpret_cast<HANDLE>(1)};
    crossThread(thread);
    if (resumed || queuedCallback == nullptr)
        return 1;
    queuedCallback(queuedParameter);
    return resumed ? 0 : 1;
}
