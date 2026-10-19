/**
 * @file AuthPopup.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "ui/screens/AuthPopup.hpp"

#include <cmath>

#include "services/UIService.hpp"
#include "ui/components/StyleManager.hpp"
#include "ui/components/UIWidgets.hpp"
#include "utils/logger.hpp"

namespace UI
{

AuthPopup::AuthPopup(UIService* uiservice)
    : IBasePopup{uiservice}
{
    // Add password keyboard
    m_popup = UIWidgets::create_pswd_popup(lv_layer_top(), {900,400}, &lv_font_montserrat_24);
    lv_obj_align(m_popup, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_hidden(m_popup, true);
    lv_obj_add_style(m_popup, &StyleManager::style_pswd_overlay, 0);

    m_keyboard = lv_obj_get_child_by_name(m_popup, "keyboard");
    m_textArea = lv_obj_get_child_by_name(m_popup, "text_area");

    if(!m_textArea)
    {
        LOG_ERROR("AuthPopup::AuthPopup() Lvgl - No child named text_area");
    }

    if(!m_keyboard)
    {
        LOG_ERROR("AuthPopup::AuthPopup() Lvgl - No child named keyboard");
        return;
    }

    lv_obj_add_style(m_keyboard, &StyleManager::style_pswd_keyboard, 0);
    lv_obj_add_event_cb(m_keyboard, &AuthPopup::onPasswordKeyboard, LV_EVENT_ALL, this);
}

void AuthPopup::onPasswordKeyboard(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    auto* cur_instance = static_cast<AuthPopup*>(lv_event_get_user_data(e));

    if(!cur_instance)
    {
        return;
    }

    if(code == LV_EVENT_READY) 
    {       
        // LOG_DEBUG("AuthPopup::onPasswordKeyboard() LV_EVENT_READY");

        if (cur_instance->m_textArea)
        {
            std::string password = lv_textarea_get_text(cur_instance->m_textArea);
            lv_textarea_set_text(cur_instance->m_textArea, "");

            if(cur_instance->m_uiservice)
            {
                cur_instance->m_uiservice->sendPasswordResponse(password);
            }
            else
            {
                LOG_ERROR("AuthPopup::onPasswordKeyboard() No uiservice availabe !"); 
            }
        }
        else
        {
            LOG_ERROR("AuthPopup::onPasswordKeyboard() Lvgl - Invalid member m_textArea !");
        }
    }

    if(code == LV_EVENT_CANCEL)
    {
        // LOG_DEBUG("AuthPopup::onPasswordKeyboard() LV_EVENT_CANCEL");
        cur_instance->m_uiservice->cancelAccessRequest();
    }
}

void AuthPopup::onAuthFailure() noexcept
{
    // LOG_DEBUG("AuthPopup::onAuthFailure()");

    start_jiggle(m_keyboard);
    start_jiggle(m_textArea);

    m_uiservice->resetAccessRequest();
}

void AuthPopup::jiggle_anim_cb(void* user_obj, int32_t value) 
{
    auto* obj = static_cast<lv_obj_t*>(user_obj);
    
    float timeLeft = static_cast<float>(value) / 1000.0f;
    
    float amplitude = ANIMATION_AMPLITUDE; 
    
    if (timeLeft > 0.75f) 
    {
        float decay = 1.0f - ((timeLeft - 0.75f) / 0.25f);
        amplitude *= decay;
    }
    
    float offset = amplitude * std::sin(timeLeft * ANIMATION_FREQUENCY * 2.0f * M_PI);
    
    lv_obj_set_style_translate_x(obj, static_cast<int32_t>(offset), LV_PART_MAIN);
}

void AuthPopup::start_jiggle(lv_obj_t* target) noexcept
{
    lv_anim_t animation;
    lv_anim_init(&animation);
    lv_anim_set_var(&animation, target);
    lv_anim_set_values(&animation, 0, 1000);
    lv_anim_set_time(&animation, ANIMATION_DURATION_MS);
    lv_anim_set_exec_cb(&animation, AuthPopup::jiggle_anim_cb);
    lv_anim_set_path_cb(&animation, lv_anim_path_linear);
    
    lv_anim_start(&animation);
}

} // namespace UI