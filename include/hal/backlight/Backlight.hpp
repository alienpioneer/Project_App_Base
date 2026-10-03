/**
 * @file Backlight.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include <string_view>

#include "utils/PosixIO.hpp"
#include "utils/PosixDir.hpp"

namespace Hal
{

class IBacklight
{
public:
    virtual ~IBacklight() = default;

    virtual int getBrightness() noexcept = 0;
    virtual int getMaxBrightness() noexcept = 0;
    virtual int setBrightness(int value) noexcept = 0;
    virtual bool initOk() const noexcept = 0;
};

class Backlight final : public IBacklight
{
public:
    explicit Backlight(std::string_view devicePath, std::string_view deviceName, std::string_view brightnessName, std::string_view maxBrightnessName);

    ~Backlight() = default;

    int getBrightness() noexcept override;
    int getMaxBrightness() noexcept override;
    int setBrightness(int value) noexcept override;
    bool initOk() const noexcept override;

private:

    int readMaxBrightness() noexcept;

    Utils::PosixDir m_deviceDir;
    Utils::PosixIO m_brightnessDev;
    Utils::PosixIO m_maxBrightnessDev;

    int m_cachedMaxBrightness{-1};
};

}// namespace Hal
