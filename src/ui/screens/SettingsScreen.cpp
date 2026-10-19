/**
 * @file SettingsScreen.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "ui/screens/SettingsScreen.hpp"

extern "C"
{
#include "ui/resources.h"
}

#include <algorithm>
#include <string>

#include "services/UIService.hpp"
#include "ui/components/StyleManager.hpp"
#include "ui/components/UIComponents.hpp"
#include "ui/components/UIWidgets.hpp"
#include "ui/components/UIUtils.hpp"
#include "utils/logger.hpp"

namespace UI
{

SettingsScreen::SettingsScreen(UIService* uiservice, lv_obj_t* parent)
    :   IBaseScreen(uiservice, parent)
{
    // Set the background color
    lv_obj_set_style_bg_color(m_root, StyleManager::get_current_palette().bkg_color, 0);
    lv_obj_set_style_bg_opa(m_root, LV_OPA_COVER, 0);

    lv_obj_set_scrollable(m_root, false);

    // int32_t width  = lv_obj_get_width(m_root);
    // int32_t height = lv_obj_get_height(m_root);


}

// ================================================================== ITEMS CREATION ==============================================================================


// ================================================================== LOCAL CALLBACKS =============================================================================


// ================================================================== EXTERNAL CALLBACKS ==========================================================================


// ================================================================== INTERNALS ===================================================================================


}// namespace UI
