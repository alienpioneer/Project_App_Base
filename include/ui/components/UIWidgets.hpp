/**
 * @file UIWidgets.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

extern "C"
{
#include <lvgl/lvgl.h>
}

#include <string>
#include <array>

#include "ui/UIStates.hpp"
#include "ui/components/UIComponents.hpp"

namespace UIWidgets 
{


//========================================================= ROLLERS =================================================================

/**
 * @brief Create a roller with label object
 * 
 * @param parent 
 * @param size 
 * @param spacing 
 * @param options { options, visible_items, selection, width_percentage, font, selection_font, min, max, step }
 * @param label 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_roller_with_label(lv_obj_t* parent, const UIComponents::Size size, const int32_t spacing, const UIComponents::RollerOptions& options, const char* labelText);

//========================================================= SPINBOXES ===============================================================

/**
 * @brief Create a horizontal spinbox object
 * 
 * @param parent 
 * @param size 
 * @param spinboxValues {defaultValue, increment, minValue, maxValue, label, labelSizePercentage}
 * @param font 
 * @return lv_obj_t* 
 * @note If labelSizePercentage is zero, the label size will automatically adapt to the text
 */
[[nodiscard]] lv_obj_t* create_horizontal_spinbox(lv_obj_t* parent, const UIComponents::Size size, const UIComponents::SpinboxValues& spinboxValues, const lv_font_t* font);


/**
 * @brief Create a vertical spinbox group object
 * 
 * @param parent 
 * @param size 
 * @param spacing 
 * @param spinboxValueMin {defaultValue, increment, minValue, maxValue, label, labelSizePercentage}
 * @param spinboxValueMax {defaultValue, increment, minValue, maxValue, label, labelSizePercentage}
 * @param font 
 * @return lv_obj_t* 
 * @note If labelSizePercentage is zero, the label size will automatically adapt to the text
 */
[[nodiscard]] lv_obj_t* create_vertical_spinbox_group(lv_obj_t* parent, const UIComponents::Size size, const int32_t spacing, const UIComponents::SpinboxValues& spinboxValueMin, const UIComponents::SpinboxValues& spinboxValueMax, const lv_font_t* font);

//========================================================= BARS ====================================================================

/**
 * @brief Create a horizontal dlc bar object
 * 
 * @param parent 
 * @param size 
 * @param indicatorWidth 
 * @param guideWidth 
 * @param showIndicator 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_horiz_dlc_bar(lv_obj_t* parent, const UIComponents::Size size, const int32_t indicatorWidth, const int32_t guideWidth, bool showIndicator=true);

/**
 * @brief Create a vertical dlc bar object
 * 
 * @param parent 
 * @param size 
 * @param indicatorWidth 
 * @param guideWidth 
 * @param showIndicator 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_vertical_dlc_bar(lv_obj_t* parent, const UIComponents::Size size, const int32_t indicatorHeight, const int32_t guideHeight, bool showIndicator=true);

/**
 * @brief Create a vertical dlc bar object that is filled from the bottom up
 * 
 * @param parent 
 * @param size 
 * @param guideHeight 
 * @param showIndicator 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_vertical_fill_dlc_bar(lv_obj_t* parent, const UIComponents::Size size, const int32_t guideHeight, bool showIndicator=true);

/**
 * @brief Create a horiz levels bar object
 * 
 * @param parent 
 * @param size 
 * @param leftIcon 
 * @param rightIcon 
 * @param levelNumber 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_horiz_levels_bar(lv_obj_t* parent, const UIComponents::Size size, const lv_image_dsc_t* leftIcon, const lv_image_dsc_t* rightIcon, uint32_t levelNumber);

//========================================================= ANIMATED WIDGETS ========================================================

/**
 * @brief Create a animated dot object
 * 
 * @param parent 
 * @param range 
 * @param color 
 * @param duration 
 * @param oscillate 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t * create_animated_dot(lv_obj_t * parent, UIComponents::Range range, lv_color_t color, uint32_t duration, bool oscillate=false);

//========================================================= KEYBOARDS ===============================================================

/**
 * @brief Create a pswd keyboard object
 * 
 * @param parent 
 * @param size 
 * @param font 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_pswd_popup(lv_obj_t* parent, const UIComponents::Size size, const lv_font_t* font);

//========================================================= POPUPS ===================================================================

/**
 * @brief Create a speed popup object
 * 
 * @param parent 
 * @param size 
 * @param options If the max is not zero, a range will be calculated depending on min/max/step
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_roller_popup(lv_obj_t* parent, const UIComponents::Size size, const UIComponents::RollerOptions& options);

}