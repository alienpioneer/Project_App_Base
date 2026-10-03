/**
 * @file ServiceTypes.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once
#include <cstdint>

namespace Core
{

enum class ServiceId : uint8_t 
{
    BASE = 0,
    ServiceManager = 1,
    // For tests only
    TestService = 2,

    UIService,
    AudioService,
    BacklightService,

    AccessService,
    
    //ADD services here

    // Do NOT remove, DO NOT USE, add services above COUNT 
    COUNT  // number of services
};

}// namespace Core
