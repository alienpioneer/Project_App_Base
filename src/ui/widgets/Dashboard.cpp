/**
 * @file Dashboard.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "ui/widgets/Dashboard.hpp"
#include "ui/components/StyleManager.hpp"
#include "utils/logger.hpp"

namespace UI
{

Dashboard::Dashboard()
{
    
}

Dashboard::~Dashboard()
{

}

void Dashboard::Initialize(lv_obj_t* parent)
    {
        if (parent == nullptr)
        {
            return;
        }

        m_parent = parent;

        lv_obj_set_layout(m_parent, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(m_parent, LV_FLEX_FLOW_COLUMN);
        
        lv_obj_set_style_pad_all(m_parent, 0, 0); 
        lv_obj_set_style_pad_row(m_parent, 0, 0);


        CreateTopBar();
        CreateMiddleArea();
        CreateBottomBar();

        // TODO Get style manager
    }

void Dashboard::CreateTopBar()
{
    if (m_parent == nullptr)
    {
        return;
    }

    m_top_bar = lv_obj_create(m_parent);
    lv_obj_set_size(m_top_bar, LV_PCT(100), 60);
    lv_obj_set_style_bg_color(m_top_bar, lv_color_hex(0x505050), 0);
    
    lv_obj_set_style_border_width(m_top_bar, 0, 0);
    lv_obj_set_style_radius(m_top_bar, 0, 0);
}

void Dashboard::CreateMiddleArea()
{
    if (m_parent == nullptr)
    {
        return;
    }

    m_middle_area = lv_obj_create(m_parent);
    lv_obj_set_width(m_middle_area, LV_PCT(100));
    lv_obj_set_scrollable(m_middle_area, false);
    
    lv_obj_set_flex_grow(m_middle_area, 1);
    
    lv_obj_set_layout(m_middle_area, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(m_middle_area, LV_FLEX_FLOW_ROW);

    // lv_obj_set_style_pad_all(m_middle_area, 0, 0);
    // lv_obj_set_style_margin_all(m_middle_area, 0, 0);
    // lv_obj_set_style_pad_gap(m_middle_area, 10, 0);
    lv_obj_set_style_pad_row(m_middle_area, 10, 0);
    
    lv_obj_set_style_bg_color(m_middle_area, lv_color_hex(0x1E1E1E), 0);
    lv_obj_set_style_border_width(m_middle_area, 0, 0);
    lv_obj_set_style_radius(m_middle_area, 0, 0);
    

    lv_obj_t* artificial_horizon_container = lv_obj_create(m_middle_area);
    lv_obj_set_size(artificial_horizon_container, LV_PCT(49), LV_PCT(99));
    lv_obj_set_style_border_width(artificial_horizon_container, 0, 0);
    // lv_obj_set_style_radius(artificial_horizon_container, 0, 0);
    lv_obj_set_style_bg_opa(artificial_horizon_container, 0, 0);
    // lv_obj_remove_style_all(artificial_horizon_container);
    lv_obj_set_style_border_width(artificial_horizon_container, 1, LV_PART_MAIN);
    
    lv_obj_t* right_widget_container = lv_obj_create(m_middle_area);
    lv_obj_set_size(right_widget_container, LV_PCT(49), LV_PCT(99));
    lv_obj_set_style_border_width(right_widget_container, 0, 0);
    // lv_obj_set_style_radius(right_widget_container, 0, 0);
    lv_obj_set_style_bg_opa(right_widget_container, 0, 0);
    // lv_obj_remove_style_all(right_widget_container);
    lv_obj_set_style_border_width(right_widget_container, 1, LV_PART_MAIN);
}

void Dashboard::CreateBottomBar()
{
    if (m_parent == nullptr)
    {
        return;
    }

    m_bottom_bar = lv_obj_create(m_parent);
    lv_obj_set_size(m_bottom_bar, LV_PCT(100), 80);

    lv_obj_set_scrollable(m_bottom_bar, false);

    lv_obj_set_style_bg_color(m_bottom_bar, lv_color_hex(0x303030), 0);
    lv_obj_set_style_radius(m_bottom_bar, 0, 0);
    
    lv_obj_set_style_border_side(m_bottom_bar, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_border_width(m_bottom_bar, 1, 0);
    lv_obj_set_style_border_color(m_bottom_bar, lv_color_hex(0xA0A0A0), 0);

    lv_obj_set_layout(m_bottom_bar, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(m_bottom_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(m_bottom_bar, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (size_t i = 0; i < m_buttons.size(); ++i)
    {
        m_buttons[i] = lv_btn_create(m_bottom_bar);
        lv_obj_set_size(m_buttons[i], 120, 50);
    }
}

} // namespace UI
