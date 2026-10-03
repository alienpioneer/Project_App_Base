/**
 * @file UIService.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "services/UIService.hpp"

#include <algorithm>
#include <cmath>
#include <thread>
#include <sstream>

#include "utils/logger.hpp"
#include "data/IDataReadUIService.hpp"
#include "data/IDataWriteUIService.hpp"
#include "ui/components/StyleManager.hpp"
#include "ui/screens/IBaseScreen.hpp"
#include "ui/screens/MainScreen.hpp"
#include "ui/screens/SettingsScreen.hpp"
#include "ui/screens/AuthPopup.hpp"

namespace UI
{
    
UIService::UIService(std::shared_ptr<Core::EventManager> manager, uint8_t maxEventsPerCycle)
    :   ServiceBaseQueue(staticId, std::move(manager), maxEventsPerCycle),
        m_currentState(AppState::Idle)
{}

void UIService::init() noexcept
{
    subscribe(Core::EventType::INITIALIZED);

    subscribe(Core::EventType::AudioVolResponse);
    subscribe(Core::EventType::BacklightResponse);

    subscribe(Core::EventType::AccessResponse);
    subscribe(Core::EventType::PasswordRequest);
}

// Setup is called in run before the while loop so the platform and the widgets are in the same thread !
void UIService::setup() 
{
    lvglPlatformInit();

    // Initilize StyleManager here, before instantiating the screens
    StyleManager::init();

    // Move screen initialization here in setup since they require an active LVGL instance
    m_screens[static_cast<size_t>(ScreenID::Main)] = std::make_unique<MainScreen>(this, nullptr);
    m_screens[static_cast<size_t>(ScreenID::Settings)] = std::make_unique<SettingsScreen>(this, nullptr);

    m_authorizationPopup = std::make_unique<AuthPopup>(this);
    m_authorizationPopup->hidePopup();

    // Register handlers for external events
    // registerHandler(Core::EventType::AudioVolResponse, [this](const Core::Event& e) {getScreenInstance<SettingsScreen>(ScreenID::Settings)->onUpdateVolumeSlider(e);});
    // registerHandler(Core::EventType::BacklightResponse, [this](const Core::Event& e) {getScreenInstance<SettingsScreen>(ScreenID::Settings)->onUpdateBacklightSlider(e);});
    registerHandler(Core::EventType::INITIALIZED, [this](const Core::Event&) {onInitilized();});
    // Access service
    registerHandler(Core::EventType::AccessResponse, [this](const Core::Event& e) {onAccessResponse(e);});
    registerHandler(Core::EventType::PasswordRequest, [this](const Core::Event&) {onPasswordRequest();});

    // Initilize the graphics
    lv_obj_invalidate(lv_scr_act());
    m_platform->step();

    // Setup navigator
    m_navigator = std::make_unique<ScreenNavigator>(m_screens);
    m_navigator->loadScreen(ScreenID::Main);
}

void UIService::dispatchLvglEvent(UIAction action, int value)
{
    if (m_currentState == AppState::Error && action != UIAction::Reset) 
    {
        return; // Block transitions during error
    }

    uint32_t current_index;

    switch(action) 
    {
        // Navigation
        case UIAction::ToMain:
            playAudio("back.wav");
            m_navigator->switchTo(ScreenID::Main);
        break;

        case UIAction::Back:
            playAudio("back.wav");
            m_navigator->switchTo(ScreenID::Back);
        break;

        case UIAction::ToSettings:
            playAudio("click.wav");
            getAudioVolume();
            getBacklight();
            m_navigator->switchTo(ScreenID::Settings);
        break;

        // ============= ACTIONS ===============

        case UIAction::AccessRequest:
            playAudio("bong.wav");
            sendEvent(Core::EventType::AccessRequest);
            resetAccessRequest();
        break;

        case UIAction::SetAudioVolume:
            setAudioVolume(value);
            playAudio("sens.wav");
        break;

        case UIAction::SetBacklight:
            setBacklight(value);
        break;

        case UIAction::ToggleTheme:
            // TODO
            StyleManager::setTheme(ThemeMode::Default);
        break;

        case UIAction::RequestShutdown:
            LOG_INFO("=================== SHUTDOWN ===================");
            sendEvent(Core::EventType::SHUTDOWN);
        break;

        default: 
        break; 
    }

    //TODO Add external actions    
}


// ============================================== SEND EVENTS ===========================================================================

// cppcheck-suppress passedByValue
void UIService::playAudio(std::string_view audioName) noexcept
{
    Core::Event audioEvent(Core::EventType::AudioPlayCmd, id());
    audioEvent.setPayload(audioName);
    sendEvent(audioEvent);
}

void UIService::setAudioVolume(int volume) noexcept
{
    if( volume < 0 || volume > 10 )
    {
        LOG_ERROR("UIService::setAudioVolume() Invalid volume value " + std::to_string(volume));
        return;
    }

    Core::Event audioEvent(Core::EventType::AudioVolumeCmd, id());
    audioEvent.setPayload(volume*10);

    sendEvent(audioEvent);
}

void UIService::setBacklight(int intensity) noexcept
{
    if( intensity < 0 || intensity > 10 )
    {
        LOG_ERROR("UIService::setBacklight() Invalid backlight intensity " + std::to_string(intensity));
        return;
    }

    Core::Event backlightEvent(Core::EventType::BacklightCmd, id());
    backlightEvent.setPayload(intensity*10);
    sendEvent(backlightEvent);
}

void UIService::getAudioVolume() noexcept
{
    Core::Event audioEvent(Core::EventType::AudioVolumeCmd, id());
    audioEvent.setPayload(-1);
    sendEvent(audioEvent);
}

void UIService::getBacklight() noexcept
{
    Core::Event backlightEvent(Core::EventType::BacklightCmd, id());
    backlightEvent.setPayload(-1);
    sendEvent(backlightEvent);
}

void UIService::sendPasswordResponse(std::string_view password) noexcept
{
    LOG_DEBUG("UIService::sendPasswordResponse() Current password " + std::string(password));

    auto passwordResponse = Core::Event(Core::EventType::PasswordResponse);
    passwordResponse.setPayload(password);
    sendEvent(passwordResponse);
}

void UIService::cancelAccessRequest() noexcept
{
    if(m_advancedSettingsRequest)
    {
        m_advancedSettingsRequest = false;
    }

    m_authorizationPopup->hidePopup();
}

void UIService::resetAccessRequest() noexcept
{
    m_advancedSettingsRequest = true;
}

// ============================================== EXTERNAL EVENT HANDLERS ===============================================================

void UIService::onInitilized() noexcept
{
    LOG_DEBUG("UIService::onInitilized() Initialized");
    // Perform startup operations
    setAudioVolume(4);
    setBacklight(5);
    getAudioVolume();
    getBacklight();
}

void UIService::onAccessResponse(const Core::Event& event) noexcept
{
    auto* payload = std::get_if<std::string>(&event.payload());

    if (!payload)
    {
        LOG_ERROR("UIService::onAccessResponse() Invalid AccessResponse payload from AccessService!");
        return;
    }

    std::string response = std::string(*payload);

    // LOG_DEBUG("UIService::onAccessResponse() m_advancedSettingsRequest "+std::to_string(m_advancedSettingsRequest));
    LOG_DEBUG("UIService::onAccessResponse() AccessResponse "+response);

    if(m_advancedSettingsRequest)
    {
        m_advancedSettingsRequest = false;

        if(response == "FAILURE")
        {
            m_authorizationPopup->onAuthFailure();
        }
        else
        {
            m_authorizationPopup->hidePopup();
            // TODO If not FAILURE, set the settings access level and change the screen
        }
    }
}

void UIService::onPasswordRequest() noexcept
{
    m_authorizationPopup->showPopup();
}

// ============================================== INTERNALS =============================================================================

void UIService::run()
{
    setup();

    auto nextUpdate = std::chrono::steady_clock::now();

    while(running())
    {
        nextUpdate += m_frameUpdatePeriod;
        
        // Process LVGL timers and rendering
        m_platform->step(); 

        // Process incoming Core::Events 
        dispatchLoop();

        updateFrame();

        auto now = std::chrono::steady_clock::now();

        if (now < nextUpdate)
        {
            std::this_thread::sleep_until(nextUpdate);
        }
        else if (now - nextUpdate < m_minPeriodBetweenFrames)
        {
            std::this_thread::sleep_for(m_minPeriodBetweenFrames - (now - nextUpdate));
        }
        else
        {
            nextUpdate = now + m_frameUpdatePeriod;
        }
    }

    if (m_platform)
    {
        m_platform->stop();
        // Force LvglPlatform destruction and implicitly call lv_deinit()
        m_platform.reset();
    }

    LOG_DEBUG("UIService::run() STOPPED !");
}

void UIService::dispatch(const Core::Event& event)
{   
    std::lock_guard<std::mutex> lock(m_mutex);

    auto* handler = getHandler(event.type());

    if (handler)
    {
        (*handler)(event);
    }
    else
    {
        LOG_ERROR("UIService::dispatch() No handler found for the event " + std::to_string(static_cast<int>(event.type())) );
    }
}

void UIService::updateFrame()
{
    // TODO
}

// The dispatcher is the only callback attatched to the buttons
void UIService::lvglEventDispatcher(lv_event_t* e)
{
    auto* service = static_cast<UIService*>(lv_event_get_user_data(e));

    lv_obj_t* target = lv_event_get_target_obj(e);

    auto action = static_cast<UIAction>(reinterpret_cast<uintptr_t>(lv_obj_get_user_data(target)));

    // Priviledge sliders for direct transmission
    int value = lv_obj_check_type(target, &lv_slider_class) ? lv_slider_get_value(target) : 0;

    // LOG_DEBUG("UIService::lvglEventDispatcher() Current action " + std::to_string(static_cast<int>(action)) );

    service->dispatchLvglEvent(action, value);
}

void UIService::lvglPlatformInit()
{
    // std::ostringstream oss;
    // oss << "[UI] LvglPlatform INIT thread=" << std::hex << std::this_thread::get_id();
    // LOG_DEBUG("UIService::lvglPlatformInit() " + oss.str() );

    size_t MAX_FPS = Data::IDataReadUIService::getMaxFps();
    size_t MIN_PERIOD_FRAME_UPDATE = Data::IDataReadUIService::getMinPeriodFrameUpdate();

    size_t SCREEN_WIDTH = Data::IDataReadUIService::getScreenWidth();
    size_t SCREEN_HEIGHT = Data::IDataReadUIService::getScreenHeight();
    bool SHOW_CURSOR = Data::IDataReadUIService::getShowCursor();
    bool MIRROR_X_AXIS = Data::IDataReadUIService::getMirrorXAxis();
    bool MIRROR_Y_AXIS = Data::IDataReadUIService::getMirrorYAxis();
    
    auto fpsPeriod = static_cast<int64_t>(1000/MAX_FPS);

    m_frameUpdatePeriod = std::chrono::milliseconds(fpsPeriod);

    m_minPeriodBetweenFrames = std::chrono::milliseconds(MIN_PERIOD_FRAME_UPDATE);

    m_platform = std::make_unique<LvglPlatform>(SCREEN_WIDTH, SCREEN_HEIGHT, 1, SHOW_CURSOR, false, MIRROR_X_AXIS, MIRROR_Y_AXIS);
}

const EventHandler* UIService::getHandler(Core::EventType event) const
{
    auto it = m_eventHandlers.find(event);

    if(it != m_eventHandlers.end())
    {
        return &it->second;
    }

    return nullptr;
}

void UIService::registerHandler(Core::EventType event, EventHandler handler)
{
    m_eventHandlers[event] = std::move(handler);
}

UIService::~UIService() = default;

}// namespace UI

