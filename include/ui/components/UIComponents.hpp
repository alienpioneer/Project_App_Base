/**
 * @file UIComponents.hpp
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


namespace UIComponents 
{
    
struct Size
{ 
    int32_t width=0;
    int32_t height=0;
};

struct Range 
{ 
    int32_t min=0;
    int32_t max=100; 
};

struct SlideButtonItems
{
    const char* btnLabel = nullptr;
    std::array<const char*, 10> labelList;
    lv_style_t* item_style = nullptr;
    size_t labelCount = 0;
    int32_t spacing = 0;
};

struct RollerOptions
{
    const char* options = nullptr;
    uint32_t visible_items = 0;
    uint32_t selection = 0;
    int32_t width_percentage = 80;
    const lv_font_t* font = nullptr; 
    const lv_font_t* selection_font = nullptr; 
    int32_t min = 0;
    int32_t max = 0;
    int32_t step = 0;
};

struct SpinboxValues
{
    int32_t defaultValue=0;
    int32_t increment=5;
    int32_t minValue=0;
    int32_t maxValue=100;
    const char* label = nullptr;
    int32_t labelSizePercentage=0;
};

struct ArcOptions
{
    int32_t range = 20;
    int32_t arcWidth=0;
    int32_t angleWidth=20;
    uint32_t changeRate=250;
};

struct TwoButtonOptions
{
    const char* label1 = nullptr;
    const char* label2 = nullptr;
    lv_image_dsc_t* iconSrc1 = nullptr;
    lv_image_dsc_t* iconSrc2 = nullptr;
    uint32_t iconScale1 = 60;
    uint32_t iconScale2 = 60;
};

struct ThreeButtonOptions
{
    const char* label1 = nullptr;
    const char* label2 = nullptr;
    const char* label3 = nullptr;
    const lv_image_dsc_t* iconSrc1 = nullptr;
    const lv_image_dsc_t* iconSrc2 = nullptr;
    const lv_image_dsc_t* iconSrc3 = nullptr;
    uint32_t iconScale1 = 60;
    uint32_t iconScale2 = 60;
    uint32_t iconScale3 = 60;
};

//========================================================= BUTTONS =================================================================

/**
 * @brief Create a round button icon object
 * 
 * @param parent 
 * @param size 
 * @param icon_src 
 * @param icon_percent from 1 to 100
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_round_button_icon(lv_obj_t* parent, const int32_t size, const lv_image_dsc_t* icon_src, uint32_t icon_percent=80);

/**
 * @brief Create a square button icon object
 * 
 * @param parent 
 * @param size 
 * @param icon_src 
 * @param icon_scale from 1 to 100
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_square_button_icon(lv_obj_t* parent, const Size size, const lv_image_dsc_t* icon_src, uint32_t icon_scale);

/**
 * @brief Create a round button object
 * 
 * @param parent 
 * @param size 
 * @param label 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_round_button(lv_obj_t* parent, const int32_t size, const char* label);

/**
 * @brief Create a square button object
 * 
 * @param parent 
 * @param size 
 * @param label 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_square_button(lv_obj_t* parent, const Size size, const char* label);

/**
 * @brief Create a invisible button object
 * 
 * @param parent 
 * @param size 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_invisible_button(lv_obj_t* parent, const Size size);

/**
 * @brief Create 2 buttons in a horizontal group that have exclusive selection
 * 
 * @param parent 
 * @param size 
 * @param label1 
 * @param label2 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_horiz_two_exclusive_btn_group(lv_obj_t* parent, const Size size, const TwoButtonOptions& options);

/**
 * @brief Create 2 buttons in a vertical group that have exclusive selection
 * 
 * @param parent 
 * @param size 
 * @param label1 
 * @param label2 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_vert_two_exclusive_btn_group(lv_obj_t* parent, const Size size, const TwoButtonOptions& options);

/**
 * @brief Create a three horiz exclusive btn group object
 * 
 * @param parent 
 * @param size 
 * @param options 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_three_horiz_exclusive_btn_group(lv_obj_t* parent, const Size size, const ThreeButtonOptions& options);

//========================================================= SLIDERS =================================================================

/**
 * @brief Create a slider with icon object
 * 
 * @param parent 
 * @param size 
 * @param spacing 
 * @param icon 
 * @param range 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_slider_with_icon(lv_obj_t* parent, const Size size, const int32_t spacing, const lv_image_dsc_t& icon_src, const Range range);

//========================================================= INDICATORS ==============================================================

/**
 * @brief Create a swipe dots indicator object
 * 
 * @param parent 
 * @param tileview 
 * @param height 
 * @param number 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_swipe_dots_indicator(lv_obj_t* parent, lv_obj_t* tileview, const int32_t height, const uint8_t number);
    
/**
 * @brief Create a dot object for debug
 * 
 * @param parent 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_red_dot(lv_obj_t* parent, const int32_t size);

/**
 * @brief Create a level indicator object
 * 
 * @param parent 
 * @param size 
 * @param levelNumber 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_level_indicator(lv_obj_t* parent, const Size size, uint32_t levelNumber);

//========================================================= LABELS ==================================================================

/**
 * @brief Create a label case object
 * 
 * @param parent 
 * @param size 
 * @param font 
 * @param labelText 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_label_container(lv_obj_t* parent, const Size size,const lv_font_t* font, const char* labelText);

//========================================================= CONTAINERS ==============================================================

/**
 * @brief Create a container object
 * 
 * @param parent 
 * @param size 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_container(lv_obj_t* parent, const Size size);

/**
 * @brief Create a base container object
 * 
 * @param parent 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_base_container(lv_obj_t* parent);

/**
 * @brief Create a swipe container object
 * 
 * @param parent 
 * @param size 
 * @param spacing 
 * @param items 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_swipe_container(lv_obj_t* parent, const Size size, const int32_t spacing, const SlideButtonItems& items);

//========================================================= TILEVIEWS ===============================================================

/**
 * @brief Create a wrapping tileview object
 * 
 * @param parent 
 * @param num_tiles 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_wrapping_tileview(lv_obj_t *parent, int num_tiles);

/**
 * @brief Create a tileview object
 * 
 * @param parent 
 * @param num_tiles 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_tileview(lv_obj_t *parent, int num_tiles);

//========================================================= ROLLERS =================================================================

/**
 * @brief Create a custom roller object
 * 
 * @param parent 
 * @param options If the max and step is not zero, a range will be calculated depending on min/max/step
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* create_custom_roller(lv_obj_t* parent, const RollerOptions& options);

}
