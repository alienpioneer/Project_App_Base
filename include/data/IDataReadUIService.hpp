/**
 * @file IDataReadUIService.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include "managers/DataManager.hpp"

namespace Data
{
    
class IDataReadUIService: public DataManager
{

public:
    static int getEquipementId() noexcept;
    static size_t getMaxFps() noexcept;
    static size_t getMinPeriodFrameUpdate() noexcept;
    static std::string getDrmDispalyFile() noexcept;
    static std::string getDrmInputFile() noexcept;
    static size_t getScreenWidth() noexcept;
    static size_t getScreenHeight() noexcept;

    static bool getShowCursor() noexcept;
    static bool getMirrorXAxis() noexcept;
    static bool getMirrorYAxis() noexcept;

    static int getIndevScrollLimit() noexcept;
    static int getGestureMinVelocity() noexcept;
    static int getGestureMinDistance() noexcept;
};

}// namespace Data

