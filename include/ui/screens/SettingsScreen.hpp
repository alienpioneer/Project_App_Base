/**
 * @file SettingsScreen.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include <atomic>
#include "ui/screens/IBaseScreen.hpp"

namespace UI
{

    constexpr int SOUND_SLIDER_MAX_VALUE = 10;
    constexpr int BKLIGHT_SLIDER_MAX_VALUE = 10;

class SettingsScreen : public IBaseScreen 
{
public:

    explicit SettingsScreen(UIService* uiservice, lv_obj_t* parent=NULL);
    ~SettingsScreen() = default;

    inline ScreenID getID() const override { return m_id; }

private:
    static constexpr ScreenID m_id = ScreenID::Settings;   

    lv_obj_t* m_soundSlider = nullptr;
    int m_soundSliderCache{1};

    lv_obj_t* m_backlightSlider = nullptr;
    int m_backlightSliderCache{1};


};

}// namespace UI
