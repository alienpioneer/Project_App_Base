/**
 * @file UIUtils.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "ui/components/UIUtils.hpp"

namespace UIUtils
{

lv_obj_t* get_child_by_class(lv_obj_t* container,const lv_obj_class_t* childClass)
{
    for (uint32_t i = 0; i < lv_obj_get_child_cnt(container); i++) 
    {
        lv_obj_t* child = lv_obj_get_child(container, i);

        if (lv_obj_check_type(child, childClass)) 
        {
            return child;
        }
    }

    return nullptr;
}

lv_obj_t* get_current_scroll_selection(lv_obj_t* parent_container, std::string_view scroll_name)
{
    lv_obj_t* scroll = lv_obj_get_child_by_name(parent_container, scroll_name.data());

    if(!scroll)
    {
        return nullptr;
    }

    lv_area_t scroll_coords;
    lv_obj_get_coords(scroll, &scroll_coords);

    // visible center in screen space
    lv_coord_t center = scroll_coords.x1 + lv_obj_get_width(scroll) / 2;

    lv_obj_t* selected = nullptr;
    lv_coord_t best_dist = LV_COORD_MAX;


    uint32_t count = lv_obj_get_child_cnt(scroll);

    if(count == 0)
    {
        return nullptr;
    }

    for(uint32_t i = 0; i < count; ++i)
    {
        lv_obj_t* child = lv_obj_get_child(scroll, i);

        lv_area_t coords;
        lv_obj_get_coords(child, &coords);

        lv_coord_t child_center = (coords.x1 + coords.x2) / 2;

        lv_coord_t dist = LV_ABS(child_center - center);

        if(dist < best_dist)
        {
            best_dist = dist;
            selected = child;
        }
    }

    return selected;
}

int32_t compute_horiz_dlc_guide_pos(lv_obj_t* bar, lv_obj_t* guide, int32_t next_pos)
{   
    int32_t result = 0;

    if (!guide || !bar)
    {
        return result;
    }

    lv_area_t bar_coords;
    lv_obj_get_coords(bar, &bar_coords);

    const int32_t guide_width = lv_obj_get_width(guide);
    const int32_t bar_width = lv_obj_get_width(bar);
    // int32_t radius = lv_obj_get_style_radius(bar, LV_PART_MAIN);
    int32_t outline_width = lv_obj_get_style_outline_width(bar, LV_PART_MAIN);
    const int32_t center = bar_coords.x1 + bar_width/2;

    const int32_t max_offset =  bar_width/2-guide_width/2-outline_width;

    if (next_pos <= (center-max_offset)) 
    {
        result = -max_offset;
    }
    else if (next_pos >= (center+max_offset)) 
    {
        result = max_offset;
    }
    else
    {
        result = next_pos-center;
    }

    return result;
}

int32_t compute_vertical_dlc_guide_pos(lv_obj_t* bar, lv_obj_t* guide, int32_t next_pos)
{
    int32_t result = 0;

    if (!guide || !bar)
    {
        return result;
    }

    lv_area_t bar_coords;
    lv_obj_get_coords(bar, &bar_coords);

    const int32_t guide_height = lv_obj_get_height(guide);
    const int32_t bar_height = lv_obj_get_height(bar);
    
    int32_t outline_width = lv_obj_get_style_outline_width(bar, LV_PART_MAIN);
    const int32_t center = bar_coords.y1 + bar_height/2;

    const int32_t max_offset =  bar_height/2-guide_height/2-outline_width;

    if (next_pos <= (center-max_offset)) 
    {

        result = -max_offset;
    }
    else if (next_pos >= (center+max_offset)) 
    {
        result = max_offset;
    }
    else
    {
        result = next_pos-center;
    }

    return result;
}

int32_t compute_horiz_dlc_indicator_max_pos(lv_obj_t* bar, lv_obj_t* indicator)
{
    if (!bar)
    {
        return 0;
    }

    lv_obj_update_layout(bar); 
    int32_t barWidth = lv_obj_get_width(bar);
    int32_t lineWidth = lv_obj_get_width(indicator);
    int32_t outline_width = lv_obj_get_style_outline_width(bar, LV_PART_MAIN);

    return (barWidth - lineWidth)/2-outline_width;
}

int32_t compute_vert_dlc_indicator_max_pos(lv_obj_t* bar, lv_obj_t* indicator)
{
    if (!bar)
    {
        return 0;
    }

    lv_obj_update_layout(bar); 
    int32_t barHeight= lv_obj_get_height(bar);
    int32_t lineHeight = lv_obj_get_height(indicator);
    int32_t outline_width = lv_obj_get_style_outline_width(bar, LV_PART_MAIN);

    return (barHeight - lineHeight)/2-outline_width;
}

int get_vertical_dlc_guide_next_pos(lv_obj_t* bar, lv_obj_t* barGuide, int32_t& currentGuidePos, int32_t increment)
{
    int result = 0;

    if (!bar || !barGuide)
    {
        return result;
    }

    lv_area_t bar_coords;
    lv_obj_get_coords(bar, &bar_coords);

    const int32_t guideHeight = lv_obj_get_height(barGuide);
    const int32_t barHeight = lv_obj_get_height(bar);
    const int32_t center = bar_coords.y1 + barHeight/2;

    if (increment < 0)
    {   
        if((center+currentGuidePos+increment-guideHeight/2) <= bar_coords.y1)
        {
            currentGuidePos = -barHeight/2+guideHeight/2;
            result = 1;
        }
        else
        {
            currentGuidePos = currentGuidePos+increment;
            result = -1;
        }
    }
    else if (increment > 0)
    {   
        if( (center+currentGuidePos+increment+guideHeight/2)>=bar_coords.y2 )
        {
            currentGuidePos = bar_coords.y2-center-guideHeight/2;
            result = -1;
        }
        else
        {
            currentGuidePos = currentGuidePos+increment;
            result = 1;
        }
    }

    return result;
}

int get_horizontal_dlc_guide_next_pos(lv_obj_t* bar, lv_obj_t* barGuide, int32_t& currentGuidePos, int32_t increment)
{
    int result = 0;

    if (!bar || !barGuide)
    {
        return result;
    }

    lv_area_t bar_coords;
    lv_obj_get_coords(bar, &bar_coords);
    const int32_t guideWidth = lv_obj_get_width(barGuide);
    const int32_t barWidth = lv_obj_get_width(bar);
    const int32_t center = bar_coords.x1 + barWidth/2;

    // std::cout << "bar_coords.x1 " << bar_coords.x1 << " bar_coords.x2 " << bar_coords.x2 << " center " << center << " guideWidth " << guideWidth << std::endl;

    if (increment < 0)
    {   
        // std::cout << "future pos " << center+currentGuidePos+increment-guideWidth/2 << " bar_coords.x1 " << bar_coords.x1 << std::endl;

        if((center+currentGuidePos+increment-guideWidth/2) <= bar_coords.x1)
        {
            currentGuidePos = -barWidth/2+guideWidth/2;
            result = 1;
        }
        else
        {
            currentGuidePos = currentGuidePos+increment;
            result = -1;
        }
    }
    else if (increment > 0)
    {   
        // std::cout << "future pos " << center+currentGuidePos+increment+guideWidth/2 << " bar_coords.x2 " << bar_coords.x2 << std::endl;

        if( (center+currentGuidePos+increment+guideWidth/2)>=bar_coords.x2 )
        {
            currentGuidePos = bar_coords.x2-guideWidth/2-center;
            result = -1;
        }
        else
        {
            currentGuidePos = currentGuidePos+increment;
            result = 1;
        }
    }

    // std::cout << "currentGuidePos " << currentGuidePos << " increment " << increment << std::endl;

    return result;
}

int32_t percentage_w(lv_obj_t* ref, int32_t percent)
{
    return (lv_obj_get_width(ref) * percent) / 100;
}

int32_t percentage_h(lv_obj_t* ref, int32_t percent)
{
    return (lv_obj_get_height(ref) * percent) / 100;
}

int32_t percentage(int32_t value, int32_t percent)
{
    return (value * percent) / 100;
}

void disable_widget_tree(lv_obj_t* group) 
{
    lv_obj_add_state(group, LV_STATE_DISABLED);
    
    uint32_t child_cnt = lv_obj_get_child_cnt(group);

    for (uint32_t i = 0; i < child_cnt; i++) 
    {
        disable_widget_tree(lv_obj_get_child(group, i));
    }
}

void enable_widget_tree(lv_obj_t* group)
{
    lv_obj_remove_state(group, LV_STATE_DISABLED);
    
    uint32_t child_cnt = lv_obj_get_child_cnt(group);

    for (uint32_t i = 0; i < child_cnt; i++) 
    {
        enable_widget_tree(lv_obj_get_child(group, i));
    }
}

}// namespace UI