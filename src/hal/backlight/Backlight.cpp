/**
 * @file Backlight.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "hal/backlight/Backlight.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>

namespace Hal
{

// cppcheck-suppress passedByValue
Backlight::Backlight(std::string_view devicePath, std::string_view deviceName, std::string_view brightnessName, std::string_view maxBrightnessName)
    :   m_deviceDir{Utils::PosixDir::joinPath(devicePath,deviceName)},
        m_brightnessDev{m_deviceDir.fd(), brightnessName, O_RDWR},
        m_maxBrightnessDev{m_deviceDir.fd(), maxBrightnessName, O_RDONLY}
{
    m_cachedMaxBrightness = readMaxBrightness();
}

bool Backlight::initOk() const noexcept
{
    return m_deviceDir && m_brightnessDev && m_maxBrightnessDev;
}

int Backlight::getMaxBrightness() noexcept
{
    if (m_cachedMaxBrightness >= 0)
    {
        return m_cachedMaxBrightness;
    }

    int val = readMaxBrightness();

    if (val >= 0)
    {
        m_cachedMaxBrightness = val;
    }
        
    return val;
}

int Backlight::setBrightness(int value) noexcept
{
    if (!m_brightnessDev) 
    {
        return -EBADF;
    }

    std::string s = std::to_string(value);
    s.push_back('\n');
    ssize_t n = m_brightnessDev.write(s.c_str(), s.size());

    if (n < 0)
    {
        return n;
    } 

    if (n == 0)
    {
        return -EIO;
    } 

    return n;
}

int Backlight::readMaxBrightness() noexcept
{
    if (!m_maxBrightnessDev)
    {
        return -EBADF;
    }

    // sysfs is ASCII
    char buf[16];
    char* buff_end = nullptr;

    ssize_t n = m_maxBrightnessDev.pread(buf, sizeof(buf) - 1);

    if (n < 0)
    {
        return n;
    } 

    if (n == 0)
    {
        return -EIO;
    } 

    buf[n] = '\0';
    long val = std::strtol(buf, &buff_end, 10);

    // valid value 0 and parse failure are indistinguishable so must be filtered
    if (buff_end == buf || (*buff_end != '\n' && *buff_end != '\0'))
    {
        return -EINVAL;
    }

    return static_cast<int>(val);
}

int Backlight::getBrightness() noexcept
{
    if (!m_brightnessDev) 
    {
        return -EBADF;
    }
    
    // sysfs is ASCII
    char buf[16];
    char* buff_end = nullptr;

    ssize_t n = m_brightnessDev.pread(buf, sizeof(buf) - 1);

    if (n < 0)
    {
        return n;
    } 

    if (n == 0)
    {
        return -EIO;
    } 

    buf[n] = '\0';
    long val = std::strtol(buf, &buff_end, 10);

    // valid value 0 and parse failure are indistinguishable so must be filtered
    if (buff_end == buf || (*buff_end != '\n' && *buff_end != '\0'))
    {
        return -EINVAL;
    }

    return static_cast<int>(val);
}

}// namespace Hal
