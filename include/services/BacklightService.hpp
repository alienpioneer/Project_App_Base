/**
 * @file BacklightService.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include <string>

#include "core/AVector.hpp"
#include "core/Event.hpp"
#include "core/ServiceBaseQueue.hpp"
#include "hal/backlight/Backlight.hpp"

/*
TODO Docs
*/

namespace Backlight
{

class BacklightService final: public Core::ServiceBaseQueue
{
public:
    // Give a valid service Id in Core::ServiceId
    static constexpr Core::ServiceId staticId = Core::ServiceId::BacklightService;

    explicit BacklightService(std::shared_ptr<Core::EventManager> manager, uint8_t maxEventsPerCycle=4);

    ~BacklightService();

    // Process the corresponding action depending on the event
    void dispatch(const Core::Event& event) override;
    void init() noexcept override;

private:
    int getMaxBrightness();
    int getBrightness();
    void setBrightness(int percentage);

    // EVENTS
    void sendBacklightIntensity() noexcept;

    // CALLBACKS    
    void onBacklightCmd(const Core::Event& event);

    // INTERNALS
    void errnoHandle(int errValue) const;

    std::unique_ptr<Hal::IBacklight> m_driver;
    bool m_disabled{false};
};

}// Backlight