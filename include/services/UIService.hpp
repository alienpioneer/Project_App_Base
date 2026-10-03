/**
 * @file UIService.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

extern "C"
{
#include <lvgl/lvgl.h>
}

#include <array>
#include <functional>
#include <memory>
#include <mutex>

#include "core/AVector.hpp"
#include "core/AMap.hpp"
#include "core/Event.hpp"
#include "core/ServiceBaseQueue.hpp"

#include "ui/UIStates.hpp"
#include "ui/components/ScreenNavigator.hpp"
#include "hal/display/LvglPlatform.hpp"

/**
 * 
 * LVGL button press
 *      |
 *      v
 * UIAction  -->  UIService::dispatchLvglEvent()
 *                    |
 *                    +--> navigation?  -->  ScreenNavigator
 *                    |
 *                    +--> system event?  -->  Core::EventManager (playAudio, setBacklight...)
 *
 * Core::EventManager
 *      |
 *      v
 * UIService::dispatch()  -->  specific screen handler (updateVolumeSlider, updateBacklightSlider...)
 *
 *
 * In this setup, C++ acts as the Host/Orchestrator (managing the DRM hardware, file descriptors, and the main loop), 
 * while LVGL acts as the Engine (managing its own widget tree).
 *
 * The "Clean Integration" Philosophy:
 *
 * - Ownership: LVGL owns the lv_obj_t pointers. We do not use std::unique_ptr because LVGL has its own internal memory manager (lv_mem).
 * - State: Use C++ Classes for Logic and Data, but keep UI handles as simple members.
 * - Threading: One thread owns the lv_timer_handler. C++ communicates with UI via a simple command queue or direct calls within that same thread.
 *
*/

namespace UI
{



constexpr std::size_t MAX_EVENT_HANDLERS = static_cast<std::size_t>(Core::EventType::EVENT_COUNT); 
 
using EventHandler = std::function<void(const Core::Event&)>;

class IBaseScreen;
class AuthPopup;

class UIService final : public Core::ServiceBaseQueue
{
public:
    static constexpr Core::ServiceId staticId = Core::ServiceId::UIService;

    explicit UIService(std::shared_ptr<Core::EventManager> manager, uint8_t maxEventsPerCycle=4);

    ~UIService();

    static void lvglEventDispatcher(lv_event_t* e);
    
    void dispatchLvglEvent(UIAction action, int value);

    // Process the corresponding action depending on the event
    void dispatch(const Core::Event& event) override;
    void init() noexcept override;
    void setup() override; 
    void run() override;

    void sendPasswordResponse(std::string_view password) noexcept;
    void resetAccessRequest() noexcept;
    void cancelAccessRequest() noexcept;

private:
    // SEND EVENTS
    void playAudio(std::string_view audioName) noexcept;
    void setAudioVolume(int volume) noexcept;
    void getAudioVolume() noexcept;
    void setBacklight(int intensity) noexcept;
    void getBacklight() noexcept;

    // EXTERNAL EVENT HANDLERS
    const EventHandler* getHandler(Core::EventType event) const;
    void registerHandler(Core::EventType event, EventHandler handler);

    // LVGL SPECIFICS
    void lvglPlatformInit();
    void updateFrame();

    // INTERNAL EVENT HANDLERS
    void onInitilized() noexcept;
    void onPasswordRequest() noexcept;
    void onAccessResponse(const Core::Event& event) noexcept;

    template <typename T>
    T* getScreenInstance(ScreenID id) const
    {
        auto& base = m_screens[static_cast<std::size_t>(id)];
        return static_cast<T*>(base.get());
    }

private:
    // POPUPS
    std::unique_ptr<AuthPopup> m_authorizationPopup;

    // SCREENS
    std::unique_ptr<ScreenNavigator> m_navigator;
    std::array<std::unique_ptr<IBaseScreen>, static_cast<std::size_t>(ScreenID::COUNT)> m_screens;
    Core::AMap<Core::EventType, EventHandler, MAX_EVENT_HANDLERS> m_eventHandlers;

    // Lvgl Platform specifics
    std::unique_ptr<LvglPlatform> m_platform;
    std::chrono::duration<int64_t, std::milli> m_frameUpdatePeriod;
    std::chrono::duration<int64_t, std::milli> m_minPeriodBetweenFrames;

    // States
    AppState m_currentState = AppState::Idle;
    bool m_advancedSettingsRequest{false};

    // MEMBERS
    std::mutex m_mutex;
};

}// namespace UI
