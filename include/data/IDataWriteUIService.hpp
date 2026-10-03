/**
 * @file IDataWriteUIService.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include "managers/DataManager.hpp"

namespace Data
{

class IDataWriteUIService: public DataManager
{

public:
    static void setCanDlcChangedValue(bool value) noexcept;

};

}// namespace Data