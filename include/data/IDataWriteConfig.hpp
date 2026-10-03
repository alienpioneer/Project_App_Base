/**
 * @file IDataWriteConfig.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include "managers/DataManager.hpp"

namespace Data
{
    
class IDataWriteConfig: public DataManager
{

public:
    static void setEquipementId(int value) noexcept;

    static void setMaxFps(size_t value) noexcept;
    static void setMinFrameUpdate(size_t value) noexcept;

    static void setScreenWidth(size_t value) noexcept;
    static void setScreenHeight(size_t value) noexcept;

    static void setDrmDispalyFile(std::string value) noexcept;
    static void setDrmInputFile(std::string value) noexcept;

    static void setShowCursor(bool value) noexcept;
    static void setMirrorXAxis(bool value) noexcept;
    static void setMirrorYAxis(bool value) noexcept;

    static void setIndevScrollLimit(int value) noexcept;
    static void setGestureMinVelocity(int value) noexcept;
    static void setGestureMinDistance(int value) noexcept;

    static void setAudioInterface(std::string value) noexcept;
    static void setAudioMixer(std::string value) noexcept;
    static void setAudioAltMixer(std::string value) noexcept;
    static void setAudioMaxAlsaBufferSize(size_t value) noexcept;
    static void setAudioMaxAlsaPeriodSize(size_t value) noexcept;

    static void setBacklightDevicePath(std::string value) noexcept;
    static void setBacklightDeviceName(std::string value) noexcept;
    static void setBacklightBrightnessName(std::string value) noexcept;
    static void setBacklightMaxBrightnessName(std::string value) noexcept;

    static void setUserLevels(UsersMap map) noexcept;

};

}// namespace Data