/**
 * @file UIUtils.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

extern "C"
{
#include <lvgl/lvgl.h>
}

#include <string_view>

namespace UIUtils
{
/**
 * @brief Get the child by object class
 * 
 * @param container 
 * @param childClass 
 * @return lv_obj_t* 
 */
[[nodiscard]] lv_obj_t* get_child_by_class(lv_obj_t* container, const lv_obj_class_t* childClass);

/**
 * @brief Get the current scroll selection object
 * 
 * @param parent_container 
 * @param scroll_name 
 * @return lv_obj_t* 
 * 
 * @note Ensure you have on the scroll container the flags
 * @note lv_obj_set_scroll_snap_x(scroll_container, LV_SCROLL_SNAP_CENTER);
 * @note lv_obj_add_flag(scroll_container, LV_OBJ_FLAG_SCROLL_ONE);
 */
[[nodiscard]] lv_obj_t* get_current_scroll_selection(lv_obj_t* parent_container, std::string_view scroll_name);

/**
 * @brief Returns the horizontal alignment offset relative to center from user click for a dlc bar
 * 
 * @param bar 
 * @param next_pos 
 * @return int32_t 
 */
int32_t compute_horiz_dlc_guide_pos(lv_obj_t* bar, lv_obj_t* guide, int32_t next_pos);

/**
 * @brief Returns the vertical alignment offset relative to center from user click for a dlc bar
 * 
 * @param bar 
 * @param guide 
 * @param next_pos 
 * @return int32_t 
 */
int32_t compute_vertical_dlc_guide_pos(lv_obj_t* bar, lv_obj_t* guide, int32_t next_pos);

/**
 * @brief Returns the maximum horizontal position relative to the left x position for a dlc bar indicator
 * 
 * @param bar 
 * @return int32_t 
 */
int32_t compute_horiz_dlc_indicator_max_pos(lv_obj_t* bar, lv_obj_t* indicator);

/**
 * @brief Returns the maximum vertical position relative to the uppper y position for a dlc bar indicator
 * 
 * @param bar 
 * @return int32_t 
 */
int32_t compute_vert_dlc_indicator_max_pos(lv_obj_t* bar, lv_obj_t* indicator);

/**
 * @brief Compute the horizontal dlc guide position with increment 
 * @note Pass the guide's current position so there is no need to recalculate
 * 
 * @param bar 
 * @param barGuide Cached pointer to the bar guide 
 * @param currentGuidePos Cached current guide position
 * @param increment Can be positive or negative
 * @return The returned direction can be 1 , -1 or 0 for error 
 */
int get_horizontal_dlc_guide_next_pos(lv_obj_t* bar, lv_obj_t* barGuide, int32_t& currentGuidePos, int32_t increment);


/**
 * @brief Compute the vertical dlc guide position with increment 
 * @note Pass the guide's current position so there is no need to recalculate
 * 
 * @param bar 
 * @param barGuide Cached pointer to the bar guide 
 * @param currentGuidePos Cached current guide position
 * @param increment Can be positive or negative
 * @return The returned direction can be 1 , -1 or 0 for error 
 */
int get_vertical_dlc_guide_next_pos(lv_obj_t* bar, lv_obj_t* barGuide, int32_t& currentGuidePos, int32_t increment);

/**
 * @brief Get the percentage value from the ref width
 * 
 * @param ref 
 * @param percent 
 * @return int32_t 
 */
int32_t percentage_w(lv_obj_t* ref, int32_t percent);

/**
 * @brief Get the percentage value from the ref height
 * 
 * @param ref 
 * @param percent 
 * @return int32_t 
 */
int32_t percentage_h(lv_obj_t* ref, int32_t percent);

/**
 * @brief Get the percentage from value
 * 
 * @param value 
 * @param percent 
 * @return int32_t 
 */
int32_t percentage(int32_t value, int32_t percent);

/**
 * @brief Disable recursively all the widgets from the group
 * 
 */
void disable_widget_tree(lv_obj_t* group);

/**
 * @brief Enable recursively all the widgets from the group
 * 
 */
void enable_widget_tree(lv_obj_t* group);

}// namespace UI