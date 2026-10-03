/**
 * @file BacklightService.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include <services/BacklightService.hpp>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <thread>

#include "utils/logger.hpp"
#include "data/IDataReadBacklightService.hpp"

namespace Backlight
{

BacklightService::BacklightService(std::shared_ptr<Core::EventManager> manager, uint8_t maxEventsPerCycle)
    :   ServiceBaseQueue(staticId, std::move(manager), maxEventsPerCycle)
{
    std::string backlightDevicePath = Data::IDataReadBacklightService::getBacklightDevicePath();
    std::string backlightDeviceName = Data::IDataReadBacklightService::getBacklightDeviceName();
    std::string backlightBrightnessName = Data::IDataReadBacklightService::getBacklightBrightnessName();
    std::string backlightMaxBrightnessName = Data::IDataReadBacklightService::getBacklightMaxBrightnessName();

    m_driver = std::make_unique<Hal::Backlight>(backlightDevicePath, backlightDeviceName, backlightBrightnessName, backlightMaxBrightnessName);
}

BacklightService::~BacklightService() = default;

void BacklightService::init() noexcept
{
    subscribe(Core::EventType::INITIALIZED);
    subscribe(Core::EventType::BacklightCmd);

    if(!m_driver->initOk())
    {
        LOG_ERROR("BacklightService::init() Backlight driver initialization failure. Service is disabled.");
        m_disabled = true;
        return;
    }
}


// ============================================== EVENTS ===================================================================

void BacklightService::dispatch(const Core::Event& event)
{
    switch(event.type())
    {
        case Core::EventType::INITIALIZED:
            LOG_DEBUG("BacklightService::dispatch() Initialized");
        break;

        case Core::EventType::BacklightCmd:
            onBacklightCmd(event);
        break;

        default: 
        break;
    }
}

void BacklightService::onBacklightCmd(const Core::Event& event)
{
    if (m_disabled)
    {
        return;
    }

    LOG_TRACE("BacklightService::onSetBacklight() Received Backlight Event.");

    auto* payload = std::get_if<int>(&event.payload());

    if (!payload)
    {
        LOG_ERROR("BacklightService::onSetBacklight() Invalid backlight event payload "+std::to_string(*payload));
        return;
    }

    int intensity = *payload;

    if(intensity == -1)
    {
        sendBacklightIntensity();    
    }
    else
    {
        setBrightness(intensity);
    }
}

// Send backlight intensity event
void BacklightService::sendBacklightIntensity() noexcept
{
    LOG_TRACE("BacklightService::sendBacklightIntensity() Get intensity request.");

    Core::Event::Payload payload{};
    payload.dataA = getBrightness();
    payload.dataB = getMaxBrightness();

    Core::Event audioEvent(Core::EventType::BacklightResponse, id());
    audioEvent.setPayload(payload);
    sendEvent(audioEvent);
}

// ============================================== CONTROLS =================================================================

int BacklightService::getMaxBrightness() 
{   
    int value = m_driver->getMaxBrightness();

    if (value < 0)
    {
        errnoHandle(-value);
    }

    return value;
}

// Get hardware PWM value for backlight 
int BacklightService::getBrightness() 
{
    int value = m_driver->getBrightness();

    if (value < 0)
    {
        errnoHandle(-value);
    }

    return value;
}

void BacklightService::setBrightness(int percentage) 
{
    if (percentage < 0)
    {
        LOG_ERROR("BacklightService::setBrightness() Invalid percentage brightness value ! " + std::to_string(percentage));
        return;
    }

    int maxBrightness = getMaxBrightness();

    if (maxBrightness <= 0)
    {
        LOG_ERROR("BacklightService::setBrightness() Invalid max brightness value " + std::to_string(maxBrightness));
        return;
    } 

    // Keep a minimum intensity to avoid black screen !
    percentage = std::clamp(percentage, 10, 100);

    int normalValue = (static_cast<float>(percentage) / 100.0f) * maxBrightness;

    // Invert it because the hardware logic is flipped
    int invertedValue = maxBrightness - normalValue;

    LOG_TRACE("BacklightService::setBrightness() Input %: " + std::to_string(percentage)  + " | HW Value: " + std::to_string(invertedValue));

    int value = m_driver->setBrightness(invertedValue);

    if (value < 0)
    {
        errnoHandle(-value);
    }
}

void BacklightService::errnoHandle(int errValue) const
{
    LOG_ERROR("Operation failed with the message " + std::string(strerror(errValue))+" errno="+std::to_string(errValue));
}

}// namespace Backlight