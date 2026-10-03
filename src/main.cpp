/**
 * @file main.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include <condition_variable>
#include <mutex>

#include "Config.hpp"
#include "managers/DataManager.hpp"
#include "managers/ConfigManager.hpp"
#include "core/EventManager.hpp"
#include "core/ServiceManager.hpp"
#include "services/UIService.hpp"
#include "services/AudioService.hpp"
#include "services/BacklightService.hpp"
#include "services/AccessService.hpp"
#include "utils/logger.hpp"

#define LOG_IN_PRODUCTION 0   // 0 = console, 1 = file

static_assert(static_cast<std::size_t>(Core::EventType::EVENT_COUNT) <= Core::MAX_EVENTS);
static_assert(static_cast<std::size_t>(Core::ServiceId::COUNT) <= Core::MAX_SERVICES);

namespace Main
{

std::condition_variable shutdownCV;
std::mutex shutdownMtx;
bool running = true;

class ShutdownListener : public Core::ISubscriber
{
public:
    void onEvent(const Core::Event& e) override
    {
        if (e.type() == Core::EventType::SHUTDOWN)
        {
            {
                std::lock_guard<std::mutex> lock(shutdownMtx);
                running = false;
            }
            shutdownCV.notify_one();
        }
    }
};

} // namespace Main

int main() 
{

#if LOG_IN_PRODUCTION
    Utils::Logger::init("app_log", "/userdata/", Utils::OutputMode::File);
    Utils::Logger::enableConsole(false);
    Utils::Logger::setLevel(Utils::LogLevel::Info);
    Utils::Logger::enableMilliseconds(true);
#else
    Utils::Logger::init("app_log", "", Utils::OutputMode::Stdout);
    Utils::Logger::enableConsole(true);
    Utils::Logger::setLevel(Utils::LogLevel::Debug);
#endif

    LOG_INFO("======================= APPLICATION START ==========================");
    {
        Data::DataManager dataManager;
        Data::ConfigManager configurationManager("app.ini");

        std::shared_ptr<Core::EventManager> eventManager = std::make_shared<Core::EventManager>();
        Core::ServiceManager serviceManager(eventManager);
        
        // Register shoutdown listner
        auto shutdownListener = std::make_shared<Main::ShutdownListener>();
        eventManager->subscribe(Core::EventType::SHUTDOWN, shutdownListener);
        
        // This MUST be the first service that starts
        serviceManager.registerService<UI::UIService>();

        serviceManager.registerService<Audio::AudioService>();
        serviceManager.registerService<Backlight::BacklightService>();
        serviceManager.registerService<Access::AccessService>();
        
        // TODO Add here the rest of the services

        // Emit after all services registration
        eventManager->postEvent(Core::Event{Core::EventType::INITIALIZED});

        LOG_INFO("======================= APPLICATION INITIALIZED =====================");

        std::unique_lock<std::mutex> lock(Main::shutdownMtx);

        while (Main::running) 
        {
            Main::shutdownCV.wait(lock, []
            {
                return !Main::running;
            });
        }

        LOG_INFO("======================= APPLICATION SHUTDOWN =======================");

        LOG_INFO("ServiceManager.shutdown()");
        serviceManager.shutdown();

        LOG_INFO("DataManager.shutdown()");
        Data::DataManager::shutdown();
    }

    return 0;
}
