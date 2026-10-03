/**
 * @file DataManager.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include <atomic>
#include <variant>

#include <core/AMap.hpp>
#include "core/AData.hpp"
#include "ui/UIStates.hpp"

/*
All the variables, parameters,etc are stored as static members.
To read, use only the service specific interface IDataReadService for a clean separation
To write, use also specilized interfaces reserved for each service, ex: IDataWriteService
*/

namespace Data
{

constexpr std::size_t MAX_USER_LEVELS = 10;


enum class DataReadStatus
{
    // Example of Data status if needed
    OK = 0,
    NOK,
    DataNotFound,
    InternalError,
    Empty,
    FileError,
    XmlError,
    CsvError
};

class DataManager
{

public:

    using UsersMap = Core::AMap<std::string, std::string, Data::MAX_USER_LEVELS>;

    DataManager()=default;
    virtual ~DataManager() = default;

    static void shutdown();

protected:
    // ==================== USERS ========================

    static Core::AData<UsersMap> m_users;

    // ==================== CONFIG =======================

    static std::atomic<int> m_equipementId;

    // Graphics
    static std::atomic<size_t> m_MAX_FPS;
    static std::atomic<size_t> m_MIN_PERIOD_FRAME_UPDATE;
    static Core::AData<std::string> m_DRM_DISPLAY_FILE;
    static Core::AData<std::string> m_DRM_INPUT_FILE;
    static std::atomic<bool> m_SHOW_CURSOR;
    static std::atomic<bool> m_MIRROR_CURSOR_X_AXIS;
    static std::atomic<bool> m_MIRROR_CURSOR_Y_AXIS;

    static std::atomic<int> m_INDEV_SCROLL_LIMIT;
    static std::atomic<int> m_GESTURE_MIN_VELOCITY;
    static std::atomic<int> m_GESTURE_MIN_DISTANCE;

    static std::atomic<size_t> m_SCREEN_WIDTH;
    static std::atomic<size_t> m_SCREEN_HEIGHT;

    // Audio
    static Core::AData<std::string> m_audioInterface;
    static Core::AData<std::string> m_audioMixer;
    static Core::AData<std::string> m_audioAltMixer;
    static std::atomic<size_t> m_MAX_ALSA_BUFFER_SIZE;
    static std::atomic<size_t> m_MAX_ALSA_PERIOD_SIZE;

    // Backlight
    static Core::AData<std::string> m_backlightDevicePath;
    static Core::AData<std::string> m_backlightDeviceName;
    static Core::AData<std::string> m_backlightBrightnessName;
    static Core::AData<std::string> m_backlightMaxBrightnessName;

    // ==================== CONTEXT =======================

    
};

} // namespace Data

