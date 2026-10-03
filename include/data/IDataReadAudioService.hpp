/**
 * @file IDataReadAudioService.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include "managers/DataManager.hpp"

namespace Data
{
    
class IDataReadAudioService: public DataManager
{

public:
    static std::string getAudioInterface() noexcept;
    static std::string getAudioMixer() noexcept;
    static std::string getAudioAltMixer() noexcept;

    static size_t getAudioMaxAlsaBufferSize() noexcept;
    static size_t getAudioMaxAlsaPeriodSize() noexcept;
};

}// namespace Data
