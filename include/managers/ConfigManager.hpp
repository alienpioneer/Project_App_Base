/**
 * @file ConfigManager.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include <string_view>

#include "core/ConfigLoader.hpp"

namespace Data
{

class ConfigManager
{
public:
    explicit ConfigManager(std::string_view configFilePath);
    ~ConfigManager()=default;

private:
    void loadConfig() noexcept;
    void loadUsers() noexcept;
    
    // fallback method
    static void loadDefaultConfig() noexcept;

private:
    Core::ConfigLoader::ConfigResult m_configResult;
};

} // namespace Data
