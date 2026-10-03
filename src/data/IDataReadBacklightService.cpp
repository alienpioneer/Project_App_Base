/**
 * @file IDataReadBacklightService.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "data/IDataReadBacklightService.hpp"

namespace Data
{

std::string IDataReadBacklightService::getBacklightDevicePath() noexcept
{
    return m_backlightDevicePath.get();
}

std::string IDataReadBacklightService::getBacklightDeviceName() noexcept
{
    return m_backlightDeviceName.get();
}

std::string IDataReadBacklightService::getBacklightBrightnessName() noexcept
{
    return m_backlightBrightnessName.get();
}

std::string IDataReadBacklightService::getBacklightMaxBrightnessName() noexcept
{
    return m_backlightMaxBrightnessName.get();
}

}// namespace Data