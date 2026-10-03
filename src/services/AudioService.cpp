/**
 * @file AudioService.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include <services/AudioService.hpp>

#include <fstream>
#include <cstdint>

#include <chrono>
#include <iostream>
#include <sstream>

#include "core/AVector.hpp"
#include "EventTypes.hpp"
#include "audiores.hpp"
#include "utils/logger.hpp"
#include "data/IDataReadAudioService.hpp"

namespace Audio
{

constexpr std::size_t WAV_HEADER_SCAN_SIZE = 8192;
constexpr std::size_t MAX_BUFFER_FRAMES   = 8192;
constexpr std::size_t MAX_BYTES_PER_FRAME = 8;

struct WavRiffHeader 
{
    char riff[4];
    uint32_t fileSize;
    char wave[4];
};

struct WavChunkHeader 
{
    char id[4];
    uint32_t size;
};

struct WavFormatHeader
{
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t bytesPerFrame;
    uint16_t bitsPerSample;
    // possible extension follows, size told by ChunkHeader::size
};

struct WavData 
{
    WavFormatHeader fmt;
    // Bytes from start of file/resource to PCM data
    uint32_t pcmDataOffset;
    // Size of the PCM data chunk
    uint32_t pcmDataSize;
    bool isValid;
};

AudioService::AudioService(std::shared_ptr<Core::EventManager> manager, uint8_t maxEventsPerCycle)
    :   ServiceBaseQueue(staticId, std::move(manager), maxEventsPerCycle),
        m_currentDevice{Data::IDataReadAudioService::getAudioInterface()},
#ifdef USE_DRM
        m_currentMixer{Data::IDataReadAudioService::getAudioMixer()},
#else
        m_currentMixer{Data::IDataReadAudioService::getAudioAltMixer()},
#endif
        m_workerRunning{false}
{
}

AudioService::~AudioService() noexcept
{
    m_workerRunning = false;
    m_queueCV.notify_all();

    if(m_playbackThread.joinable())
    {
        m_playbackThread.join();
    }

    if (m_pcmHandle)
    {
        snd_pcm_drop(m_pcmHandle);
        snd_pcm_close(m_pcmHandle);
    }
}

void AudioService::init() noexcept
{
    subscribe(Core::EventType::INITIALIZED);
    subscribe(Core::EventType::AudioPlayCmd);
    subscribe(Core::EventType::AudioPauseCmd);
    subscribe(Core::EventType::AudioResumeCmd);
    subscribe(Core::EventType::AudioStopCmd);
    subscribe(Core::EventType::AudioVolumeCmd);
}

void AudioService::setup()
{
    bool result = initDevice();

    if(!result)
    {
        m_disabled = true;
        return;
    }

    initDAC();

    m_workerRunning = true;
    m_playbackThread = std::thread(&AudioService::playbackLoop, this);
}

bool AudioService::initDevice() 
{
    if(m_currentDevice.empty())
    {
        LOG_ERROR("AudioService::initDevice() Failed to initilize the audio device. Service disabled.");
        return false;
    }

    return snd_pcm_open(&m_pcmHandle, m_currentDevice.c_str(), SND_PCM_STREAM_PLAYBACK, 0) == 0;
}

void AudioService::dispatch(const Core::Event& event)
{
    switch(event.type())
    {
        case Core::EventType::INITIALIZED:
            LOG_DEBUG("AudioService::dispatch() Initialized");
        break;

        case Core::EventType::AudioPlayCmd:
            onAudioPlay(event);
        break;

        case Core::EventType::AudioStopCmd:
            onAudioStop();
        break;

        case Core::EventType::AudioPauseCmd:
            onAudioPause();
        break;

        case Core::EventType::AudioResumeCmd:
            onAudioResume();
        break;

        case Core::EventType::AudioVolumeCmd:
            onAudioVolume(event);
        break;
        
        default: 
            break;
    }
}

// ============================================== EVENTS ===================================================================

void AudioService::onAudioPlay(const Core::Event& event)
{
    LOG_TRACE("Received Play Event");

    if(m_disabled)
    {
        LOG_ERROR("AudioService::onAudioPlay() Service disabled.");
        return ;
    }

    stop();

    if (auto* payload = std::get_if<std::string>(&event.payload()))
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);

        LOG_TRACE("Play audio payload " + std::string(*payload));

        m_audioQueue.push(std::string(*payload));
        m_queueCV.notify_one();
    }
}

void AudioService::onAudioVolume(const Core::Event& event)
{
    LOG_TRACE("Received Volume Control Event");

    if(m_disabled)
    {
        LOG_ERROR("AudioService::onAudioVolume() Service disabled.");
        return ;
    }

    if (auto* payload = std::get_if<int>(&event.payload()))
    {
        int volume = *payload;

        if(volume == -1)
        {
            sendAudioVolume();    
        }
        else
        {
            LOG_TRACE("Set volume to " + std::to_string(*payload));

            if( (0 <= volume) && (volume <= 100) )
            {
                setVolume(*payload);
            }
        }
    }
}

void AudioService::onAudioStop()
{
    LOG_TRACE("Received Stop Event");
    stop();
}

void AudioService::onAudioResume()
{
    LOG_TRACE("Received Resume Event");
    resume();
}

void AudioService::onAudioPause()
{
    LOG_TRACE("Received Pause Event");
    pause();
}

void AudioService::sendAudioVolume()
{
    LOG_TRACE("Received volume request");

    int volume = getVolume();
    Core::Event audioEvent(Core::EventType::AudioVolResponse, id());
    audioEvent.setPayload(volume);
    sendEvent(audioEvent);
}

// ============================================== CONTROLS =================================================================

int AudioService::getVolume() const
{
    if(m_disabled)
    {
        LOG_ERROR("AudioService::getVolume() Service disabled.");
        return 0;
    }

    long min, max, volume;

    snd_mixer_t* handle = nullptr;
    snd_mixer_selem_id_t* sid = nullptr;
    const char* card = m_currentDevice.c_str();
    const char* selemName = m_currentMixer.c_str();

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, nullptr, nullptr);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selemName);

    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    if (!elem) 
    {
        snd_mixer_close(handle);
        return 0;
    }

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    
    // Using Front Left as the representative channel
    snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &volume);

    snd_mixer_close(handle);

    if (max == min) 
    {
        return 0;
    }
    
    // Returns 0-100 percentage
    return static_cast<int>(((volume - min) * 100.0) / (max - min));
}

void AudioService::setVolume(int percentage)
{
    if(m_disabled)
    {
        LOG_ERROR("AudioService::setVolume() Service disabled.");
    }

    long min, max, volume;
    snd_mixer_t* handle = nullptr;
    snd_mixer_selem_id_t* sid = nullptr;
    const char* card = m_currentDevice.c_str();
    const char* selemName = m_currentMixer.c_str();

    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, nullptr, nullptr);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selemName);

    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    if (!elem) 
    {
        snd_mixer_close(handle);
        return;
    }

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    
    volume = min + (percentage * (max - min) / 100);
    
    snd_mixer_selem_set_playback_volume_all(elem, volume);

    snd_mixer_close(handle);
}

void AudioService::stop() 
{
    m_is_playing = false;
}

void AudioService::pause() 
{
    m_is_paused = true;
}

void AudioService::resume() 
{
    m_is_paused = false;
}

// ============================================== INTERNALS ==============================================================

void AudioService::playbackLoop()
{
    while(m_workerRunning)
    {
        std::string currentAudioRes;

        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_queueCV.wait(lock, [this]{ 
                return (!m_audioQueue.empty() || !m_workerRunning);
            });

            if(!m_workerRunning && m_audioQueue.empty())
            {
                break;
            }

            currentAudioRes = std::move(m_audioQueue.get());
            m_audioQueue.pop();
        }

        m_is_playing = true;

        playResource(currentAudioRes);
    }
}

WavData AudioService::parseWavHeader(const uint8_t* data, std::size_t size, std::string_view filename)
{
    WavData result{};
    result.isValid = false;

    if(!data)
    {
        LOG_ERROR("AudioService::parseWavHeader() Invalid data for " + std::string(filename));
        return result;
    }

    const WavRiffHeader* riffHeader = reinterpret_cast<const WavRiffHeader*>(data);

    if (std::string_view(riffHeader->riff, 4) != "RIFF" ||
        std::string_view(riffHeader->wave, 4) != "WAVE")
    {
        LOG_ERROR("AudioService::parseWavHeader() Invalid Wav File " + std::string(filename));
        return result;
    }

    const uint8_t* iterator = data + sizeof(WavRiffHeader);
    const uint8_t* end = data + size;
    bool fmtFound = false;

    while (iterator + sizeof(WavChunkHeader) <= end) 
    {
        auto chunk = reinterpret_cast<const WavChunkHeader*>(iterator);
        std::string_view chunkId(chunk->id, 4);
        iterator += sizeof(WavChunkHeader);

        // There is a trailing space at the end !!!
        if (chunkId == "fmt ")
        {
            fmtFound = true;

            if (chunk->size >= sizeof(WavFormatHeader)) 
            {
                result.fmt = *reinterpret_cast<const WavFormatHeader*>(iterator);
            }
            else
            {
                LOG_ERROR("AudioService::parseWavHeader() Malformed WAV fmt header");
                return result;
            }
        } 
        else if (chunkId == "data") 
        {
            result.pcmDataOffset = iterator-data;
            result.pcmDataSize = chunk->size;
        }

        // Prevents iterator from jumping past 'end' memory boundary
        if (chunk->size > static_cast<std::size_t>(end - iterator)) 
        {
            break;
        }

        // Handle padding byte (WAV chunks are word-aligned - 2 bytes)
        uint32_t nextChunkOffset = chunk->size + (chunk->size & 1);

        // ensure the iterator increment for padding is strictly within bounds
        if (nextChunkOffset > static_cast<std::size_t>(end - iterator)) 
        {
            break; 
        }
        iterator += nextChunkOffset;
    }

    if(!fmtFound)
    {
        LOG_ERROR("AudioService::parseWavHeader() Fmt header not found !");
        return result;
    }

    if (result.pcmDataSize == 0)
    {
        LOG_ERROR("AudioService::parseWavHeader() Invalid WavData size !");
        return result;
    }

    if (result.fmt.audioFormat != 1)  // 1 = PCM, 3 = IEEE float, etc.
    {
        LOG_ERROR("AudioService::parseWavHeader() Unsupported audio fmt: " + std::to_string(result.fmt.audioFormat));
        return result;
    }

    result.isValid = true;
    return result;
}

bool AudioService::setHardwareParameters(const WavFormatHeader& headerFmt)
{
    // Initilize and allocate memory
    snd_pcm_hw_params_t* params_raw = nullptr;
    // DO NOT USE snd_pcm_hw_params_alloca(), is allocating on stack => segfault
    snd_pcm_hw_params_malloc(&params_raw);
    // Automatically release with custom deleter "snd_pcm_hw_params_free" for snd_pcm_hw_params_malloc() !
    std::unique_ptr<snd_pcm_hw_params_t, decltype(&snd_pcm_hw_params_free)> params(params_raw, snd_pcm_hw_params_free);

    snd_pcm_hw_params_any(m_pcmHandle, params.get());
    snd_pcm_hw_params_set_access(m_pcmHandle, params.get(), SND_PCM_ACCESS_RW_INTERLEAVED);

    snd_pcm_format_t format;
    switch (headerFmt.bitsPerSample) 
    {
        case 8:  
            format = SND_PCM_FORMAT_U8;     
            break;
        case 16:
            format = SND_PCM_FORMAT_S16_LE; 
            break;
        case 24:
            format = SND_PCM_FORMAT_S24_LE; 
            break;
        case 32:
            format = SND_PCM_FORMAT_S32_LE; 
            break;
        default:
            LOG_ERROR("AudioService::setHardwareParameters() Unsupported Format "+ std::to_string(headerFmt.bitsPerSample));
            return false;
    }

    snd_pcm_hw_params_set_format(m_pcmHandle, params.get(), format);
    snd_pcm_hw_params_set_channels(m_pcmHandle, params.get(), headerFmt.numChannels);
    auto sampleRate = headerFmt.sampleRate;
    snd_pcm_hw_params_set_rate_near(m_pcmHandle, params.get(), &sampleRate, nullptr);

    if (sampleRate != headerFmt.sampleRate) 
    {
        LOG_WARN("AudioService::setHardwareParameters() Warning: Sample rate mismatch. Requested " + std::to_string(headerFmt.sampleRate) + " Actual: " + std::to_string(sampleRate));
    }

    size_t MAX_ALSA_BUFFER_SIZE = Data::IDataReadAudioService::getAudioMaxAlsaBufferSize();
    size_t MAX_ALSA_PERIOD_SIZE = Data::IDataReadAudioService::getAudioMaxAlsaPeriodSize();

    // Important for pause/resume !
    snd_pcm_uframes_t bufferSize = static_cast<snd_pcm_uframes_t>(MAX_ALSA_BUFFER_SIZE); 
    snd_pcm_uframes_t periodSize = static_cast<snd_pcm_uframes_t>(MAX_ALSA_PERIOD_SIZE);  

    // The total size of the hardware's internal ring buffer
    snd_pcm_hw_params_set_buffer_size_near(m_pcmHandle, params.get(), &bufferSize);
    // The number of frames processed between hardware interrupts
    snd_pcm_hw_params_set_period_size_near(m_pcmHandle, params.get(), &periodSize, nullptr);

    if (snd_pcm_hw_params(m_pcmHandle, params.get()) < 0) 
    {
        LOG_ERROR("AudioService::setHardwareParameters() Error: Hardware does not support the exact file format natively.");
        return false;
    }

    // Stabilize DAC ?
    // playSilence();

    return true;
}

void AudioService::playResource(std::string_view filename)
{
    const AudioResource* resource = get_audio_resource(std::string(filename));
    
    // =========================================== VALIDATE RESOURCE ===============================
    if (!resource) 
    {
        LOG_ERROR("AudioService::playResource() Invalid audio resource "+std::string(filename));
        return;
    } 

    if (resource->size < sizeof(WavRiffHeader))
    {
        LOG_ERROR("AudioService::playResource() Invalid audio resource size "+std::string(filename));
        return;
    } 

    WavData wavData = parseWavHeader(resource->data, resource->size, filename);

    if(!wavData.isValid)
    {
        LOG_ERROR("AudioService::playResource() Invalid WAV PCM data");
        return;
    }

    const uint8_t* pcmData = resource->data+wavData.pcmDataOffset;

    if (!pcmData || wavData.pcmDataSize == 0)
    {
        LOG_ERROR("AudioService::playResource() Invalid WavData offset");
        return;
    }

    const uint16_t expectedBytesPerFrame = wavData.fmt.numChannels * (wavData.fmt.bitsPerSample / 8);
    const uint16_t bytesPerFrame = (wavData.fmt.bytesPerFrame > 0) ? wavData.fmt.bytesPerFrame : expectedBytesPerFrame;

    if (bytesPerFrame > MAX_BYTES_PER_FRAME)
    {
        LOG_ERROR("AudioService::playResource() Unsupported Format");
        return;
    }

    if(!setHardwareParameters(wavData.fmt))
    {
        LOG_ERROR("AudioService::playResource() Unable to set hardware parameters for audio format " + std::to_string(wavData.fmt.audioFormat));
        return;
    }

    const int pcmChunkSize = MAX_BUFFER_FRAMES * bytesPerFrame;
    std::size_t offset = 0;

    while ( (offset < wavData.pcmDataSize) && m_is_playing.load()) 
    {
        if (m_is_paused.load()) 
        {
            LOG_TRACE("AudioService::playResource() Pause inside the loop.");

            int err = snd_pcm_pause(m_pcmHandle, 1);
            
            if (err == -ENOSYS) 
            {
                snd_pcm_drop(m_pcmHandle);
            }

            while (m_is_paused.load() && m_is_playing.load()) 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            if (err == -ENOSYS)
            {
                snd_pcm_prepare(m_pcmHandle);
            }
            else
            {
                snd_pcm_pause(m_pcmHandle, 0);
            }
        }

        std::size_t bytesToWrite = std::min(static_cast<std::size_t>(pcmChunkSize), wavData.pcmDataSize - offset);

        snd_pcm_sframes_t frames = bytesToWrite / bytesPerFrame;

        snd_pcm_sframes_t result = snd_pcm_writei(m_pcmHandle, pcmData + offset, frames);
        
        if (result == -EPIPE)
        {
            LOG_ERROR("AudioService::playResource() EPIPE error occured.");
            // snd_pcm_prepare(m_pcmHandle);
            snd_pcm_recover(m_pcmHandle, result, 0);
            continue;
        } 
        else if (result < 0) 
        {
            break;
        }
        else 
        {
            offset += (result * bytesPerFrame);
        }
    }

    if (m_is_playing.load()) 
    {
        snd_pcm_drain(m_pcmHandle);
    }
    else
    {
        snd_pcm_drop(m_pcmHandle);
    }

    snd_pcm_prepare(m_pcmHandle);

    m_is_playing = false;
}

void AudioService::playFile(std::string_view filename) 
{
    snd_pcm_drop(m_pcmHandle);

    std::ifstream file(filename.data(), std::ios::binary);

    if (!file)
    {
        LOG_ERROR("AudioService::playFile() Can't open file " +std::string(filename));
        return;
    } 

    std::array<uint8_t, WAV_HEADER_SCAN_SIZE> headerBuffer;
    file.read(reinterpret_cast<char*>(headerBuffer.data()), headerBuffer.size());

    auto wavData = parseWavHeader(headerBuffer.data(), file.gcount(), filename);

    if(!wavData.isValid)
    {
        LOG_ERROR("AudioService::playFile() Invalid WAV PCM data");
        return;
    }

    if(!setHardwareParameters(wavData.fmt))
    {
        LOG_ERROR("AudioService::playFile() Unable to set hardware parameters for audio format " + std::to_string(wavData.fmt.audioFormat));
        return;
    };

    const int bufferSize = MAX_BUFFER_FRAMES * wavData.fmt.bytesPerFrame;
    std::array<char, MAX_BUFFER_FRAMES * MAX_BYTES_PER_FRAME> buffer;

    // Seek to the actual PCM data start in the file
    file.seekg(wavData.pcmDataOffset, std::ios::beg);

    uint32_t totalBytesRead = 0;

    while ( file && totalBytesRead < wavData.pcmDataSize && m_is_playing.load()) 
    {
        if (m_is_paused.load()) 
        {
            LOG_TRACE("AudioService::playFile() Pause inside the loop.");

            int err = snd_pcm_pause(m_pcmHandle, 1);
            
            if (err == -ENOSYS) 
            {
                snd_pcm_drop(m_pcmHandle);
            }

            while (m_is_paused.load() && m_is_playing.load()) 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            if (err == -ENOSYS)
            {
                snd_pcm_prepare(m_pcmHandle);
            }
            else
            {
                snd_pcm_pause(m_pcmHandle, 0);
            }
        }

        const uint32_t remaining = wavData.pcmDataSize - totalBytesRead;
        const uint32_t toRead = std::min(static_cast<uint32_t>(bufferSize), remaining);
        
        file.read(buffer.data(), toRead);
        std::streamsize bytesRead = file.gcount();

        if (bytesRead <= 0) break;

        snd_pcm_sframes_t frames = bytesRead / wavData.fmt.bytesPerFrame;
        snd_pcm_sframes_t result = snd_pcm_writei(m_pcmHandle, buffer.data(), frames);
        
        
        if (result == -EPIPE)
        {
            LOG_ERROR("AudioService::playFile() EPIPE error occured.");
            // snd_pcm_prepare(m_pcmHandle);
            snd_pcm_recover(m_pcmHandle, result, 0);
            continue;
        } 
        else if (result < 0) 
        {
            break;
        }

        totalBytesRead += (result * wavData.fmt.bytesPerFrame);
    }

    if (m_is_playing.load()) 
    {
        snd_pcm_drain(m_pcmHandle);
    }
    else
    {
        snd_pcm_drop(m_pcmHandle);
    }

    snd_pcm_prepare(m_pcmHandle);

    m_is_playing = false;

}

void AudioService::playSilence()
{
    if (!m_pcmHandle)
    {
        LOG_ERROR("AudioService::playSilence() PCM handle invalid or uninitialized!");
        return;
    }

    // constexpr int framesNb = 2048;
    constexpr int framesNb = 512;

    // Use the maximum possible bytes per frame to ensure the buffer is always safe
    std::array<uint8_t, framesNb * MAX_BYTES_PER_FRAME> silence{};
    silence.fill(0);

    snd_pcm_writei(m_pcmHandle, silence.data(), framesNb);
    
    // Drain ensures the samples reach the DAC before we continue
    snd_pcm_drain(m_pcmHandle);
    
    // Return to PREPARED state so the first real sound is ready to go
    snd_pcm_prepare(m_pcmHandle);
}

void AudioService::initDAC()
{
    if(m_disabled)
    {
        LOG_ERROR("AudioService::initDAC() Service disabled.");
        return;
    }
    
    int currentVolume = getVolume();
    setVolume(0);

    WavFormatHeader dummyHeader{1, 2, 44100, 176400, 4, 16};
    
    if(setHardwareParameters(dummyHeader))
    {
        playSilence();
        setVolume(currentVolume);
    }
    else
    {
        LOG_ERROR("AudioService::playSilence() Failed to initialize DAC parameters !");
    }
}

}// namespace Audio