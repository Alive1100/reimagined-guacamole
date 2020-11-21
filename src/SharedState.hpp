#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <spdlog/spdlog.h>

#include "WsConnection.hpp"

class SharedState
{
public:
    void add(WsConnection *session) {
        sessions_.insert({session, currentIdx_++});
        spdlog::info("Adding session, total: {}", sessions_.size());
    }
    void remove(WsConnection *session) {
        auto it = sessions_.find(session);
        if (it != sessions_.end())
        {
            int sessionNumber = it->second;
            sessions_.erase(it);
            spdlog::info("Removed session ({}), total: {}", sessionNumber, sessions_.size());
        } else {
            spdlog::error("{} Failed to find session", __func__);
        }
    }
    void send(WsConnection *sender, std::string message) {
        for (auto session: sessions_) {
            if (session.first != sender) {
                session.first->write(message);
            }
        }
    }
    int getSessionNumber(WsConnection *session) {
        auto it = sessions_.find(session);
        if (it != sessions_.end())
        {
            return it->second;
        } else {
            spdlog::error("{} Failed to find session", __func__);
        }
        return -1;
    }
private:
    std::unordered_map<WsConnection*, int> sessions_;
    int currentIdx_ = 0;
};