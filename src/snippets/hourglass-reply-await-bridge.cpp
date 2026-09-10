struct NetworkTask {
  std::coroutine_handle<> handle;
  struct promise_type {
    // ...
    auto await_transform(QNetworkReply *reply) {
      struct Awaiter {
        QNetworkReply *reply;
        bool await_ready() {
          return reply->isFinished();
        }
        void await_suspend(std::coroutine_handle<> handle) {
          QObject::connect(reply, &QNetworkReply::finished, [handle]() {
            handle.resume();
          });
        }
        auto await_resume() const noexcept -> ResponseVariant {
          // ...error handling...
          auto body = reply->readAll();
          return QJsonDocument::fromJson(body);
        }
      };
      return Awaiter{reply};
    }
  };
};
