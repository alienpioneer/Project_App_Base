/**
 * @file Dashboard.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

extern "C"
{
#include <lvgl/lvgl.h>
}
#include <array>


namespace UI
{

class Dashboard
{
public:
    Dashboard();
    ~Dashboard();

    void Initialize(lv_obj_t* parent);

private:

    void CreateTopBar();
    void CreateMiddleArea();
    void CreateBottomBar();

private:
    lv_obj_t* m_parent{nullptr};
    lv_obj_t* m_top_bar{nullptr};
    lv_obj_t* m_middle_area{nullptr};
    lv_obj_t* m_bottom_bar{nullptr};
    std::array<lv_obj_t*, 3> m_buttons{};
};

} //namespace UI