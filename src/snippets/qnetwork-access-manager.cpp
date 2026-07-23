// #region qnam-api
class QNetworkAccessManager {
    // ...
    QNetworkReply *get(const QNetworkRequest &request);
};
// #endregion qnam-api

// #region qnam-start
auto request = QNetworkRequest{url};
auto *reply = networkAccessManager->get(request);
QObject::connect(reply, &QNetworkReply::finished, [reply]() {
    // The response arrives here later.
});
// #endregion qnam-start

// #region qnam-response
QObject::connect(reply, &QNetworkReply::finished, [reply]() {
    if (auto networkError = reply->error();
        networkError != QNetworkReply::NoError) {
        // Handle transport failure.
    }
    if (auto httpStatus = reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute).toInt();
        httpStatus < 200 || httpStatus >= 300) {
        // Handle HTTP failure.
    }
    auto parseError = QJsonParseError{};
    auto jsonDocument = QJsonDocument::fromJson(
        reply->readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        // Handle malformed JSON.
    }
    process(jsonDocument);
    runNextRequest();
});
// #endregion qnam-response

// #region refresh-coroutine
auto refreshFromServer() -> NetworkTask {
    markUpdating();

    auto user = co_await networking.get(makeCurrentUserRequest());
    if (!applyCurrentUserResponse(user))
        co_return markStale();

    auto activities = co_await networking.get(makeAllActivitiesRequest());
    if (!applyAllActivitiesResponse(activities))
        co_return markStale();

    auto tracker = co_await networking.get(makeCurrentTimeTrackerRequest());
    if (!applyCurrentTimeTrackerResponse(expectNotFound(tracker)))
        co_return markStale();

    markLive();
}
// #endregion refresh-coroutine
