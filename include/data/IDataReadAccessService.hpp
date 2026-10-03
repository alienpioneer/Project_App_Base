/**
 * @file IDataReadAccessService.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include "managers/DataManager.hpp"

namespace Data
{
    
class IDataReadAccessService: public DataManager
{

public:
    static UsersMap getUsers() noexcept;
};

}// namespace Data