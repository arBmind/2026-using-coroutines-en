#include "../desktop-duplication-cpp/src/meta/void_coroutine.h"

bool completed = false;

void runCoroutine() {
    completed = true;
    co_return;
}

int main() {
    runCoroutine();
    return completed ? 0 : 1;
}
