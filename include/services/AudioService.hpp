/**
 * @file AudioService.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include <alsa/asoundlib.h>

#include <atomic>
#include <string_view>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "core/AQueue.hpp"
#include "core/Event.hpp"
#include "core/ServiceBaseQueue.hpp"

/*
TODO DOCS
*/

namespace Audio
{

struct WavData;
struct WavFormatHeader;

constexpr std::size_t MAX_PLAYBACK_QUEUE = 16;

class AudioService final: public Core::ServiceBaseQueue
{
public:
    // Give a valid service Id in Core::ServiceId
    static constexpr Core::ServiceId staticId = Core::ServiceId::AudioService;

    explicit AudioService(std::shared_ptr<Core::EventManager> manager, uint8_t maxEventsPerCycle=4);

    ~AudioService() noexcept;

    AudioService(const AudioService&) = delete;
    AudioService& operator=(const AudioService&) = delete;

    // SERVICE
    void init() noexcept override;
    void setup() override;
    void dispatch(const Core::Event& event) override;

private:
    // EVENTS AND CALLBACKS
    void sendAudioVolume();
    void onAudioPlay(const Core::Event& event);
    void onAudioVolume(const Core::Event& event);
    void onAudioStop();
    void onAudioResume();
    void onAudioPause();

    // INTERNALS
    bool initDevice();
    void initDAC();
    void playbackLoop();

    // AUDIO WAV HANDLING
    WavData parseWavHeader(const uint8_t* data, std::size_t size, std::string_view filename);
    bool setHardwareParameters(const WavFormatHeader& headerFmt);
    void playFile(std::string_view filename);
    void playResource(std::string_view filename);
    void playSilence();

    // CONTROLS 
    void stop();
    void pause();
    void resume();
    void setVolume(int percentage);
    int getVolume() const;

    // Devices
    snd_pcm_t* m_pcmHandle = nullptr;
    std::string m_currentDevice;
    std::string m_currentMixer;

    // Playback control
    std::atomic<bool> m_is_playing{false};
    std::atomic<bool> m_is_paused{false};
    std::atomic<bool> m_workerRunning{false};

    // Playback variables
    Core::AQueue<std::string, MAX_PLAYBACK_QUEUE> m_audioQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCV;
    std::thread m_playbackThread;

    bool m_disabled{false};
};

}// namespace Audio