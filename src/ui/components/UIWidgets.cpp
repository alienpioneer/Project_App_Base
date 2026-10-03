/**
 * @file UIWidgets.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "ui/components/UIWidgets.hpp"

#include <cstdint>
#include <string>
#include <cstdlib>

#include "ui/resources.h"
#include "ui/components/StyleManager.hpp"
#include "ui/components/UIUtils.hpp"

namespace UIWidgets 
{

//========================================================= ROLLERS ================================================================

lv_obj_t* create_roller_with_label(lv_obj_t* parent, const UIComponents::Size size, const int32_t spacing, const UIComponents::RollerOptions& options, const char* labelText)
{
    lv_obj_t* group = UIComponents::create_container(parent, size);
    lv_obj_set_layout(group, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(group, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(group, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(group, spacing, 0);
    lv_obj_set_style_pad_all(group, 0, 0);

    lv_obj_t* roller = create_custom_roller(group, options);
    lv_obj_set_name_static(roller, "roller");
    
    lv_obj_t * label = lv_label_create(group);
    lv_obj_set_name_static(label, "label");

    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);

    if (labelText && labelText[0] != '\0')
    {
        lv_label_set_text(label, labelText);
    }
    
    lv_obj_set_width(label, LV_PCT(100));
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);

    return group;
}

//========================================================= SPINBOXES ===============================================================

lv_obj_t* create_horizontal_spinbox(lv_obj_t* parent, const UIComponents::Size size, const UIComponents::SpinboxValues& spinboxValues, const lv_font_t* font)
{
    lv_obj_t* group = UIComponents::create_container(parent, size);
    lv_obj_set_layout(group, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(group, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    int32_t spinBoxGrpWidth = UIUtils::percentage(size.width, 95);
    int32_t spinBoxGrpHeight = UIUtils::percentage(size.height, 95);

    if (spinboxValues.label && spinboxValues.label[0] != '\0')
    {
        lv_obj_update_layout(group);
        lv_obj_t * label = lv_label_create(group);
        lv_obj_set_name_static(label, "spinboxLabel");

        lv_label_set_text(label, spinboxValues.label);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(label, lv_color_white(), 0);

        if(font)
        {
            lv_obj_set_style_text_font(label, font, 0);
        }

        lv_obj_update_layout(label);

        int32_t labelWidth = 0; 

        if(spinboxValues.labelSizePercentage != 0)
        {
            labelWidth = UIUtils::percentage(size.width, spinboxValues.labelSizePercentage);
            lv_obj_set_width(label, labelWidth);
        }
        else
        {
            lv_obj_set_width(label, LV_SIZE_CONTENT);
            labelWidth = lv_obj_get_width(label);
        }
        
        spinBoxGrpWidth -= labelWidth;
    }

    lv_obj_t* spinBoxGrp = UIComponents::create_container(group, {spinBoxGrpWidth, spinBoxGrpHeight});
    lv_obj_set_name_static(spinBoxGrp, "spinboxGrp");
    lv_obj_set_layout(spinBoxGrp, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(spinBoxGrp, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(spinBoxGrp, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(spinBoxGrp, 0, 0);

    lv_obj_t* btnMinus = UIComponents::create_square_button(spinBoxGrp, {UIUtils::percentage(size.height,90), UIUtils::percentage(size.height,90)}, "-");
    lv_obj_set_name_static(btnMinus, "minusBtn");
    lv_obj_set_user_data(btnMinus, reinterpret_cast<void*>((uintptr_t)spinboxValues.minValue));
    lv_obj_set_style_text_font(btnMinus, font, 0);

    // Buttons are UIUtils::percentage(size.height,90) and keep 5% inbetween
    int32_t labelContainerAvailableWidth = spinBoxGrpWidth - 2*UIUtils::percentage(size.height,90)-UIUtils::percentage(size.width,5);

    lv_obj_t* valueContainer = UIComponents::create_label_container( spinBoxGrp, {UIUtils::percentage(labelContainerAvailableWidth,80), UIUtils::percentage(size.height,80)}, 
                                                                font, std::to_string(spinboxValues.defaultValue).c_str());
    lv_obj_set_name_static(valueContainer, "valueContainer");

    lv_obj_t* btnPlus = UIComponents::create_square_button(spinBoxGrp, {UIUtils::percentage(size.height,90), UIUtils::percentage(size.height,90)}, "+");
    lv_obj_set_name_static(btnPlus, "plusBtn");
    lv_obj_set_user_data(btnPlus, reinterpret_cast<void*>((uintptr_t)spinboxValues.maxValue));
    lv_obj_set_style_text_font(btnPlus, font, 0);

    // DEBUG
    // lv_obj_add_style(group, &UI::StyleManager::style_containers, 0);
    // lv_obj_add_style(spinBoxGrp, &UI::StyleManager::style_containers, 0);
    // lv_obj_add_style(valueContainer, &UI::StyleManager::style_containers, 0);

    lv_obj_add_event_cb(btnMinus, [](lv_event_t* e)
        {
            lv_obj_t* btn = lv_event_get_target_obj(e);
            int32_t inc = static_cast<uint32_t>((uintptr_t)lv_event_get_user_data(e));
            int32_t minValue = static_cast<uint32_t>((uintptr_t)lv_obj_get_user_data(btn));

            lv_obj_t* container = lv_obj_get_parent(btn);

            if(!container)
            {
                return;
            }

            lv_obj_t* valueContainer = lv_obj_get_child_by_name(container, "valueContainer");

            if(!valueContainer)
            {
                return;
            }

            lv_obj_t* label = lv_obj_get_child_by_name(valueContainer, "label");

            if(!label)
            {
                return;
            }

            int32_t value = std::stoi(lv_label_get_text(label));

            value -= inc;

            if(value < minValue)
            {
                value = minValue;
            }

            lv_label_set_text(label, std::to_string(value).c_str());

        }, LV_EVENT_CLICKED, reinterpret_cast<void*>((uintptr_t)spinboxValues.increment));

    lv_obj_add_event_cb(btnPlus, [](lv_event_t* e)
        {
            lv_obj_t* btn = lv_event_get_target_obj(e);
            int32_t inc = static_cast<uint32_t>((uintptr_t)lv_event_get_user_data(e));
            int32_t maxValue = static_cast<uint32_t>((uintptr_t)lv_obj_get_user_data(btn));

            // std::cout << "inc " << inc << " maxValue " << maxValue << std::endl;

            lv_obj_t* container = lv_obj_get_parent(btn);

            if(!container)
            {
                return;
            }

            lv_obj_t* valueContainer = lv_obj_get_child_by_name(container, "valueContainer");

            if(!valueContainer)
            {
                return;
            }

            lv_obj_t* label = lv_obj_get_child_by_name(valueContainer, "label");

            if(!label)
            {
                return;
            }

            int32_t value = std::stoi(lv_label_get_text(label));

            value += inc;

            if(value > maxValue)
            {
                value = maxValue;
            }

            lv_label_set_text(label, std::to_string(value).c_str());

        }, LV_EVENT_CLICKED, reinterpret_cast<void*>((uintptr_t)spinboxValues.increment));

    return group;
}

lv_obj_t* create_vertical_spinbox_group(lv_obj_t* parent, const UIComponents::Size size, const int32_t spacing, const UIComponents::SpinboxValues& spinboxValueMin, const UIComponents::SpinboxValues& spinboxValueMax, const lv_font_t* font)
{
    lv_obj_t* group = UIComponents::create_container(parent, size);
    // lv_obj_add_style(group, &UI::StyleManager::style_containers, 0);
    lv_obj_set_layout(group, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(group, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(group, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(group, spacing, 0);

    int32_t spinBoxGrpHeight = UIUtils::percentage(size.height, 45);

    // Upper spinbox
    lv_obj_t* upperSpinBox = UIWidgets::create_horizontal_spinbox(group, {size.width, spinBoxGrpHeight}, spinboxValueMin, font);
    lv_obj_set_name_static(upperSpinBox, "upperSpinbox");
    lv_obj_update_layout(upperSpinBox);

    // Lower spinbox
    lv_obj_t* lowerSpinBox = UIWidgets::create_horizontal_spinbox(group, {size.width, spinBoxGrpHeight}, spinboxValueMax, font);
    lv_obj_set_name_static(lowerSpinBox, "lowerSpinbox");
    lv_obj_update_layout(lowerSpinBox);
    
    // DEBUG
    // lv_obj_add_style(upperSpinBoxGrp, &UI::StyleManager::style_containers, 0);
    // lv_obj_add_style(lowerSpinBoxGrp, &UI::StyleManager::style_containers, 0);

    // The min/max values syncronization !
    lv_obj_t* upperSpinBoxPlusBtn = lv_obj_find_by_name(upperSpinBox, "plusBtn");
    lv_obj_t* lowerSpinBoxMinusBtn = lv_obj_find_by_name(lowerSpinBox, "minusBtn");

    lv_obj_add_event_cb(upperSpinBoxPlusBtn, [](lv_event_t* e)
        {
            lv_obj_t* btn = lv_event_get_target_obj(e);

            lv_obj_t* upperSpinbox = lv_obj_get_parent(btn);

            if(!upperSpinbox)
            {
                return;
            }

            lv_obj_t* upperLabel = lv_obj_find_by_name(upperSpinbox, "label");

            lv_obj_t* lowerSpinbox = static_cast<lv_obj_t*>(lv_event_get_user_data(e));

            if(!lowerSpinbox)
            {
                return;
            }

            lv_obj_t* lowerLabel = lv_obj_find_by_name(lowerSpinbox, "label");

            if(!lowerLabel)
            {
                return;
            }

            int32_t minValue = std::stoi(lv_label_get_text(upperLabel));
            int32_t maxValue = std::stoi(lv_label_get_text(lowerLabel));

            if(minValue > maxValue)
            {
                lv_label_set_text(lowerLabel, std::to_string(minValue).c_str());
            }

        }, LV_EVENT_CLICKED, lowerSpinBox);

    lv_obj_add_event_cb(lowerSpinBoxMinusBtn, [](lv_event_t* e)
        {
            lv_obj_t* btn = lv_event_get_target_obj(e);

            lv_obj_t* lowerSpinbox = lv_obj_get_parent(btn);

            if(!lowerSpinbox)
            {
                return;
            }

            lv_obj_t* lowerLabel = lv_obj_find_by_name(lowerSpinbox, "label");

            lv_obj_t* upperSpinbox = static_cast<lv_obj_t*>(lv_event_get_user_data(e));

            if(!upperSpinbox)
            {
                return;
            }

            lv_obj_t* upperLabel = lv_obj_find_by_name(upperSpinbox, "label");

            if(!upperLabel)
            {
                return;
            }

            int32_t minValue = std::stoi(lv_label_get_text(upperLabel));
            int32_t maxValue = std::stoi(lv_label_get_text(lowerLabel));

            if( maxValue < minValue)
            {
                lv_label_set_text(upperLabel, std::to_string(maxValue).c_str());
            }

        }, LV_EVENT_CLICKED, upperSpinBox);

    return group;
}

// ====================================================== BARS =========================================================

lv_obj_t* create_horiz_dlc_bar(lv_obj_t* parent, const UIComponents::Size size, const int32_t indicatorWidth, const int32_t guideWidth, bool showIndicator)
{
    lv_obj_t* bar = UIComponents::create_container(parent, size);
    lv_obj_add_flag(bar, LV_OBJ_FLAG_CLICKABLE);

    // Central guide indicator
    lv_obj_t* guide = lv_obj_create(bar);
    lv_obj_clear_flag(guide, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_name_static(guide, "guide");
    lv_obj_set_size(guide, guideWidth, LV_PCT(100)); 
    lv_obj_center(guide);

    if(!showIndicator)
    {
        lv_obj_add_flag(guide, LV_OBJ_FLAG_HIDDEN);
    }

    // Central line indicator
    lv_obj_t* indicator = lv_obj_create(bar);
    lv_obj_clear_flag(indicator, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_name_static(indicator, "indicator");
    lv_obj_set_size(indicator, indicatorWidth, LV_PCT(100)); 

    lv_obj_align(indicator, LV_ALIGN_CENTER, 0, 0);

    return bar;
}

lv_obj_t* create_vertical_dlc_bar(lv_obj_t* parent, const UIComponents::Size size, const int32_t indicatorHeight, const int32_t guideHeight, bool showIndicator)
{
    lv_obj_t* bar = UIComponents::create_container(parent, size);
    lv_obj_add_flag(bar, LV_OBJ_FLAG_CLICKABLE);

    // Central guide indicator
    lv_obj_t* guide = lv_obj_create(bar);
    lv_obj_clear_flag(guide, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_name_static(guide, "guide");
    lv_obj_set_size(guide, LV_PCT(100), guideHeight); 
    lv_obj_center(guide);

    if(!showIndicator)
    {
        lv_obj_add_flag(guide, LV_OBJ_FLAG_HIDDEN);
    }

    // Central line indicator
    lv_obj_t* indicator = lv_obj_create(bar);
    lv_obj_clear_flag(indicator, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_name_static(indicator, "indicator");
    lv_obj_set_size(indicator, LV_PCT(100), indicatorHeight);

    lv_obj_align(indicator, LV_ALIGN_CENTER, 0, 0);

    return bar;
}

lv_obj_t* create_vertical_fill_dlc_bar(lv_obj_t* parent, const UIComponents::Size size, const int32_t guideHeight, bool showIndicator)
{
    lv_obj_t* bar = UIComponents::create_container(parent, size);
    lv_obj_add_flag(bar, LV_OBJ_FLAG_CLICKABLE);

    // Fill bar indicator
    lv_obj_t* indicator = lv_obj_create(bar);
    lv_obj_clear_flag(indicator, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(indicator, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_name_static(indicator, "indicator");
    lv_obj_set_size(indicator, LV_PCT(100), LV_PCT(50)); 
    lv_obj_align(indicator, LV_ALIGN_BOTTOM_MID, 0, 0);

    // Central guide indicator
    lv_obj_t* guide = lv_obj_create(bar);
    lv_obj_clear_flag(guide, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(guide, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_name_static(guide, "guide");
    lv_obj_set_size(guide, LV_PCT(100), guideHeight); 
    lv_obj_center(guide);

    if(!showIndicator)
    {
        lv_obj_add_flag(guide, LV_OBJ_FLAG_HIDDEN);
    }

    return bar;
}

lv_obj_t* create_horiz_levels_bar(lv_obj_t* parent, const UIComponents::Size size, const lv_image_dsc_t* leftIcon, const lv_image_dsc_t* rightIcon, uint32_t levelNumber)
{
    lv_obj_t* group = UIComponents::create_container(parent, size);
    lv_obj_set_layout(group, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(group, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_update_layout(group);

    lv_obj_t* leftBtn = UIComponents::create_square_button_icon(group, {UIUtils::percentage(size.height,90), UIUtils::percentage(size.height,90)}, leftIcon, 60);
    lv_obj_set_name_static(leftBtn, "leftButton");

    int32_t levelContainerAvailableWidth = size.width - 2*UIUtils::percentage(size.height,90)-UIUtils::percentage(size.width,5);

    lv_obj_t* levelContainer = UIComponents::create_level_indicator( group, {UIUtils::percentage(levelContainerAvailableWidth,90), UIUtils::percentage(size.height,50)}, levelNumber);
    lv_obj_set_name_static(levelContainer, "levelContainer");

    lv_obj_t* rightBtn = UIComponents::create_square_button_icon(group, {UIUtils::percentage(size.height,90), UIUtils::percentage(size.height,90)}, rightIcon, 60);
    lv_obj_set_name_static(rightBtn, "rightButton");

    // lv_obj_add_style(group, &UI::StyleManager::style_containers, 0);

    // Set current level into the widget data - default 1
    lv_obj_set_user_data(group, reinterpret_cast<void*>((uintptr_t)1));

    lv_obj_add_event_cb(leftBtn, [](lv_event_t* e)
        {
            lv_obj_t* btn = lv_event_get_target_obj(e);

            if(!btn)
            {
                return;
            }

            lv_obj_t* container = lv_obj_get_parent(btn);

            if(!container)
            {
                return;
            }

            lv_obj_t* levelContainer = lv_obj_find_by_name(container, "levelContainer");

            if(!levelContainer)
            {
                return;
            }

            lv_obj_t* indicator = lv_obj_find_by_name(levelContainer, "indicator");

            if(!indicator)
            {
                return;
            }

            uint32_t currentLevel = static_cast<uint32_t>((uintptr_t)lv_obj_get_user_data(container));
            uint32_t levelNumber = static_cast<uint32_t>((uintptr_t)lv_event_get_user_data(e));
            int32_t availableWidth = lv_obj_get_width(levelContainer);

            if(currentLevel > 1)
            {
                currentLevel--;

                if (levelNumber != 0)
                {
                    lv_obj_set_width(indicator, UIUtils::percentage(availableWidth, currentLevel*100/levelNumber)); 
                }
                else
                {
                    LV_LOG_ERROR("create_horiz_levels_bar() lv_obj_set_width() leftBtn levelNumber is 0 !");
                }
            }
             
            lv_obj_set_user_data(container, reinterpret_cast<void*>((uintptr_t)currentLevel));

        }, LV_EVENT_CLICKED, reinterpret_cast<void*>((uintptr_t)levelNumber));

    lv_obj_add_event_cb(rightBtn, [](lv_event_t* e)
        {
            lv_obj_t* btn = lv_event_get_target_obj(e);

            if(!btn)
            {
                return;
            }

            lv_obj_t* container = lv_obj_get_parent(btn);

            if(!container)
            {
                return;
            }

            lv_obj_t* levelContainer = lv_obj_find_by_name(container, "levelContainer");

            if(!levelContainer)
            {
                return;
            }

            lv_obj_t* indicator = lv_obj_find_by_name(levelContainer, "indicator");

            if(!indicator)
            {
                return;
            }

            uint32_t currentLevel = static_cast<uint32_t>((uintptr_t)lv_obj_get_user_data(container));
            uint32_t levelNumber = static_cast<uint32_t>((uintptr_t)lv_event_get_user_data(e));
            int32_t availableWidth = lv_obj_get_width(levelContainer);

            if(currentLevel < levelNumber)
            {
                currentLevel++;

                if (levelNumber != 0)
                {
                    lv_obj_set_width(indicator, UIUtils::percentage(availableWidth, currentLevel*100/levelNumber)); 
                }
                else
                {
                    LV_LOG_ERROR("create_horiz_levels_bar() lv_obj_set_width() rightBtn levelNumber is 0 !");
                }
            }
             
            lv_obj_set_user_data(container, reinterpret_cast<void*>((uintptr_t)currentLevel));

        }, LV_EVENT_CLICKED, reinterpret_cast<void*>((uintptr_t)levelNumber));

    return group;
}

// ====================================================== ANIMATED WIDGETS ===============================================

lv_obj_t * create_animated_dot(lv_obj_t * parent, UIComponents::Range range, lv_color_t color, uint32_t duration, bool oscillate)
{
    lv_obj_t * container = lv_obj_create(parent);
    lv_obj_remove_style_all(container);
    lv_obj_set_size(container, range.max, range.max);
    // Important so that the child is not clipped by the parent
    lv_obj_add_flag(container, LV_OBJ_FLAG_OVERFLOW_VISIBLE);

    lv_obj_t * outline = lv_obj_create(container);
    lv_obj_set_name_static(outline, "outline");
    lv_obj_remove_style_all(outline);
    lv_obj_set_size(outline, range.max, range.max);
    lv_obj_set_style_radius(outline, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(outline, 2, 0);
    lv_obj_set_style_border_color(outline, color, 0);
    lv_obj_set_style_bg_opa(outline, LV_OPA_TRANSP, 0);
    lv_obj_center(outline);

    lv_obj_t * dot = lv_obj_create(container);
    lv_obj_set_name_static(dot, "dot");
    lv_obj_remove_style_all(dot);
    lv_obj_set_size(dot, range.min, range.min);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(dot, color, 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_center(dot);

    lv_anim_t dot_animation{};
    lv_anim_init(&dot_animation);
    lv_anim_set_var(&dot_animation, outline);
    lv_anim_set_exec_cb(&dot_animation, [](void * obj, int32_t value)
        { 
            lv_obj_set_size((lv_obj_t *)obj, value, value); 
        });
    lv_anim_set_values(&dot_animation, range.min, range.max);
    lv_anim_set_duration(&dot_animation, duration);
    lv_anim_set_repeat_count(&dot_animation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&dot_animation, lv_anim_path_ease_in_out);

    if(oscillate)
    {
        lv_anim_set_playback_duration(&dot_animation, duration);
    }

    lv_anim_start(&dot_animation);

    // Put the outline handle in the container user data
    lv_obj_set_user_data(container, outline);

    return container;
}

// ====================================================== KEYBOARDS ====================================================

static void kb_draw_event_cb(lv_event_t *e)
{
    lv_obj_t *kb = lv_event_get_target_obj(e);
    lv_draw_task_t *draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t *base_dsc = static_cast<lv_draw_dsc_base_t *>(lv_draw_task_get_draw_dsc(draw_task));

    if(base_dsc->part == LV_PART_ITEMS)
    {
        bool pressed = (lv_keyboard_get_selected_button(kb) == base_dsc->id1 && lv_obj_has_state(kb, LV_STATE_PRESSED));
        
        lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);

        // LV_LOG_USER("part=%d id1=%d text=%s", base_dsc->part, base_dsc->id1, label_draw_dsc ? label_draw_dsc->text : "null");

        if(!fill_draw_dsc) return;

        UI::ThemePalette currentPalette = UI::StyleManager::get_current_palette(); 

        if(base_dsc->id1 == 41) 
        {
            fill_draw_dsc->color = pressed ? lv_color_darken(currentPalette.keyboard_cancel, 32) : currentPalette.keyboard_cancel;
        }
        else if(base_dsc->id1 == 43) 
        {
            fill_draw_dsc->color = pressed ? lv_color_darken(currentPalette.keyboard_ok, 32) : currentPalette.keyboard_ok;
        }
        else
        {
            fill_draw_dsc->color = pressed ? lv_color_darken(currentPalette.keyboard_keys, 32) : currentPalette.keyboard_keys;
        }
    }
}

lv_obj_t* create_pswd_popup(lv_obj_t* parent, const UIComponents::Size size, const lv_font_t* font)
{
    static const char * kb_map[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", LV_SYMBOL_BACKSPACE, "\n",
                                    "a", "z", "e", "r", "t", "y", "u", "i", "o", "p", "\n",
                                    "q", "s", "d", "f", "g", "h", "j", "k", "l", "m", "\n",
                                    "w", "x", "c", "v", "b", "n", ".", "!", "#", "_", "\n",
                                    LV_SYMBOL_CLOSE, " ", LV_SYMBOL_OK, NULL};

    static const lv_buttonmatrix_ctrl_t kb_ctrl[] = {   LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4,
                                                        LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4,
                                                        LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4,
                                                        LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4,
                                                        LV_BUTTONMATRIX_CTRL_WIDTH_4, (lv_buttonmatrix_ctrl_t)(LV_BUTTONMATRIX_CTRL_HIDDEN | LV_BUTTONMATRIX_CTRL_WIDTH_10), LV_BUTTONMATRIX_CTRL_WIDTH_4};

    lv_obj_t* overlay = lv_obj_create(parent);
    lv_obj_set_name_static(overlay, "overlay");
    lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
    lv_obj_clear_flag(overlay, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(overlay, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *kb = lv_keyboard_create(overlay);
    lv_obj_set_name_static(kb, "keyboard");
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_USER_1);
    lv_obj_set_size(kb, size.width, size.height);
    lv_obj_align(kb, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* text_area = lv_textarea_create(overlay);
    lv_obj_set_name_static(text_area, "text_area");
    lv_obj_set_width(text_area, size.width);
    lv_textarea_set_password_mode(text_area, true);
    lv_textarea_set_one_line(text_area, true);
    lv_obj_align_to(text_area, kb, LV_ALIGN_OUT_TOP_MID, 0, -10);
    lv_keyboard_set_textarea(kb, text_area);

    if (font)
    {
        lv_obj_set_style_text_font(kb, font , 0);
        lv_obj_set_style_text_font(text_area, font , 0);
    }

    lv_obj_add_flag(kb, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

    // Store a pointer to the parent
    lv_obj_set_user_data(kb, overlay);

    lv_obj_add_event_cb(kb, kb_draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);

    return overlay;
}

//========================================================= POPUPS ===================================================================

lv_obj_t* create_roller_popup(lv_obj_t* parent, const UIComponents::Size size, const UIComponents::RollerOptions& options)
{
    lv_obj_t* group = UIComponents::create_container(parent, size);
    lv_obj_set_layout(group, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(group, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(group, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* roller = create_custom_roller(group, options);
    lv_obj_set_name_static(roller, "roller");

    lv_obj_t* okBtn = UIComponents::create_square_button(group, {LV_PCT(80), LV_PCT(20)}, "OK");
    lv_obj_set_name_static(okBtn, "button");

    return group;
}

}