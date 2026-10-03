#pragma once
#include <string>
#include <unordered_map>

extern "C" {
    extern const unsigned char _binary_urgence_wav_start[];
    extern const unsigned char _binary_urgence_wav_end[];
    extern const unsigned char _binary_tic_wav_start[];
    extern const unsigned char _binary_tic_wav_end[];
    extern const unsigned char _binary_click_wav_start[];
    extern const unsigned char _binary_click_wav_end[];
    extern const unsigned char _binary_back_wav_start[];
    extern const unsigned char _binary_back_wav_end[];
    extern const unsigned char _binary_erreur_wav_start[];
    extern const unsigned char _binary_erreur_wav_end[];
    extern const unsigned char _binary_bong_wav_start[];
    extern const unsigned char _binary_bong_wav_end[];
    extern const unsigned char _binary_Bip_ok_court_wav_start[];
    extern const unsigned char _binary_Bip_ok_court_wav_end[];
    extern const unsigned char _binary_click_aigue_wav_start[];
    extern const unsigned char _binary_click_aigue_wav_end[];
    extern const unsigned char _binary_sens_wav_start[];
    extern const unsigned char _binary_sens_wav_end[];
    extern const unsigned char _binary_Bip_error_court_wav_start[];
    extern const unsigned char _binary_Bip_error_court_wav_end[];
    extern const unsigned char _binary_tac_wav_start[];
    extern const unsigned char _binary_tac_wav_end[];
    extern const unsigned char _binary_intro_wav_start[];
    extern const unsigned char _binary_intro_wav_end[];
    extern const unsigned char _binary_beep_wav_start[];
    extern const unsigned char _binary_beep_wav_end[];
}

struct AudioResource
{
    const unsigned char* data;
    size_t size;
};

inline const AudioResource* get_audio_resource(const std::string& name)
{
    static const std::unordered_map<std::string, AudioResource> registry = {
        {"urgence.wav", {_binary_urgence_wav_start, (size_t)(_binary_urgence_wav_end - (size_t)_binary_urgence_wav_start)}},
        {"tic.wav", {_binary_tic_wav_start, (size_t)(_binary_tic_wav_end - (size_t)_binary_tic_wav_start)}},
        {"click.wav", {_binary_click_wav_start, (size_t)(_binary_click_wav_end - (size_t)_binary_click_wav_start)}},
        {"back.wav", {_binary_back_wav_start, (size_t)(_binary_back_wav_end - (size_t)_binary_back_wav_start)}},
        {"erreur.wav", {_binary_erreur_wav_start, (size_t)(_binary_erreur_wav_end - (size_t)_binary_erreur_wav_start)}},
        {"bong.wav", {_binary_bong_wav_start, (size_t)(_binary_bong_wav_end - (size_t)_binary_bong_wav_start)}},
        {"Bip_ok_court.wav", {_binary_Bip_ok_court_wav_start, (size_t)(_binary_Bip_ok_court_wav_end - (size_t)_binary_Bip_ok_court_wav_start)}},
        {"click_aigue.wav", {_binary_click_aigue_wav_start, (size_t)(_binary_click_aigue_wav_end - (size_t)_binary_click_aigue_wav_start)}},
        {"sens.wav", {_binary_sens_wav_start, (size_t)(_binary_sens_wav_end - (size_t)_binary_sens_wav_start)}},
        {"Bip_error_court.wav", {_binary_Bip_error_court_wav_start, (size_t)(_binary_Bip_error_court_wav_end - (size_t)_binary_Bip_error_court_wav_start)}},
        {"tac.wav", {_binary_tac_wav_start, (size_t)(_binary_tac_wav_end - (size_t)_binary_tac_wav_start)}},
        {"intro.wav", {_binary_intro_wav_start, (size_t)(_binary_intro_wav_end - (size_t)_binary_intro_wav_start)}},
        {"beep.wav", {_binary_beep_wav_start, (size_t)(_binary_beep_wav_end - (size_t)_binary_beep_wav_start)}},
    };

    auto it = registry.find(name);
    return (it != registry.end()) ? &it->second : nullptr;
}
