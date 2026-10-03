/**
 * @file IDataReadBacklightService.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

 #pragma once

#include "managers/DataManager.hpp"

namespace Data
{
    
class IDataReadBacklightService: public DataManager
{

public:
    static std::string getBacklightDevicePath() noexcept;
    static std::string getBacklightDeviceName() noexcept;
    static std::string getBacklightBrightnessName() noexcept;
    static std::string getBacklightMaxBrightnessName() noexcept;

};

}// namespace Data