#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include <spdlog/spdlog.h>

#include "WsConnection.hpp"

class SharedState
{
public:
    void add(WsConnection *session) {
        sessions_.insert(session);
        spdlog::info("Adding session, total: {}", sessions_.size());
    }
    void remove(WsConnection *session) {
        sessions_.erase(session);
        spdlog::info("Removing session, total: {}", sessions_.size());
    }
    void send(WsConnection *sender, std::string message) {
        for (auto session: sessions_) {
            if (session != sender) {
                session->write(message);
            }
        }
    }

private:
    std::unordered_set<WsConnection*> sessions_;
    int currentIdx_ = 0;
};