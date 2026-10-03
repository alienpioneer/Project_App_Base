/**
 * @file AccessService.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include <services/AccessService.hpp>

#include <chrono>
#include <string>
#include <utility>
#include <thread>
#include <openssl/sha.h>

#include "utils/logger.hpp"

namespace Access
{

AccessService::AccessService(std::shared_ptr<Core::EventManager> manager, uint8_t maxEventsPerCycle)
    :   ServiceBaseQueue(staticId, std::move(manager), maxEventsPerCycle)
{
    for (const auto& level: Data::IDataReadAccessService::getUsers())
    {
        // LOG_DEBUG("AccessService::AccessService() Current user: "+level.first+" = "+level.second);

        auto hashPair = split_string(level.second);

        m_usersHash[level.first] = hashPair.first; 
        m_usersSalt[level.first] = hashPair.second; 
    }
}

AccessService::~AccessService() = default;

void AccessService::init() noexcept
{
    subscribe(Core::EventType::INITIALIZED);
    subscribe(Core::EventType::AccessRequest);
    subscribe(Core::EventType::PasswordResponse);
}

void AccessService::dispatch(const Core::Event& event)
{
    switch(event.type())
    {
        case Core::EventType::INITIALIZED:
            LOG_DEBUG("AccessService::dispatch() Initialized");
        break;

        case Core::EventType::AccessRequest:
            onAccessRequest(event);
        break;

        case Core::EventType::PasswordResponse:
            onPasswordResponse(event);
        break;

        default: 
            break;
    }
}

void AccessService::onAccessRequest(const Core::Event& event)
{   
    // Send PasswordRequest Event with CallingService Id - TBC
    Core::Event passwordRequest(Core::EventType::PasswordRequest, id());
    passwordRequest.setPayload(static_cast<int>(event.publisherId()));
    sendEvent(passwordRequest);
}

void AccessService::onPasswordResponse(const Core::Event& event)
{
    const auto* payload = std::get_if<std::string>(&event.payload());

    if (!payload)
    {
        LOG_ERROR("AccessService::onPasswordResponse() Invalid PasswordEntered payload from UIService!");
        return;
    }

    std::string password = *payload;

    std::string response{"FAILURE"};

    LOG_DEBUG("AccessService::onPasswordResponse() PasswordResponse password : " + password);

    for (const auto& user: m_usersSalt)
    {
        // Check the default user first
        if (password == user.first)
        {
            response = user.first;
            break;
        }

        std::string passwordHash = computeHashWithSalt(password, user.second);

        if(passwordHash == m_usersHash.at(user.first))
        {
            response = user.first;
            break;
        }
    }

    auto accessResponse = Core::Event(Core::EventType::AccessResponse);
    accessResponse.setPayload(response);
    sendEvent(accessResponse);
}

std::string AccessService::computeHashWithSalt(const std::string& password, const std::string& salt) noexcept
{
    // Combine them: "password" + "salt"
    std::string salted_input = password + salt;
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(salted_input.c_str()), salted_input.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) 
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}


std::pair<std::string, std::string> AccessService::split_string(const std::string& s) noexcept
{
    auto pos = s.find(':');

    if (pos != std::string::npos) 
    {
        return {s.substr(0, pos), s.substr(pos + 1)};
    }

    return {s, {}};
}

}// namespace Access