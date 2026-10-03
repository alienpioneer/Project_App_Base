/**
 * @file IDataReadUIService.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "data/IDataReadUIService.hpp"

namespace Data
{

int IDataReadUIService::getEquipementId() noexcept
{
    return m_equipementId.load();
}

size_t IDataReadUIService::getMaxFps() noexcept
{
    return m_MAX_FPS.load();
}

std::string IDataReadUIService::getDrmDispalyFile() noexcept
{
    return m_DRM_DISPLAY_FILE.get();
}

std::string IDataReadUIService::getDrmInputFile() noexcept
{
    return m_DRM_INPUT_FILE.get();
}

bool IDataReadUIService::getShowCursor() noexcept
{
    return m_SHOW_CURSOR.load();
}

bool IDataReadUIService::getMirrorXAxis() noexcept
{
    return m_MIRROR_CURSOR_X_AXIS.load();
}

bool IDataReadUIService::getMirrorYAxis() noexcept
{
    return m_MIRROR_CURSOR_Y_AXIS.load();
}

int IDataReadUIService::getIndevScrollLimit() noexcept
{
    return m_INDEV_SCROLL_LIMIT.load();
}

int IDataReadUIService::getGestureMinVelocity() noexcept
{
    return m_GESTURE_MIN_VELOCITY.load();
}

int IDataReadUIService::getGestureMinDistance() noexcept
{
    return m_GESTURE_MIN_DISTANCE.load();
}

size_t IDataReadUIService::getMinPeriodFrameUpdate() noexcept
{
    return m_MIN_PERIOD_FRAME_UPDATE.load();
}

size_t IDataReadUIService::getScreenWidth() noexcept
{
    return m_SCREEN_WIDTH.load();
}

size_t IDataReadUIService::getScreenHeight() noexcept
{
    return m_SCREEN_HEIGHT.load();
}

}// namespace Data