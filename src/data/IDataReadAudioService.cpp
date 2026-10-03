/**
 * @file IDataReadAudioService.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "data/IDataReadAudioService.hpp"

namespace Data
{

std::string IDataReadAudioService::getAudioInterface() noexcept
{
    return m_audioInterface.get();
}

std::string IDataReadAudioService::getAudioMixer() noexcept
{
    return m_audioMixer.get();
}

std::string IDataReadAudioService::getAudioAltMixer() noexcept
{
    return m_audioAltMixer.get();
}

size_t IDataReadAudioService::getAudioMaxAlsaBufferSize() noexcept
{
    return m_MAX_ALSA_BUFFER_SIZE.load();
}

size_t IDataReadAudioService::getAudioMaxAlsaPeriodSize() noexcept
{
    return m_MAX_ALSA_PERIOD_SIZE.load();
}

}// namespace Data