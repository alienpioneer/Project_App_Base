/**
 * @file IDataWriteConfig.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "data/IDataWriteConfig.hpp"

namespace Data
{

void IDataWriteConfig::setEquipementId(int value) noexcept
{
    m_equipementId.store(value);
}

void IDataWriteConfig::setScreenWidth(size_t value) noexcept
{
    m_SCREEN_WIDTH.store(value);
}

void IDataWriteConfig::setScreenHeight(size_t value) noexcept
{
    m_SCREEN_HEIGHT.store(value);
}

void IDataWriteConfig::setMaxFps(size_t value) noexcept
{
    m_MAX_FPS.store(value);
}

void IDataWriteConfig::setMinFrameUpdate(size_t value) noexcept
{
    m_MIN_PERIOD_FRAME_UPDATE.store(value);
}

void IDataWriteConfig::setDrmDispalyFile(std::string value) noexcept
{
    m_DRM_DISPLAY_FILE.set(value);
}

void IDataWriteConfig::setDrmInputFile(std::string value) noexcept
{
    m_DRM_INPUT_FILE.set(value);
}

void IDataWriteConfig::setShowCursor(bool value) noexcept
{
    m_SHOW_CURSOR.store(value);
}

void IDataWriteConfig::setMirrorXAxis(bool value) noexcept
{
    m_MIRROR_CURSOR_X_AXIS.store(value);
}

void IDataWriteConfig::setMirrorYAxis(bool value) noexcept
{
    m_MIRROR_CURSOR_Y_AXIS.store(value);
}

void IDataWriteConfig::setIndevScrollLimit(int value) noexcept
{
    m_INDEV_SCROLL_LIMIT.store(value);
}

void IDataWriteConfig::setGestureMinVelocity(int value) noexcept
{
    m_GESTURE_MIN_VELOCITY.store(value);
}

void IDataWriteConfig::setGestureMinDistance(int value) noexcept
{
    m_GESTURE_MIN_DISTANCE.store(value);
}

void IDataWriteConfig::setAudioInterface(std::string value) noexcept
{
    m_audioInterface.set(value);
}

void IDataWriteConfig::setAudioMixer(std::string value) noexcept
{
    m_audioMixer.set(value);
}

void IDataWriteConfig::setAudioAltMixer(std::string value) noexcept
{
    m_audioAltMixer.set(value);
}

void IDataWriteConfig::setAudioMaxAlsaBufferSize(size_t value) noexcept
{
    m_MAX_ALSA_BUFFER_SIZE.store(value);
}

void IDataWriteConfig::setAudioMaxAlsaPeriodSize(size_t value) noexcept
{
    m_MAX_ALSA_PERIOD_SIZE.store(value);
}

void IDataWriteConfig::setBacklightDevicePath(std::string value) noexcept
{
    m_backlightDevicePath.set(value);
}

void IDataWriteConfig::setBacklightDeviceName(std::string value) noexcept
{
    m_backlightDeviceName.set(value);
}

void IDataWriteConfig::setBacklightBrightnessName(std::string value) noexcept
{
    m_backlightBrightnessName.set(value);
}

void IDataWriteConfig::setBacklightMaxBrightnessName(std::string value) noexcept
{
    m_backlightMaxBrightnessName.set(value);
}

void IDataWriteConfig::setUserLevels(UsersMap map) noexcept
{
    m_users.set(map);
}

}// namespace Data