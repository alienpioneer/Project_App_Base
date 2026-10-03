/**
 * @file UIComponents.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "ui/components/UIComponents.hpp"

#include <cstdint>

#include "ui/components/StyleManager.hpp"
#include "ui/components/UIUtils.hpp"

namespace UIComponents 
{

// ====================================================== CALLBACKS =========================================================================

static void two_exclusive_btn_group_cb(lv_event_t *e)
{
    lv_obj_t* group = lv_event_get_current_target_obj(e);

    if (!group)
    {
        return;
    }

    lv_obj_t* cur_btn = lv_event_get_target_obj(e);

    if (!cur_btn)
    {
        return;
    }

    lv_obj_t* btn1 = lv_obj_get_child_by_name(group, "button1");
    lv_obj_t* btn2 = lv_obj_get_child_by_name(group, "button2");

    if (btn1 == cur_btn)
    {
        // std::cout << "Button 1" << std::endl;
        lv_obj_add_state(btn1, LV_STATE_CHECKED);
        lv_obj_remove_flag(btn1, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_state(btn2, LV_STATE_CHECKED);
        lv_obj_add_flag(btn2, LV_OBJ_FLAG_CLICKABLE);
    }
    else if (btn2 == cur_btn)
    {
        // std::cout << "Button 2" << std::endl;
        lv_obj_add_state(btn2, LV_STATE_CHECKED);
        lv_obj_remove_flag(btn2, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_state(btn1, LV_STATE_CHECKED);
        lv_obj_add_flag(btn1, LV_OBJ_FLAG_CLICKABLE);
    }
}

static void three_exclusive_btn_group_cb(lv_event_t *e)
{
    lv_obj_t* group = lv_event_get_current_target_obj(e);

    if (!group)
    {
        return;
    }

    lv_obj_t* cur_btn = lv_event_get_target_obj(e);

    if (!cur_btn)
    {
        return;
    }

    lv_obj_t* btn1 = lv_obj_get_child_by_name(group, "button1");
    lv_obj_t* btn2 = lv_obj_get_child_by_name(group, "button2");
    lv_obj_t* btn3 = lv_obj_get_child_by_name(group, "button3");

    lv_obj_clear_state(btn1, LV_STATE_CHECKED);
    lv_obj_clear_state(btn2, LV_STATE_CHECKED);
    lv_obj_clear_state(btn3, LV_STATE_CHECKED);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(btn3, LV_OBJ_FLAG_CLICKABLE);

    if (btn1 == cur_btn)
    {
        lv_obj_add_state(btn1, LV_STATE_CHECKED);
        lv_obj_remove_flag(btn1, LV_OBJ_FLAG_CLICKABLE);
    }
    else if (btn2 == cur_btn)
    {
        lv_obj_add_state(btn2, LV_STATE_CHECKED);
        lv_obj_remove_flag(btn2, LV_OBJ_FLAG_CLICKABLE);
    }
    else if (btn3 == cur_btn)
    {
        lv_obj_add_state(btn3, LV_STATE_CHECKED);
        lv_obj_remove_flag(btn3, LV_OBJ_FLAG_CLICKABLE);
    }
}

// ====================================================== BUTTONS ===========================================================================

lv_obj_t* create_round_button_icon(lv_obj_t* parent, const int32_t size, const lv_image_dsc_t* icon_src, uint32_t icon_percent)
{
    lv_obj_t* btn = lv_button_create(parent);
    lv_obj_set_size(btn, size, size);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    if (icon_src)
    {
        lv_obj_t* btn_icon = lv_image_create(btn);
        lv_obj_set_name_static(btn_icon, "icon");
        lv_image_set_src(btn_icon, icon_src);
        // uint32_t scale_factor = static_cast<uint32_t>((static_cast<float>(icon_percent)/static_cast<float>(100))*static_cast<float>(LV_SCALE_NONE));
        
        // Get image dimensions
        float img_w = static_cast<float>(icon_src->header.w);
        // Resize icon img to icon_percent of the size
        float cont_w = static_cast<float>(size) * static_cast<float>(icon_percent)/100;

        uint32_t img_scale = 1;
        if (img_w != 0)
        {
            img_scale = static_cast<uint32_t>(cont_w/img_w*256.0f);
        }
        else
        {
            LV_LOG_ERROR("create_round_button_icon() img_w is 0 !");
        }

        lv_image_set_scale(btn_icon, img_scale);
        lv_image_set_antialias(btn_icon, true);

        lv_obj_center(btn_icon);
    }

    return btn;
}

lv_obj_t* create_square_button_icon(lv_obj_t* parent, const Size size, const lv_image_dsc_t* icon_src, uint32_t icon_scale)
{
    lv_obj_t* btn = lv_button_create(parent);
    lv_obj_set_size(btn, size.width, size.height);

    if (icon_src)
    {
        lv_obj_t* btn_icon = lv_image_create(btn);
        lv_obj_set_name_static(btn_icon, "icon");
        lv_image_set_src(btn_icon, icon_src);
        // lv_image_set_inner_align(btn_icon, LV_IMAGE_ALIGN_TILE);
        uint32_t scale_factor = static_cast<uint32_t>((static_cast<float>(icon_scale)/static_cast<float>(100))*static_cast<float>(LV_SCALE_NONE));
        lv_image_set_scale(btn_icon, scale_factor);
        lv_obj_center(btn_icon);
    }

    return btn;
}

lv_obj_t* create_round_button(lv_obj_t* parent, const int32_t size, const char* label)
{
    lv_obj_t* btn = lv_button_create(parent);
    lv_obj_set_size(btn, size, size);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    if (label)
    {
        lv_obj_t* lbl = lv_label_create(btn);
        lv_obj_set_name_static(lbl, "label");
        lv_label_set_text(lbl, label);
        lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_center(lbl);
    }

    return btn;
}

lv_obj_t* create_square_button(lv_obj_t* parent, const Size size, const char* label)
{
    lv_obj_t* btn = lv_button_create(parent);
    lv_obj_set_size(btn, size.width, size.height);

    if (label)
    {
        lv_obj_t* lbl = lv_label_create(btn);
        lv_label_set_text(lbl, label);
        lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_center(lbl);
    }

    return btn;
}

lv_obj_t* create_invisible_button(lv_obj_t* parent, const Size size)
{
    lv_obj_t* hitbox = lv_obj_create(parent);
    lv_obj_remove_style_all(hitbox);
    lv_obj_set_size(hitbox, size.width, size.height);
    
    lv_obj_add_flag(hitbox, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(hitbox, LV_OBJ_FLAG_SCROLLABLE);

    return hitbox;
}

lv_obj_t* create_horiz_two_exclusive_btn_group(lv_obj_t* parent, const Size size, const TwoButtonOptions& options)
{
    lv_obj_t* group = UIComponents::create_container(parent, size);
    lv_obj_set_layout(group, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(group, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // lv_obj_set_style_pad_gap(group, spacing, 0);
    lv_obj_set_style_pad_all(group, 0, 0);

    // Set buttons size to 90 percent height
    int32_t btn_size = (size.height * 90) / 100;

    lv_obj_t* btn1 = create_round_button(group, btn_size, options.label1);
    lv_obj_set_name_static(btn1, "button1");
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_remove_flag(btn1, LV_OBJ_FLAG_CLICKABLE);


    lv_obj_t* btn2 = create_round_button(group, btn_size, options.label2);
    lv_obj_set_name_static(btn2, "button2");
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_EVENT_BUBBLE);

    // Start with button 1 checked
    lv_obj_add_state(btn1, LV_STATE_CHECKED);

    lv_obj_add_event_cb(group, two_exclusive_btn_group_cb, LV_EVENT_CLICKED, nullptr);

    return group;
}

lv_obj_t* create_vert_two_exclusive_btn_group(lv_obj_t* parent, const Size size, const TwoButtonOptions& options)
{
    lv_obj_t* group = UIComponents::create_container(parent, size);
    lv_obj_set_layout(group, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(group, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(group, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // lv_obj_set_style_pad_gap(group, spacing, 0);
    lv_obj_set_style_pad_all(group, 0, 0);

    // Set buttons size to 90 percent width
    int32_t btn_size = (size.width * 90) / 100;

    lv_obj_t* btn1 = nullptr;
    if (options.label1 && options.label1[0] != '\0')
    {
        btn1 = create_round_button(group, btn_size, options.label1);
    }
    else
    {
        btn1 = create_round_button_icon(group, btn_size, options.iconSrc1, 80);
    }

    if(!btn1)
    {   
        lv_obj_delete(group);
        return nullptr;
    }
    
    lv_obj_set_name_static(btn1, "button1");
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t* btn2 = nullptr;
    if (options.label2 && options.label2[0] != '\0')
    {
        btn2 = create_round_button(group, btn_size, options.label2);
    }
    else
    {
        btn2 = create_round_button_icon(group, btn_size, options.iconSrc2, 80);
    }

    if(!btn2)
    {   
        lv_obj_delete(group);
        return nullptr;
    }
    
    lv_obj_set_name_static(btn2, "button2");
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_EVENT_BUBBLE);

    // Start with button 1 checked
    lv_obj_add_state(btn1, LV_STATE_CHECKED);
    lv_obj_remove_flag(btn1, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_add_event_cb(group, two_exclusive_btn_group_cb, LV_EVENT_CLICKED, nullptr);

    return group;
}

lv_obj_t* create_three_horiz_exclusive_btn_group(lv_obj_t* parent, const Size size, const ThreeButtonOptions& options)
{
    lv_obj_t* group = UIComponents::create_container(parent, size);
    lv_obj_set_layout(group, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(group, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(group, 0, 0);

    // Set buttons size to 90 percent height
    int32_t btn_size = (size.height * 88) / 100;

    lv_obj_t* btn1 = nullptr;
    if (options.label1 && options.label1[0] != '\0')
    {
        btn1 = create_round_button(group, btn_size, options.label1);
    }
    else
    {
        btn1 = create_round_button_icon(group, btn_size, options.iconSrc1, 60);
    }

    if(!btn1)
    {   
        lv_obj_delete(group);
        return nullptr;
    }
    
    lv_obj_set_name_static(btn1, "button1");
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t* btn2 = nullptr;
    if (options.label2 && options.label2[0] != '\0')
    {
        btn2 = create_round_button(group, btn_size, options.label2);
    }
    else
    {
        btn2 = create_round_button_icon(group, btn_size, options.iconSrc2, 60);
    }

    if(!btn2)
    {   
        lv_obj_delete(group);
        return nullptr;
    }
    
    lv_obj_set_name_static(btn2, "button2");
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_EVENT_BUBBLE);
    
    lv_obj_t* btn3 = nullptr;
    if (options.label3 && options.label3[0] != '\0')
    {
        btn3 = create_round_button(group, btn_size, options.label3);
    }
    else
    {
        btn3 = create_round_button_icon(group, btn_size, options.iconSrc3, 60);
    }

    if(!btn3)
    {   
        lv_obj_delete(group);
        return nullptr;
    }
    
    lv_obj_set_name_static(btn3, "button3");
    lv_obj_add_flag(btn3, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(btn3, LV_OBJ_FLAG_EVENT_BUBBLE);
    
    // Start with button 1 checked
    lv_obj_add_state(btn1, LV_STATE_CHECKED);
    lv_obj_remove_flag(btn1, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_add_event_cb(group, three_exclusive_btn_group_cb, LV_EVENT_CLICKED, nullptr);

    return group;
}

// ====================================================== SLIDERS ===========================================================================

lv_obj_t* create_slider_with_icon(lv_obj_t* parent, const Size size, const int32_t spacing, const lv_image_dsc_t& icon_src, const Range range)
{
    lv_obj_t* container = lv_obj_create(parent);
    lv_obj_set_size(container, size.width, size.height);
    // Remove properties
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_shadow_width(container, 0, 0);

    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // lv_obj_set_style_pad_all(container, spacing, spacing);
    lv_obj_set_style_pad_gap(container, spacing, 0);
    lv_obj_set_style_pad_all(container, 0, 0);

    if (icon_src.data)
    {
        lv_obj_t* img = lv_image_create(container);
        lv_obj_set_name_static(img, "icon");
        lv_image_set_src(img, &icon_src);
    }

    lv_obj_t* slider = lv_slider_create(container);
    lv_obj_set_name_static(slider, "slider");
    lv_obj_set_flex_grow(slider, 1);
    lv_obj_clear_flag(slider, LV_OBJ_FLAG_GESTURE_BUBBLE);

    lv_slider_set_range(slider, range.min, range.max);
    lv_slider_set_value(slider, range.min, LV_ANIM_OFF);

    // This can cause a crash with rounded corners ! 
    // lv_slider_set_range(slider, 0, 100);

    // Very important remove round corners on active objects - whithout => may crash if refresh rate high!
    // lv_obj_set_style_radius(slider, 0, LV_PART_MAIN);
    // lv_obj_set_style_radius(slider, 0, LV_PART_INDICATOR);

    return container;
}

// ====================================================== CONTAINERS ========================================================================

lv_obj_t* create_container(lv_obj_t* parent, const Size size)
{
    lv_obj_t * container = lv_obj_create(parent);
    lv_obj_remove_style_all(container);
    lv_obj_remove_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_width(container, size.width);
    lv_obj_set_height(container, size.height);
    return container;
}

lv_obj_t* create_base_container(lv_obj_t* parent)
{
    lv_obj_t * container = lv_obj_create(parent);
    lv_obj_remove_style_all(container);
    lv_obj_remove_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_width(container, lv_obj_get_width(parent));
    lv_obj_set_height(container, lv_obj_get_height(parent));
    return container;
}

lv_obj_t* create_swipe_container(lv_obj_t* parent, const Size size, const int32_t spacing, const SlideButtonItems& items)
{
    lv_obj_t* container = create_container(parent, size);

    // Root row container 
    lv_obj_set_style_pad_column(container, spacing, 0);
    lv_obj_set_layout(container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Left button 
    lv_obj_t* btn = create_square_button(container, {LV_PCT(45), LV_PCT(90)}, items.btnLabel);
    lv_obj_set_name_static(btn, "button");

    // Scrollable right container 
    lv_obj_t* scroll_container = lv_obj_create(container);
    lv_obj_set_name_static(scroll_container, "scroll_container");
    lv_obj_remove_style_all(scroll_container);
    lv_obj_set_style_bg_opa(scroll_container, LV_OPA_TRANSP, 0);
    lv_obj_set_size(scroll_container, LV_PCT(50), LV_PCT(90));  

    lv_obj_set_layout(scroll_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(scroll_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(scroll_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(scroll_container, items.spacing, 0);

    // Horizontal-only scrolling, no scroll bars visible
    lv_obj_add_flag(scroll_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(scroll_container, LV_OBJ_FLAG_SCROLL_ONE);   // No momentum 
    lv_obj_set_scroll_dir(scroll_container, LV_DIR_HOR);
    lv_obj_set_scrollbar_mode(scroll_container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_snap_x(scroll_container, LV_SCROLL_SNAP_CENTER);

    for (size_t i = 0; i < items.labelCount; i++) 
    {
        lv_obj_t *cell = lv_obj_create(scroll_container);
        lv_obj_remove_style_all(cell);
        lv_obj_set_size(cell, LV_PCT(80), LV_PCT(85));
        lv_obj_clear_flag(cell, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_pad_left(cell, 0, 0);
        lv_obj_set_style_pad_right(cell, 0, 0);
        // Add index as user data
        lv_obj_set_user_data(cell, (void*)(uintptr_t)(i));

        if(items.item_style)
        {
            lv_obj_add_style(cell, items.item_style, 0);
        }
                
        lv_obj_t *label = lv_label_create(cell);
        lv_label_set_text(label, items.labelList.at(i));
        lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
        // LV_SIZE_CONTENT very important here !
        lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }

    lv_obj_update_snap(scroll_container, LV_ANIM_OFF);

    return container;
}

// ====================================================== INDICATORS ========================================================================

lv_obj_t* create_swipe_dots_indicator(lv_obj_t* parent, lv_obj_t* tileview, const int32_t height, const uint8_t number)
{
    lv_obj_t* dots_container = lv_obj_create(parent);
    lv_obj_remove_style_all(dots_container);
    lv_obj_set_size(dots_container, LV_SIZE_CONTENT, height);
    lv_obj_set_flex_flow(dots_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(dots_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(dots_container, height, 0);
    lv_obj_remove_flag(dots_container, LV_OBJ_FLAG_CLICKABLE);

    // Create the N dots
    for(int i = 0; i < number; i++) 
    {
        lv_obj_t* dot = lv_obj_create(dots_container);
        lv_obj_set_size(dot, height, height);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);

        // lv_obj_set_style_border_width(dot, 0, 0);
        
        if (i == 0) 
        {
            lv_obj_set_style_bg_color(dot, lv_palette_main(LV_PALETTE_BLUE), 0);
        } 
        else
        {
            lv_obj_set_style_bg_color(dot, lv_palette_main(LV_PALETTE_GREY), 0);
        }
        
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
        lv_obj_remove_flag(dot, LV_OBJ_FLAG_CLICKABLE);
    }

    lv_obj_add_event_cb(tileview, [](lv_event_t* e) 
    {
            lv_obj_t* tv = lv_event_get_target_obj(e);
            lv_obj_t* dots_container = static_cast<lv_obj_t*>(lv_event_get_user_data(e));
            
            lv_obj_t* active_tile = lv_tileview_get_tile_active(tv);
            
            // Get active tile index
            uint32_t tile_idx = 0;
            for(uint32_t i = 0; i < lv_obj_get_child_count(tv); i++) 
            {
                if(lv_obj_get_child(tv, i) == active_tile)
                {
                    tile_idx = i;
                    break;
                }
            }
            
            // Change the style on the index circle
            for(uint32_t i = 0; i < lv_obj_get_child_count(dots_container); i++) 
            {
                lv_obj_t* dot = lv_obj_get_child(dots_container, i);

                if(i == tile_idx) 
                {
                    lv_obj_set_style_bg_color(dot, lv_palette_main(LV_PALETTE_BLUE), 0);
                } 
                else 
                {
                    lv_obj_set_style_bg_color(dot, lv_palette_main(LV_PALETTE_GREY), 0);
                }
            }
        }, LV_EVENT_VALUE_CHANGED, dots_container);

    return dots_container;
}

lv_obj_t* create_red_dot(lv_obj_t* parent, const int32_t size)
{
    lv_obj_t * dot = lv_obj_create(parent);
    lv_obj_remove_style_all(dot);
    lv_obj_set_size(dot, size, size);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0xFF0000), 0);

    return dot;
}

lv_obj_t* create_level_indicator(lv_obj_t* parent, const Size size, uint32_t levelNumber)
{
    lv_obj_t* container = UIComponents::create_container(parent, size);
    lv_obj_add_style(container, &UI::StyleManager::style_containers, 0);

    lv_obj_t* indicator = lv_obj_create(container);
    lv_obj_set_name_static(indicator, "indicator");
    lv_obj_set_style_border_width(indicator, 0, 0);
    lv_obj_clear_flag(indicator, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(indicator, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t* line = nullptr;
    for (uint32_t i = 1; i < levelNumber; i++)
    {
        line = UIComponents::create_container(container, {1, UIUtils::percentage(size.height,40),});
        lv_obj_align(line, LV_ALIGN_TOP_LEFT, i*(size.width/levelNumber), 0);
        lv_obj_add_style(line, &UI::StyleManager::style_containers, 0);
    }

    lv_obj_align(indicator, LV_ALIGN_LEFT_MID, 0, 0);

    if (levelNumber != 0)
    {
        lv_obj_set_size(indicator, UIUtils::percentage(size.width,100/levelNumber), LV_PCT(100)); 
    }
    else
    {
        LV_LOG_ERROR("create_level_indicator() lv_obj_set_size() levelNumber is 0 !");
    }

    return container;
}

//========================================================= LABELS ==========================================================================

lv_obj_t* create_label_container(lv_obj_t* parent, const Size size, const lv_font_t* font, const char* labelText)
{
    
    lv_obj_t* container = UIComponents::create_container(parent, size);

    lv_obj_t * label = lv_label_create(container);
    lv_obj_set_name_static(label, "label");

    if (font)
    {
        lv_obj_set_style_text_font(label, font, 0);
    }
    
    lv_obj_set_width(label, LV_PCT(100));

    if (labelText && labelText[0] != '\0')
    {
        lv_label_set_text(label, labelText);
    }

    lv_obj_center(label);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    
    return container;
}

//========================================================= TILEVIEWS =======================================================================

lv_obj_t* create_wrapping_tileview(lv_obj_t *parent, int num_tiles)
{
    lv_obj_t *tileview = lv_tileview_create(parent);
    lv_obj_remove_style_all(tileview);
    lv_obj_set_size(tileview, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scroll_dir(tileview, LV_DIR_HOR);
    lv_obj_set_scroll_snap_x(tileview, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);
    
    // lv_obj_remove_flag(tileview, LV_OBJ_FLAG_GESTURE_BUBBLE);

    for (int i = 0; i < num_tiles; i++) 
    {
        lv_obj_t *tile = lv_tileview_add_tile(tileview, i, 0, static_cast<lv_dir_t>(LV_DIR_LEFT | LV_DIR_RIGHT));
        std::string name = "tile" + std::to_string(i);
        lv_obj_set_name(tile, name.c_str());
    }

    lv_obj_add_event_cb(tileview, [](lv_event_t *e)
        {
            lv_obj_t* tileview = lv_event_get_target_obj(e);

            // Very important, teleport the tile on the swipe animation end
            if(lv_obj_is_scrolling(tileview))
            {
                return;
            } 

            lv_obj_t* activeTile = lv_tileview_get_tile_active(tileview);
            uint32_t tileCount = lv_obj_get_child_count(tileview);

            if (tileCount < 2)
            {
                return;
            }

            std::string lastTile = "tile"+std::to_string(tileCount-1);
            
            if ( activeTile == lv_obj_get_child_by_name(tileview, "tile0")) 
            {   
                lv_async_call([](void* arg)
                    {
                        lv_obj_t* tileview = static_cast<lv_obj_t*>(arg);
                        uint32_t count = lv_obj_get_child_count(tileview);
                        std::string realLastTile = "tile"+std::to_string(count-2);
                        lv_tileview_set_tile(tileview, lv_obj_get_child_by_name(tileview, realLastTile.c_str()), LV_ANIM_OFF);
                    }
                    ,tileview);
            } 
            else if ( activeTile == lv_obj_get_child_by_name(tileview, lastTile.c_str()) )
            {
                lv_async_call([](void* arg)
                    {
                        lv_obj_t* tileview = static_cast<lv_obj_t*>(arg);
                        lv_tileview_set_tile(tileview, lv_obj_get_child_by_name(tileview, "tile1"), LV_ANIM_OFF);
                    }
                    ,tileview);
            }

        }, LV_EVENT_SCROLL_END, nullptr);

    return tileview;
}

lv_obj_t* create_tileview(lv_obj_t *parent, int num_tiles)
{
    lv_obj_t *tileview = lv_tileview_create(parent);
    lv_obj_remove_style_all(tileview);
    lv_obj_set_size(tileview, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scroll_dir(tileview, LV_DIR_HOR);
    lv_obj_set_scroll_snap_x(tileview, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);

    for (int i = 0; i < num_tiles; i++) 
    {
        lv_obj_t *tile = lv_tileview_add_tile(tileview, i, 0, static_cast<lv_dir_t>(LV_DIR_LEFT | LV_DIR_RIGHT));
        std::string name = "tile" + std::to_string(i);
        lv_obj_set_name(tile, name.c_str());
    }

    lv_tileview_set_tile(tileview, lv_obj_get_child_by_name(tileview, "tile0"), LV_ANIM_OFF);

    return tileview;
}

//========================================================= ROLLERS =========================================================================

lv_obj_t* create_custom_roller(lv_obj_t* parent, const RollerOptions& options)
{
    lv_obj_t* roller = lv_roller_create(parent);

    // cppcheck-suppress knownConditionTrueFalse
    if( (options.max != 0) && (options.step != 0) )
    {
        std::string range;
        int32_t val = options.min;
        
        const int32_t range_size = std::abs(static_cast<int>(options.max-options.min))/options.step;

        // std::cout << "range_size " << range_size << std::endl;

        for(int i = 0; i<=range_size; i++)
        {
            range += std::to_string(val);

            if(i < range_size)
            {
                range += '\n';
            }

            val += options.step;
        }

        lv_roller_set_options(roller, range.c_str(), LV_ROLLER_MODE_NORMAL);
    }
    else
    {
        lv_roller_set_options(roller, options.options, LV_ROLLER_MODE_NORMAL);
    }

    if(options.font)
    {
        lv_obj_set_style_text_font(roller, options.font, 0);
    }

    if(options.selection_font)
    {
        lv_obj_set_style_text_font(roller, &lv_font_montserrat_42, LV_PART_SELECTED);
    }

    lv_roller_set_visible_row_count(roller, options.visible_items);

    // cppcheck-suppress knownConditionTrueFalse
    if( (options.max != 0) && (options.step != 0) )
    {
        const uint32_t range_size = std::abs(static_cast<int>(options.max-options.min))/options.step;
        lv_roller_set_selected(roller, range_size/2, LV_ANIM_OFF);
    }
    else
    {
        lv_roller_set_selected(roller, options.selection, LV_ANIM_OFF);
    }

    lv_obj_set_width(roller, LV_PCT(options.width_percentage));

    return roller;
}

}
