/**
 * @file UIStates.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include <cstdint>

namespace UI
{

enum class ScreenID : uint8_t 
{
    Back = 0,
    Main,
    Settings,

    AuthPopup,

    // Do NOT remove, add above this line !
    COUNT
};


enum class UIAction : uintptr_t 
{
    // Screens navigation
    ToMain = 0,
    Back,
    ToSettings,

    // UI updates
    SetAudioVolume,
    SetBacklight,

    AccessRequest,

    Reset,
    ToggleTheme,

    RequestShutdown,

    // Do NOT remove, add above this line !
    COUNT
};

enum class AppState : uint8_t
{ 
    Idle, 
    Running,
    Error,

    // Do NOT remove, add more states above this line !
    COUNT
};


}// namespace UI

