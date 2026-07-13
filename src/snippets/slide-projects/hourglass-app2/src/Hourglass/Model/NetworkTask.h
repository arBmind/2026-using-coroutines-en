// Excerpt from NetworkTask::promise_type in the Hourglass app.

        auto await_transform(QNetworkReply *reply) {
            struct NetworkReplyAwaiter {
                QNetworkReply *reply;

                // #region qt-reply-await-bridge
                [[nodiscard]] bool await_ready() const noexcept { return nullptr == reply || reply->isFinished(); }

                // NOLINTNEXTLINE(readability-make-member-function-const)
                void await_suspend(std::coroutine_handle<> handle) noexcept {
                    QObject::connect(reply, &QNetworkReply::finished, [handle]() { handle.resume(); });
                }
                // #endregion qt-reply-await-bridge

                [[nodiscard]] auto await_resume() const noexcept -> NetworkResponse {
                    if (!reply->isFinished()) {
                        return NetworkAbort{};
                    }
                    if (auto networkError = reply->error(); networkError != QNetworkReply::NoError) {
                        return networkError;
                    }
                    if (auto httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                        httpStatus < 200 || httpStatus >= 300) {
                        return HttpStatus{httpStatus};
                    }
                    auto body = reply->readAll();
                    if (body.isEmpty()) {
                        return NetworkSuccess{};
                    }
                    auto parseError = QJsonParseError{};
                    auto document = QJsonDocument::fromJson(body, &parseError);
                    if (parseError.error != QJsonParseError::NoError) {
                        return parseError;
                    }
                    return document;
                }
            };
            return NetworkReplyAwaiter{reply};
        }
