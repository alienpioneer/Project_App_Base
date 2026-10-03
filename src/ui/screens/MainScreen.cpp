/**
 * @file MainScreen.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "ui/screens/MainScreen.hpp"
#include "services/UIService.hpp"
#include "ui/components/UIComponents.hpp"
#include "ui/components/StyleManager.hpp"
#include "utils/logger.hpp"

namespace UI
{

MainScreen::MainScreen(UIService* uiservice, lv_obj_t* parent)
    :   IBaseScreen(uiservice, parent),
        m_dashboard()
{
    // Set the background color
    // lv_obj_set_style_bg_color(m_root, StyleManager::get_current_palette().bkg_color, 0);
    // lv_obj_set_style_bg_opa(m_root, LV_OPA_COVER, 0);
    // lv_obj_clear_flag(m_root, LV_OBJ_FLAG_SCROLLABLE);

    // int32_t width  = lv_obj_get_width(m_root);
    // int32_t height = lv_obj_get_height(m_root);

    // Add settings button
    // lv_obj_t* settings_btn = UIComponents::create_round_button_icon(m_root, 60, &img_gear, 60);
    // setAction(settings_btn, UIAction::ToSettings);
    // lv_obj_align(settings_btn, LV_ALIGN_TOP_LEFT, 30, 20);
    // lv_obj_add_style(settings_btn, &StyleManager::style_round_button, 0);
    // lv_obj_add_event_cb(settings_btn, UIService::lvglEventDispatcher, LV_EVENT_CLICKED, m_uiservice);


    // Shutdown hidden button
    // lv_obj_t* hitbox = UIComponents::create_invisible_button(m_root, {100, 100});
    // lv_obj_align(hitbox, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    // setAction(hitbox, UIAction::RequestShutdown);
    // lv_obj_add_event_cb(hitbox, UIService::lvglEventDispatcher, LV_EVENT_CLICKED, m_uiservice);

    m_dashboard.Initialize(m_root);

}

// ================================================================== ITEMS CREATION ==============================================================================

// ================================================================== EXTERNAL CALLBACKS ==========================================================================

// ================================================================== LOCAL CALLBACKS =============================================================================

// ================================================================== ACTION CALLBACKS ============================================================================

// ================================================================== INTERNALS ===================================================================================

}// namespace UI
