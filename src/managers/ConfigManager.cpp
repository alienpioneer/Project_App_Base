/**
 * @file ConfigManager.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "managers/ConfigManager.hpp"

#include "data/IDataWriteConfig.hpp"
#include "utils/logger.hpp"

namespace Data
{

ConfigManager::ConfigManager(std::string_view configFilePath)
    : m_configResult{Core::ConfigLoader::load(configFilePath)}
{
    
    if(m_configResult.error == Core::ConfigError::FileError)
    {
        // TODO Display popup Error screen
        LOG_ERROR("ConfigManager::ConfigManager() Error reading " +std::string(configFilePath) + " configuration file !");
        loadDefaultConfig();
        return;
    }

    if((m_configResult.error != Core::ConfigError::Ok) && (m_configResult.error != Core::ConfigError::FileError))
    {
        // TODO Display popup Error screen
        LOG_ERROR("ConfigManager::ConfigManager() Load configuration error " + std::string(Core::ConfigLoader::errorToStr(m_configResult.error)) + " at line " + std::to_string(m_configResult.errorLine));
        return;
    }

    loadConfig();

    loadUsers();
}

void ConfigManager::loadUsers() noexcept
{
    const Core::Config::Section* usersSection = m_configResult.config->getSection("USERS");

    if (!usersSection)
    {
        LOG_ERROR("ConfigManager::loadUsers() Fail to get MESSAGES section from the can config file.");
        return;
    }
    
    IDataWriteConfig::UsersMap users;

    // cppcheck-suppress unassignedVariable
    for (const auto& [key, value] : *usersSection)
    {   
        users[key] = value;
    }

    IDataWriteConfig::setUserLevels(users);
}

void ConfigManager::loadConfig() noexcept
{
    int equipmentId = 0;
    if (m_configResult.config->getInt("ID", "equipement_id", equipmentId))
    {
        IDataWriteConfig::setEquipementId(equipmentId);
        // std::cout << "equipmentId " << equipmentId << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get ID/EQUIPEMENT_ID from config. Defaulting to 0.");
        IDataWriteConfig::setEquipementId(0);
    }

    int screenWidth = 0;
    if (m_configResult.config->getInt("GRAPHICS", "WIDTH", screenWidth))
    {
        IDataWriteConfig::setScreenWidth(screenWidth);
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get GRAPHICS/WIDTH from config. Defaulting to 600.");
        IDataWriteConfig::setScreenWidth(600);
    }

    int screenHeight = 0;
    if (m_configResult.config->getInt("GRAPHICS", "HEIGHT", screenHeight))
    {
        IDataWriteConfig::setScreenHeight(screenHeight);
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get GRAPHICS/HEIGHT from config. Defaulting to 600.");
        IDataWriteConfig::setScreenHeight(400);
    }
    
    int maxFps = 0;
    if (m_configResult.config->getInt("GRAPHICS", "MAX_FPS", maxFps))
    {
        IDataWriteConfig::setMaxFps(maxFps);
        // std::cout << "maxFps " << maxFps << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get GRAPHICS/MAX_FPS from config. Defaulting to 40.");
        IDataWriteConfig::setMaxFps(40);
    }

    int minPeriod = 0;
    if (m_configResult.config->getInt("GRAPHICS", "MIN_PERIOD_FRAME_UPDATE", minPeriod))
    {
        IDataWriteConfig::setMinFrameUpdate(minPeriod);
        // std::cout << "minPeriod " << minPeriod << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get GRAPHICS/MIN_PERIOD_FRAME_UPDATE from config. Defaulting to 5.");
        IDataWriteConfig::setMinFrameUpdate(5);
    }

    std::string drmDispalyFile = m_configResult.config->get("GRAPHICS", "drm_display_file");
    if (!drmDispalyFile.empty())
    {
        IDataWriteConfig::setDrmDispalyFile(drmDispalyFile);
        // std::cout << "drmDispalyFile " << drmDispalyFile << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get GRAPHICS/drm_display_file from config. Defaulting to /dev/dri/card0");
        IDataWriteConfig::setDrmDispalyFile("/dev/dri/card0");
    }

    std::string drmInputFile = m_configResult.config->get("GRAPHICS", "drm_input_file");
    if (!drmInputFile.empty())
    {
        IDataWriteConfig::setDrmInputFile(drmInputFile);
        // std::cout << "drmInputFile " << drmInputFile << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get GRAPHICS/drm_input_file from config. Defaulting to /dev/input/event1");
        IDataWriteConfig::setDrmInputFile("/dev/input/event1");
    }

    bool showCursor = false;
    if (m_configResult.config->getBool("LVGL", "SHOW_CURSOR", showCursor))
    {
        IDataWriteConfig::setShowCursor(showCursor);
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get LVGL/SHOW_CURSOR from config. Defaulting to false.");
        IDataWriteConfig::setShowCursor(false);
    }

    bool mirror_x_axis = false;
    if (m_configResult.config->getBool("LVGL", "MIRROR_CURSOR_X_AXIS", mirror_x_axis))
    {
        IDataWriteConfig::setMirrorXAxis(mirror_x_axis);
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get LVGL/MIRROR_CURSOR_X_AXIS from config. Defaulting to false.");
        IDataWriteConfig::setMirrorXAxis(false);
    }

    bool mirror_y_axis = false;
    if (m_configResult.config->getBool("LVGL", "MIRROR_CURSOR_Y_AXIS", mirror_y_axis))
    {
        IDataWriteConfig::setMirrorYAxis(mirror_y_axis);
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get LVGL/MIRROR_CURSOR_Y_AXIS from config. Defaulting to false.");
        IDataWriteConfig::setMirrorYAxis(false);
    }

    int indevScrollLimit = 0;
    if (m_configResult.config->getInt("LVGL", "INDEV_SCROLL_LIMIT", indevScrollLimit))
    {
        IDataWriteConfig::setIndevScrollLimit(indevScrollLimit);
        // std::cout << "indevScrollLimit " << indevScrollLimit << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get LVGL/INDEV_SCROLL_LIMIT from config. Defaulting to 100.");
        IDataWriteConfig::setIndevScrollLimit(100);
    }

    int gestureMinVelocity = 0;
    if (m_configResult.config->getInt("LVGL", "GESTURE_MIN_VELOCITY", gestureMinVelocity))
    {
        IDataWriteConfig::setGestureMinVelocity(gestureMinVelocity);
        // std::cout << "gestureMinVelocity " << gestureMinVelocity << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get LVGL/GESTURE_MIN_VELOCITY from config. Defaulting to 3.");
        IDataWriteConfig::setGestureMinVelocity(3);
    }

    int gestureMinDistance = 0;
    if (m_configResult.config->getInt("LVGL", "GESTURE_MIN_DISTANCE", gestureMinDistance))
    {
        IDataWriteConfig::setGestureMinDistance(gestureMinDistance);
        // std::cout << "gestureMinDistance " << gestureMinDistance << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get LVGL/GESTURE_MIN_DISTANCE from config. Defaulting to 50.");
        IDataWriteConfig::setGestureMinDistance(50);
    }

    std::string audioInterface = m_configResult.config->get("AUDIO", "audio_interface");
    if (!audioInterface.empty())
    {
        IDataWriteConfig::setAudioInterface(audioInterface);
        // std::cout << "audioInterface " << audioInterface << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get AUDIO/audio_interface from config. Defaulting to hw:0,0.");
        IDataWriteConfig::setAudioInterface("hw:0,0");
    }

    std::string audioMixer = m_configResult.config->get("AUDIO", "audio_mixer");
    if (!audioMixer.empty())
    {
        IDataWriteConfig::setAudioMixer(audioMixer);
        // std::cout << "audioMixer " << audioMixer << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get AUDIO/audio_mixer from config. Defaulting to Master.");
        IDataWriteConfig::setAudioMixer("Master");
    }

    std::string audioAltMixer = m_configResult.config->get("AUDIO", "audio_alt_mixer");
    if (!audioAltMixer.empty())
    {
        IDataWriteConfig::setAudioAltMixer(audioAltMixer);
        // std::cout << "audioAltMixer " << audioAltMixer << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get AUDIO/audio_alt_mixer from config. Defaulting to Master.");
        IDataWriteConfig::setAudioAltMixer("Master");
    }

    int maxAlsaBufferSize = 0;
    if (m_configResult.config->getInt("AUDIO", "MAX_ALSA_BUFFER_SIZE", maxAlsaBufferSize))
    {
        IDataWriteConfig::setAudioMaxAlsaBufferSize(maxAlsaBufferSize);
        // std::cout << "maxAlsaBufferSize " << maxAlsaBufferSize << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get AUDIO/MAX_ALSA_BUFFER_SIZE from config. Defaulting to 1024.");
        IDataWriteConfig::setAudioMaxAlsaBufferSize(1024);
    }

    int maxAlsaPeriodSize = 0;
    if (m_configResult.config->getInt("AUDIO", "MAX_ALSA_PERIOD_SIZE", maxAlsaPeriodSize))
    {
        IDataWriteConfig::setAudioMaxAlsaPeriodSize(maxAlsaPeriodSize);
        // std::cout << "maxAlsaPeriodSize " << maxAlsaPeriodSize << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get AUDIO/MAX_ALSA_PERIOD_SIZE from config. Defaulting to 512.");
        IDataWriteConfig::setAudioMaxAlsaPeriodSize(512);
    }

    std::string backlightDevicePath = m_configResult.config->get("BACKLIGHT", "device_path");
    if (!backlightDevicePath.empty())
    {
        IDataWriteConfig::setBacklightDevicePath(backlightDevicePath);
        // std::cout << "backlightDevicePath " << backlightDevicePath << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get BACKLIGHT/device_path from config. Defaulting to /sys/class/backlight.");
        IDataWriteConfig::setBacklightDevicePath("/sys/class/backlight");
    }

    std::string backlightDevName = m_configResult.config->get("BACKLIGHT", "device_name");
    if (!backlightDevName.empty())
    {
        IDataWriteConfig::setBacklightDeviceName(backlightDevName);
        // std::cout << "backlightDevName " << backlightDevName << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get BACKLIGHT/device_name from config. Defaulting to backlight.");
        IDataWriteConfig::setBacklightDeviceName("backlight");
    }

    std::string brightness = m_configResult.config->get("BACKLIGHT", "brightness_name");
    if (!brightness.empty())
    {
        IDataWriteConfig::setBacklightBrightnessName(brightness);
        // std::cout << "brightness " << brightness << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get BACKLIGHT/brightness_name from config. Defaulting to brightness.");
        IDataWriteConfig::setBacklightBrightnessName("brightness");
    }

    std::string maxBrightness = m_configResult.config->get("BACKLIGHT", "max_brightness_name");
    if (!maxBrightness.empty())
    {
        IDataWriteConfig::setBacklightMaxBrightnessName(maxBrightness);
        // std::cout << "maxBrightness " << maxBrightness << std::endl;
    }
    else
    {
        LOG_ERROR("ConfigManager::loadConfig() Failed to get BACKLIGHT/max_brightness_name from config. Defaulting to max_brightness.");
        IDataWriteConfig::setBacklightMaxBrightnessName("max_brightness");
    }
}   

void ConfigManager::loadDefaultConfig() noexcept
{
    IDataWriteConfig::setEquipementId(0);

    IDataWriteConfig::setMaxFps(40);

    IDataWriteConfig::setMinFrameUpdate(5);

    IDataWriteConfig::setDrmDispalyFile("/dev/dri/card0");

    IDataWriteConfig::setDrmInputFile("/dev/input/event1");

    IDataWriteConfig::setIndevScrollLimit(100);

    IDataWriteConfig::setGestureMinVelocity(3);

    IDataWriteConfig::setGestureMinDistance(50);

    IDataWriteConfig::setAudioInterface("default");

    IDataWriteConfig::setAudioMixer("Master");

    IDataWriteConfig::setAudioAltMixer("Master");

    IDataWriteConfig::setAudioMaxAlsaBufferSize(1024);

    IDataWriteConfig::setAudioMaxAlsaPeriodSize(512);

    IDataWriteConfig::setBacklightDevicePath("/sys/class/backlight");

    IDataWriteConfig::setBacklightDeviceName("backlight");

    IDataWriteConfig::setBacklightBrightnessName("brightness");

    IDataWriteConfig::setBacklightMaxBrightnessName("max_brightness");

}

} // namespace Data
