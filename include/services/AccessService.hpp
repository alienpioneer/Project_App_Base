/**
 * @file AccessService.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include "core/AMap.hpp"
#include "core/Event.hpp"
#include "core/ServiceBaseQueue.hpp"
#include "data/IDataReadAccessService.hpp"

/*

A Service needs authentication -> AccessRequest (event to AccessService) 
AccessService -> PasswordRequest to UIService
UIService -> PasswordEntered to AccessService

AccessService when requesting access:
1. Load stored_hash from file
2. if (hash_with_salt(input_from_lvgl, salt) == stored_hash) -> SUCCESS

AccessService -> AccessResponse (user_level or failure)

If AccessRequest comes from UIService, no PasswordRequest -> get password from AccessRequest event and send AccessResponse directly.

One service should send only one access request at a time !

*/

namespace Access
{

class AccessService final: public Core::ServiceBaseQueue
{
public:
    static constexpr Core::ServiceId staticId = Core::ServiceId::AccessService;

    explicit AccessService(std::shared_ptr<Core::EventManager> manager, uint8_t maxEventsPerCycle=4);

    ~AccessService();

    // Process the corresponding action depending on the event
    void dispatch(const Core::Event& event) override;

    void init() noexcept override;

private:
    // Events
    void onAccessRequest(const Core::Event& event);
    void onPasswordResponse(const Core::Event& event);

    // Internals
    std::string computeHashWithSalt(const std::string& password, const std::string& salt) noexcept;
    std::pair<std::string, std::string> split_string(const std::string& s) noexcept;

private:
    // Map user level to its corresponding salt
    Core::AMap<std::string, std::string, Data::MAX_USER_LEVELS> m_usersSalt;
    // Map user level to its corresponding hash
    Core::AMap<std::string, std::string, Data::MAX_USER_LEVELS> m_usersHash;
};

} // namespace Access