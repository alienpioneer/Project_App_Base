/**
 * @file AuthPopup.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include "ui/screens/IBasePopup.hpp"

namespace UI
{

class UIService;

class AuthPopup: public IBasePopup
{
public:

    static constexpr uint32_t ANIMATION_DURATION_MS = 500;
    static constexpr float ANIMATION_AMPLITUDE = 25.0f; 
    static constexpr float ANIMATION_FREQUENCY = 4.0f;  

public:

    explicit AuthPopup(UIService* uiservice);
    ~AuthPopup() = default;

    inline ScreenID getID() const override { return m_id; }

    void onAuthFailure() noexcept;

private:
    void start_jiggle(lv_obj_t* target) noexcept;
    // Lvgl Callbacks
    static void onPasswordKeyboard(lv_event_t *e);
    static void jiggle_anim_cb(void* user_obj, int32_t value);

private:
    static constexpr ScreenID m_id = ScreenID::AuthPopup;  

    lv_obj_t* m_keyboard=nullptr;
    lv_obj_t* m_textArea=nullptr;
};

}// namespace UI