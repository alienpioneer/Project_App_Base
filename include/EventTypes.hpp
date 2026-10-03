/**
 * @file EventTypes.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

/*
Do not polute with a lot of events like for example:
EventType::PlayClickSound
EventType::PlayErrorSound
EventType::PlayStartupSound

Use EventType::AudioCpmmand and attach a payload describing what to play,
or play, stop, pause

Use commmon enums like:

enum class AudioClipId : uint8_t
{
    Click,
    Error,
    Startup,
    Notification,
    Count
};

to syncronize events/payloads

*/

namespace Core
{

enum class EventType : int
{
    // Do not remove, add events bellow
    DEFAULT = 0,
    // Do NOT remove, used for shutdown
    SHUTDOWN = 1,
    // For tests only, add events below !
    TestEvent = 2,

    INITIALIZED,

    AudioPlayCmd,
    AudioStopCmd,
    AudioPauseCmd,
    AudioResumeCmd,
    AudioVolumeCmd,   // also handles volume requests
    AudioVolResponse,

    BacklightCmd,     // also handles intensity requests
    BacklightResponse,

    AccessRequest,
    AccessResponse,
    PasswordRequest,
    PasswordResponse,

    // Add events here

    // Do NOT remove, DO NOT USE !!!
    EVENT_COUNT
};


}// namespace Core
