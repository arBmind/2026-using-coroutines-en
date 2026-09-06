#include <memory>
#include <type_traits>
#include <utility>
#include <windows.h>

// #region owned-apc
template<class Functor>
class UniqueCallbackAdapter {
    std::unique_ptr<Functor> m_functor;
    static void CALLBACK invoke(ULONG_PTR parameter) {
        auto functor = std::unique_ptr<Functor>{reinterpret_cast<Functor *>(parameter)};
        (*functor)();
    }

public:
    template<class F>
    explicit UniqueCallbackAdapter(F &&functor)
        : m_functor{std::make_unique<Functor>(std::forward<F>(functor))} {}
    auto c_callback() const -> PAPCFUNC { return &invoke; }
    auto c_parameter() const -> ULONG_PTR {
        return reinterpret_cast<ULONG_PTR>(m_functor.get());
    }
    void release() { (void)m_functor.release(); }
};

template<class Functor, class Callback = std::decay_t<Functor>>
bool queueUserApc(HANDLE thread, Functor &&functor) {
    auto callback = UniqueCallbackAdapter<Callback>{std::forward<Functor>(functor)};
    const auto success = QueueUserAPC(callback.c_callback(), thread, callback.c_parameter());
    if (success != 0) callback.release();
    return success != 0;
}
// #endregion owned-apc

#ifdef APC_OWNERSHIP_TEST
#include <cstdio>

DWORD WINAPI ThreadLoop(LPVOID) {
    while (true) SleepEx(INFINITE, TRUE);
}

int main() {
    auto thread = CreateThread(nullptr, 0, ThreadLoop, nullptr, 0, nullptr);
    if (!thread || !queueUserApc(thread, [value = 42] {
            std::printf("Hello from RAII APC. Got: %d\n", value);
            std::fflush(stdout);
        }))
        return 1;
    while (true);
}
#endif
