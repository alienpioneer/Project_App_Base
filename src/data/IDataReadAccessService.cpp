/**
 * @file IDataReadAccessService.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "data/IDataReadAccessService.hpp"

namespace Data
{

DataManager::UsersMap IDataReadAccessService::getUsers() noexcept
{
    return m_users.get();
}

}// namespace Data