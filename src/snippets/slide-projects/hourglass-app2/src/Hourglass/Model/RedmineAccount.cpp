// Coroutine excerpts from the Hourglass app.

auto RedmineAccount::refreshFromServerImpl() -> NetworkTask {
    auto &networking = *RedmineNetworking::instance();

    setActivity(Activity::Reading);

    if (auto userSuccess =
            applyCurrentUserResponse(processJsonErrors(co_await networking.get(makeCurrentUserRequest())));
        !userSuccess) {
        markStale();
        co_return;
    }
    if (auto activitySuccess =
            applyAllActivitiesResponse(processJsonErrors(co_await networking.get(makeAllActivitiesRequest())));
        !activitySuccess) {
        markStale();
        co_return;
    }
    if (auto trackerSuccess = applyCurrentTimeTrackerResponse(
            expectNotFoundErrors(co_await networking.get(makeCurrentTimeTrackerRequest())));
        !trackerSuccess) {
        markStale();
        co_return;
    }

    setStatus(Status::Live);
    setActivity(Activity::Idle);
}

// #region start-time-tracker-coroutine
auto RedmineAccount::startTimeTrackerImpl() -> NetworkTask {
    auto &networking = *RedmineNetworking::instance();

    setActivity(Activity::Writing);

    if (auto success = applyCurrentTimeTrackerResponse(
            processJsonErrors(co_await networking.post(makeStartTimeTrackerRequest(), "{}")));
        !success) {
        markStale();
        co_return;
    }

    if (m_timeTracker != nullptr && m_timeTracker->issueId() > 0) {
        auto id = static_cast<int>(m_timeTracker->issueId());
        if (m_issues->byId(id) == nullptr) {
            std::ignore = applyResolveIssuesResponse(
                processJsonErrors(co_await networking.get(makeResolveIssuesRequest({QString::number(id)}))));
        }
    }

    setActivity(Activity::Idle);
}
// #endregion start-time-tracker-coroutine
