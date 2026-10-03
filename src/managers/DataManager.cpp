/**
 * @file DataManager.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "managers/DataManager.hpp"

namespace Data
{
// ==================== USERS ========================

Core::AData<DataManager::UsersMap> DataManager::m_users{};

// ==================== CONFIG =======================
std::atomic<int> DataManager::m_equipementId{};

// Graphics
std::atomic<size_t> DataManager::m_MAX_FPS{};
std::atomic<size_t> DataManager::m_MIN_PERIOD_FRAME_UPDATE{};
Core::AData<std::string> DataManager::m_DRM_DISPLAY_FILE{};
Core::AData<std::string> DataManager::m_DRM_INPUT_FILE{};
std::atomic<bool> DataManager::m_SHOW_CURSOR{};
std::atomic<bool> DataManager::m_MIRROR_CURSOR_X_AXIS{};
std::atomic<bool> DataManager::m_MIRROR_CURSOR_Y_AXIS{};

std::atomic<int> DataManager::m_INDEV_SCROLL_LIMIT{};
std::atomic<int> DataManager::m_GESTURE_MIN_VELOCITY{};
std::atomic<int> DataManager::m_GESTURE_MIN_DISTANCE{};

std::atomic<size_t> DataManager::m_SCREEN_WIDTH{};
std::atomic<size_t> DataManager::m_SCREEN_HEIGHT{};

// Audio
Core::AData<std::string> DataManager::m_audioInterface{};
Core::AData<std::string> DataManager::m_audioMixer{};
Core::AData<std::string> DataManager::m_audioAltMixer{};
std::atomic<size_t> DataManager::m_MAX_ALSA_BUFFER_SIZE{};
std::atomic<size_t> DataManager::m_MAX_ALSA_PERIOD_SIZE{};

// Backlight
Core::AData<std::string> DataManager::m_backlightDevicePath{};
Core::AData<std::string> DataManager::m_backlightDeviceName{};
Core::AData<std::string> DataManager::m_backlightBrightnessName{};
Core::AData<std::string> DataManager::m_backlightMaxBrightnessName{};

void DataManager::shutdown()
{
    m_DRM_DISPLAY_FILE.shutdown();
    m_DRM_DISPLAY_FILE.shutdown();
    m_DRM_INPUT_FILE.shutdown();

    m_audioInterface.shutdown();
    m_audioMixer.shutdown();
    m_audioAltMixer.shutdown();
    
    m_backlightDevicePath.shutdown();
    m_backlightDeviceName.shutdown();
    m_backlightBrightnessName.shutdown();
    m_backlightMaxBrightnessName.shutdown();
};

}// namespace Data

