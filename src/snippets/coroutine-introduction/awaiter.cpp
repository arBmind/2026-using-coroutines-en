// #region awaiter
struct Awaiter {
    // should return false or no await happens
    bool await_ready() const;

    // decide what happens on suspension
    // a) last call to resume returns
    void await_suspend(std::coroutine_handle<> handle);

    // b) on false nothing happens, true like void
    bool await_suspend(std::coroutine_handle<> handle);

    // c) returned coroutine is resumed
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> handle);

    // return result of co_await expression
    auto await_resume();
};
// #endregion awaiter