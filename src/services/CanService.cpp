/**
 * @file CanService.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include <services/CanService.hpp>

#include <linux/can/raw.h>

#include <functional>
#include <chrono>
#include <cmath>
#include <string>
#include <cstring>
#include <cerrno>
#include <thread>
#include <iomanip>
#include <sstream>

#include "utils/logger.hpp"
#include "data/IDataReadCanService.hpp"
#include "data/IDataWriteCanService.hpp"

namespace Can
{

CanService::CanService(std::shared_ptr<Core::EventManager> manager, uint8_t maxEventsPerCycle)
    :   ServiceBaseQueue(staticId, std::move(manager), maxEventsPerCycle)
{
    std::string canInterfaceName = Data::IDataReadCanService::getCanInterface();
    uint32_t canBitrate = Data::IDataReadCanService::getCanBitrate();
    CAN_SAFE_FALLBACK_VALUE = Data::IDataReadCanService::getCanSafeValue();
    CAN_DLC_STATIC_NOISE_VALUE = Data::IDataReadCanService::getCanDlcNoiseValue();

    m_socket = std::make_unique<Hal::CANSocket>(canInterfaceName.c_str(), std::chrono::milliseconds(1));
    m_canDriver = std::make_unique<Hal::CanDriver>(canInterfaceName.c_str());
    m_codecs = std::make_unique<CanCodecs>();

    if (!m_codecs->loadFromConfig("can.ini"))
    {
        LOG_ERROR("Error loading CAN codecs! Service is compromised!");
    }

    // Create CAN heartbeat timer
    m_heartbeatTimer = std::make_unique<Core::ATimer>(std::chrono::milliseconds(3000), [this]() {onHeartbeatTimer();}, false);

    setupCanInterface(canBitrate);
}

CanService::~CanService()
{
    m_heartbeatTimer->stop();
}

// ================================================================== EVENTS =======================================================================

void CanService::init() noexcept
{
    subscribe(Core::EventType::INITIALIZED);
    subscribe(Core::EventType::CanDlcCmd);
    subscribe(Core::EventType::CanGcCmd);
    subscribe(Core::EventType::CanCiCmd);
    subscribe(Core::EventType::CanLedCmd);
    subscribe(Core::EventType::CanInclinationCmd);
    subscribe(Core::EventType::CanJackAsymCmd);
    subscribe(Core::EventType::CanJackAmplitudeCmd);
    subscribe(Core::EventType::CanVariatorCmd);
}

void CanService::dispatch(const Core::Event& event)
{
    switch(event.type())
    {
        case Core::EventType::INITIALIZED:
            LOG_DEBUG("CanService::dispatch() Initilized");
        break;

        case Core::EventType::CanDlcCmd:
            onDlcCommand(event);
        break;
        
        case Core::EventType::CanGcCmd:
            onGravityCenterCommand(event);
        break;

        case Core::EventType::CanCiCmd:
            onInertialCenterCommand(event);
        break;

        case Core::EventType::CanLedCmd:
            onLedCommand(event);
        break;

        case Core::EventType::CanInclinationCmd:
            onInclinationCommand(event);
        break;

        case Core::EventType::CanJackAsymCmd:
            onJackAsymCommand(event);
        break;

        case Core::EventType::CanJackAmplitudeCmd:
            onJackAmplitudeCommand(event);
        break;

        case Core::EventType::CanVariatorCmd:
            onVariatorCommand(event);
        break;

        default: 
        break;
    }
}

// ================================================================== PROCESSING ===================================================================

void CanService::routeCanResponse(const CanMessage& msgResponse) noexcept
{
    auto getCanValuePercentage = [&](int value, int min, int max)
    {
        int result = 0;

        // Remove static noise (basic)
        if (abs(value-min) <= CAN_DLC_STATIC_NOISE_VALUE)
        {
            return result;
        }

        const int range = max - min;

        if (range > 0)
        {
            result = ((value - min) * 100) / (max - min);
        }

        return result;
    };

    auto normalizeValue = [&](int value, std::string_view limitKey)
    {        
        int normalizedValue = CAN_SAFE_FALLBACK_VALUE;

        auto limits = m_codecs->getMsgValues(limitKey);
        if(limits)
        {
            value = std::clamp(value, limits->min, limits->max);
            normalizedValue = getCanValuePercentage(value, limits->min, limits->max);
        }
        else
        {
            LOG_ERROR("CanService::routeCanResponse() Limits not found for '"+std::string(limitKey)+"'");
        }

        return normalizedValue;
    };

    auto registerCanData = [&](auto&& storeMethod, std::string_view limitKey, int data)
    {   
        int value = data;

        if (!limitKey.empty())
        {
            value = normalizeValue(data, limitKey);
        }

        storeMethod(value);
    };

    auto frameType = m_codecs->resolveResponse(msgResponse);

    // LOG_DEBUG("CanService::routeCanResponse() Message type: " + std::string(frameType));

    int data = msgResponse.getParsedData();

    if(frameType == "dlc_left_response_x")
    {
        m_dlcLeftState.x = normalizeValue(data, "dlc_values");
        m_dlcLeftState.hasX = true;
    }
    else if (frameType == "dlc_left_response_y")
    {
        m_dlcLeftState.y = normalizeValue(data, "dlc_values");
        m_dlcLeftState.hasY = true;;
    }
    else if (frameType == "dlc_left_response_z")
    {
        m_dlcLeftState.z = normalizeValue(data, "dlc_values");
        m_dlcLeftState.hasZ = true;
        // Raw values are for test purposes since they are not completely syncronized with normalized values in case of frame lost
        Data::IDataWriteCanService::setCanRawLeftDlcValue(data);
    }
    else if (frameType == "dlc_right_response_x")
    {
        m_dlcRightState.x = normalizeValue(data, "dlc_values");
        m_dlcRightState.hasX = true;
    }
    else if (frameType == "dlc_right_response_y")
    {
        m_dlcRightState.y = normalizeValue(data, "dlc_values");
        m_dlcRightState.hasY = true;
    }
    else if (frameType == "dlc_right_response_z")
    {
        m_dlcRightState.z = normalizeValue(data, "dlc_values");
        m_dlcRightState.hasZ = true;
        // Raw values are for test purposes since they are not completely syncronized with normalized values in case of frame lost
        Data::IDataWriteCanService::setCanRawRightDlcValue(data);
    }
    else if (frameType == "weight_center_response_x")
    {
        m_gcState.x = normalizeValue(data, "gc_values");
        m_gcState.hasX = true;
    }
    else if (frameType == "weight_center_response_y")
    {
        m_gcState.y = normalizeValue(data, "gc_values");
        m_gcState.hasY = true;
    }
    else if(frameType == "jack_amplitude_response")
    {
        registerCanData(Data::IDataWriteCanService::setCanJackAmplitudeValue, "jack_amplitude_values", data);
    }
    else if (frameType == "jack_amplitude_intensity_response")
    {
        registerCanData(Data::IDataWriteCanService::setCanJackAmplitudeIntensityValue, "jack_intensity_values", data);
    }
    else if (frameType == "jack_asym_response")
    {
        registerCanData(Data::IDataWriteCanService::setCanJackAsymetryValue, "jack_asymetry_values", data);
    }
    else if (frameType == "jack_asym_intensity_response")
    {
        registerCanData(Data::IDataWriteCanService::setCanJackAsymetryIntensityValue, "jack_intensity_values", data);
    }
    else if (frameType == "inertial_inclination_response")
    {
        registerCanData(Data::IDataWriteCanService::setCanInertialInclinationValue, "plate_inclination_values", data);
    }
    else if (frameType == "inertial_azimuth_response")
    {
        registerCanData(Data::IDataWriteCanService::setCanInertialAzimuthValue, "plate_azimuth_values", data);
    }
    else if (frameType == "led_response")
    {
        registerCanData(Data::IDataWriteCanService::setCanLedValue, "led_values", data);
    }
    else if (frameType == "jack_asym_error")
    {
        registerCanData(Data::IDataWriteCanService::setCanJackAsymetryErrorValue, "", data);
    }
    else if (frameType == "jack_amplitude_error")
    {
        registerCanData(Data::IDataWriteCanService::setCanJackAmplitudeErrorValue, "", data);
    }
    else if(frameType == "security_stop")
    {
        registerCanData(Data::IDataWriteCanService::setCanSecurityStopValue, "", data);
    }
    else
    {
        LOG_ERROR("CanService::routeCanResponse() Unkown CAN frame !");
    }

    // SET TELEMETRY DATA
    if(m_dlcLeftState.completeZ())
    {
        // LOG_DEBUG("CanService::routeCanResponse() Left DLC usable data: "+std::to_string(m_dlcLeftState.z));
        Data::IDataWriteCanService::setCanLeftDlcValue(m_dlcLeftState.z);
    }

    if(m_dlcRightState.completeZ())
    {
        // LOG_DEBUG("CanService::routeCanResponse() Rigth DLC usable data: "+std::to_string(m_dlcRightState.z));
        Data::IDataWriteCanService::setCanRightDlcValue(m_dlcRightState.z);
    }

    if(m_gcState.complete())
    {
        Data::IDataWriteCanService::setCanGcValue({m_gcState.x, m_gcState.y});
    }
}

// ================================================================== COMMANDS =====================================================================

void CanService::onDlcCommand(const Core::Event& event) noexcept
{
    // LOG_DEBUG("CanService::onDlcCommand() Received DLC Event");

    if (auto* payload = std::get_if<int>(&event.payload()))
    {
        // LOG_DEBUG("Received DLC payload " + std::to_string(*payload));

        if (*payload == 1)
        {
            sendCommand("dlc_start_aquisition", {});
        }
        else if (*payload == 0)
        {
            sendCommand("dlc_stop_aquisition", {});
        }
    }
}

void CanService::onGravityCenterCommand(const Core::Event& event) noexcept
{
    // LOG_DEBUG("CanService::onGravityCenterCommand() Received GC Event");

    if (auto* payload = std::get_if<int>(&event.payload()))
    {
        // LOG_DEBUG("Received GC payload " + std::to_string(*payload));

        if (*payload == 1)
        {
            sendCommand("weight_center_start", {});
        }
        else if (*payload == 0)
        {
            sendCommand("weight_center_stop", {});
        }
    }
}

void CanService::onInertialCenterCommand(const Core::Event& ) noexcept
{
    // TODO
    LOG_DEBUG("CanService::onInertialCenterCommand() Received Inertial Center Cmd Event");
}

void CanService::onJackAsymCommand(const Core::Event& ) noexcept
{
    // TODO
    LOG_DEBUG("CanService::onJackAsymCommand() Received Jack Asym Cmd Event");
}

void CanService::onJackAmplitudeCommand(const Core::Event& ) noexcept
{
    // TODO
    LOG_DEBUG("CanService::onJackAmplitudeCommand() Received Jack Amplitude Cmd Event");
}

void CanService::onLedCommand(const Core::Event& ) noexcept
{
    // TODO
    LOG_DEBUG("CanService::onGravityCenterCommand() Received Jack Amplitude Cmd Event");
}

void CanService::onInclinationCommand(const Core::Event& ) noexcept
{
    // TODO
    LOG_DEBUG("CanService::onInclinationCommand() Received Inclination Cmd Event");
}

void CanService::onVariatorCommand(const Core::Event& ) noexcept
{
    // TODO
    LOG_DEBUG("CanService::onVariatorCommand() Received Variator Cmd Event");
}

// ================================================================== SERVICE ======================================================================

void CanService::setup()
{
    ssize_t res = m_socket->setup();

    if (res < 0)
    {
        LOG_ERROR("CanService::setup() error with the following code:");
        errnoHandle(-res);
    }

    if(!m_socket->initOk())
    {
        m_disabled = true;
        LOG_ERROR("CanService::setup() CAN socket initialization failure. Service is disabled.");
        return;
    }

    // TODO TBD
    // m_heartbeatTimer->start();
}

void CanService::run()
{
    setup();

    while(running())
    {
        canUpdate();
        
        dispatchLoop();
    }

    LOG_DEBUG("CanService::run() STOPPED !");
}

void CanService::canUpdate()
{
    if(m_disabled)
    {
        return;
    }

    // if (m_socket->wait() == Hal::CanSelectStatus::Ready)
    // {
    //     receiveMessage();
    // }

    switch (m_socket->wait())
    {
        case Hal::CanSelectStatus::Ready:
            receiveMessage();
            break;

        case Hal::CanSelectStatus::Interrupted:
            return;

        case Hal::CanSelectStatus::Timeout:
            return;

        case Hal::CanSelectStatus::Error:
            LOG_ERROR("CanSelectStatus::Error");
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            break;

        case Hal::CanSelectStatus::Disabled:
            LOG_ERROR("CanSelectStatus::Disabled");
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            break;
        default:
            break;
    }
}

// ================================================================== CAN INTERFACE ================================================================

void CanService::checkCanInterface() noexcept
{
    if(m_canDriver->isUp())
    {
        return;
    }

    LOG_WARN("CAN interface is DOWN. Restarting ...");

    turnOnCanInterface();
}

void CanService::setupCanInterface(uint32_t bitrate) noexcept
{
    int res = m_canDriver->setBitrate(bitrate);

    if (res != 0)
    {
        LOG_ERROR("CanService::setupCanInterface() Error while setting the CAN bitrate with following error: ");
        errnoHandle(res);
    }

    checkCanInterface();
}

void CanService::onHeartbeatTimer() noexcept
{
    checkCanInterface();
}

void CanService::turnOnCanInterface() noexcept
{
    if(m_canDriver->isUp())
    {
        return;
    }

    int res = m_canDriver->setUp(true);

    if (res != 0)
    {
        LOG_ERROR("CanService::turnOnCanInterface() Error while turning CAN up with following error: ");
        errnoHandle(res);
    }
}

// ================================================================== CAN OPERATIONS ===============================================================

void CanService::sendCommand(std::string_view commandName, std::initializer_list<uint8_t> data) noexcept
{
    const CanMessageTemplate* msgTemplate = m_codecs->getMsgTemplate(commandName);
    
    if (msgTemplate == nullptr)
    {
        LOG_ERROR("CanService::sendCommand() Template not found for command name " + std::string(commandName));
        return;
    }

    CanMessage msg(msgTemplate->id, m_codecs->uiSenderId(), static_cast<uint8_t>(msgTemplate->param), static_cast<uint8_t>(msgTemplate->func), data);
    sendMessage(msg);
}

void CanService::sendMessage(const CanMessage& msg) noexcept
{
    // std::ostringstream oss;
    // oss << "Send CAN message " << std::hex << std::uppercase << "0x" << msg.id() << msg.param() << msg.func();
    // LOG_DEBUG(oss.str());

    ::can_frame frame{};

    if(!msg.encode(frame))
    {
        std::ostringstream oss;
        oss << "CanService::sendMessage : Failed to encode message " << std::hex << std::uppercase << "0x" << msg.id() << msg.param() << msg.func();
        LOG_ERROR(oss.str());
        return;
    }

    LOG_DEBUG("CanService::sendMessage() Send frame:");
    printFrame(frame);

    ssize_t res = m_socket->send(frame);

    if (res < 0)
    {
        std::ostringstream oss;
        oss << "CanService::sendMessage : Failed to send message " << std::hex << std::uppercase << "0x" << msg.id() << msg.param() << msg.func() << " with the error: \n";
        LOG_ERROR(oss.str());
        errnoHandle(-res);
    }
    else
    {
        std::ostringstream oss;
        oss << "CanService::sendMessage : Sent " << res << " bytes" << std::endl;
        LOG_DEBUG(oss.str());
    }
}

void CanService::receiveMessage() noexcept
{
    ::can_frame frame{};

    ssize_t ret;

    while ( (ret = m_socket->receive(frame)) > 0 )
    {
        // Debug only
        // printFrame(frame);

        CanMessage msg{};

        if (!msg.decode(frame))
        {
            LOG_ERROR("CanService::receiveMessage() Fail to decode CAN frame");
            return;
        }

        // std::ostringstream oss;
        // oss << "Received 0x" << std::hex << std::uppercase << static_cast<int>(msg.id()) << " " 
        //     << static_cast<int>(msg.sender()) << " " << msg.param() << " " << msg.func() << " " << std::dec << msg.getParsedData();
        // LOG_DEBUG(oss.str());
        
        routeCanResponse(msg);
    }

    if (ret < 0)
    {
        LOG_ERROR("CanService::receiveMessage() error with the following code:");
        errnoHandle(-ret);
    }
}

// ================================================================== DEBUG ========================================================================

void CanService::printFrame(const ::can_frame& f)
{
    std::ostringstream oss;
    oss << "ID: 0x" << std::hex << std::uppercase << (f.can_id & CAN_SFF_MASK) << "  LEN: " << std::dec << static_cast<int>(f.len) << "  DATA: ";

    for (uint8_t i = 0; i < f.len; ++i)
    {
        oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(f.data[i]) << " ";
    }

    LOG_DEBUG(oss.str());
}

void CanService::errnoHandle(int errValue) const
{
    if (errValue < 0)
    {
        errValue = -errValue;
    }
    
    std::ostringstream oss;
    oss << "Operation failed with the error \"" << strerror(errValue) << "\" errno=" << errValue;
    LOG_ERROR(oss.str());
}

}// namespace Can