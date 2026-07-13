#include <coroutine>
#include <expected>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

struct Response {
  int status{};
  std::string body;
};

struct Error {
  int code{};
  std::string message;
};

template <class T>
struct Task {
  struct promise_type {
    T value{};

    Task get_return_object() { return {}; }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void unhandled_exception() {}
    void return_value(T result) { value = std::move(result); }
  };

  bool await_ready() const noexcept { return true; }
  void await_suspend(std::coroutine_handle<>) const noexcept {}
  T await_resume() const { return {}; }
};

template <>
struct Task<void> {
  struct promise_type {
    Task get_return_object() { return {}; }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void unhandled_exception() {}
    void return_void() {}
  };

  bool await_ready() const noexcept { return true; }
  void await_suspend(std::coroutine_handle<>) const noexcept {}
  void await_resume() const {}
};

Task<Response> http_get(std::string_view url);
void http_get(std::string url, std::function<void(Response)> callback);
Task<std::expected<std::string, Error>> parse_payload(Response response);
void parse_payload(Response response, std::function<void(std::string)> callback);
void report_error(std::string_view id, int status);
void store_customer(std::string_view id, std::string_view payload);

namespace callbacks {

// #region manual-state
void load_customer(std::string id) {
  auto url = "/customers/" + id;
  http_get(url, [id](Response response) {
    if (response.status != 200) {
      report_error(id, response.status);
      return;
    }

    parse_payload(response, [id](auto payload) {
      store_customer(id, payload);
    });
  });
}
// #endregion manual-state

} // namespace callbacks

namespace coroutines {

// #region coroutine-flow
Task<std::expected<std::string, Error>> load_customer(std::string id) {
  auto response = co_await http_get("/customers/" + id); // [!code word:co_await]

  if (response.status != 200) {
    co_return std::unexpected(Error{response.status, "request failed"});
  }

  co_return co_await parse_payload(response); // [!code word:co_await]
}
// #endregion coroutine-flow

} // namespace coroutines

struct Thread {};

struct Work {
  void run() {}
  void fail(std::exception_ptr) {}
};

void queue_apc(Thread& thread, std::function<void()> callback);

struct SwitchTo {
  bool await_ready() const noexcept { return false; }
  void await_suspend(std::coroutine_handle<>) const noexcept {}
  void await_resume() const noexcept {}
};

SwitchTo switch_to(Thread& thread);

namespace apc {

// #region apc-before
void schedule_work(Thread& thread, Work args) {
  queue_apc(thread, [work = std::make_unique<Work>(std::move(args))]() mutable {
    try {
      work->run();
    } catch (...) {
      work->fail(std::current_exception());
    }
  });
}
// #endregion apc-before

// #region apc-after
Task<void> run_on_worker(Thread& thread, Work work) {
  co_await switch_to(thread); // [!code word:co_await]
  work.run();
}
// #endregion apc-after

} // namespace apc
