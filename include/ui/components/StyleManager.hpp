/**
 * @file StyleManager.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

extern "C"
{
#include <lvgl/lvgl.h>
#include "ui/resources.h"
}

#include "Palette.hpp"
#include "core/AMap.hpp"

/*
Apply a general style first, then a "modifier" style.
LVGL processes them in the order they are added; the last style applied wins for conflicting properties.

Use lv_obj_set_style_* functions for local properties overrides.
These act as "inline styles" (highest priority) and override any properties set in the attached lv_style_t objects.
*/

namespace UI
{

enum class ThemeMode 
{
    Default,
    Light,
    Dark,
    
    COUNT // Add themes above, do not remove
};

static_assert(static_cast<size_t>(ThemeMode::COUNT)>0);
constexpr size_t MAX_THEMES = static_cast<size_t>(ThemeMode::COUNT);

class StyleManager
{
public:
    // Buttons styles
    static lv_style_t style_round_button;
    static lv_style_t style_square_button;
    static lv_style_t style_square_button_checked;
    static lv_style_t style_main_button;
    static lv_style_t style_main_button_checked;
    static lv_style_t style_random_button;
    static lv_style_t style_random_button_checked;

    static lv_style_t style_start_button_checked;

    // Background styles
    static lv_style_t style_bkg_gradient;

    // Cards and containers styles
    static lv_style_t style_card;
    static lv_style_t style_containers;

    // Swipers
    static lv_style_t style_main_swiper_container;
    static lv_style_t style_main_swiper_card;

    // Keyboards
    static lv_style_t style_pswd_keyboard;
    static lv_style_t style_pswd_overlay;

    // Containers
    static lv_style_t style_label_container;

    // Popups
    static lv_style_t style_popup;

    // Gradient descriptors
    static lv_grad_dsc_t bkg_gradient;

    // Main initialization
    static void init();

    static void setTheme(ThemeMode theme);

    static const ThemePalette get_current_palette();
    
private:
    // Buttons
    static void init_round_buttons_styles();
    static void init_main_buttons_styles();
    //Background
    static void init_background_gd_styles();
    // Cards and containers
    static void init_card_styles();
    static void init_containers_styles();
    static void init_swiper_card_styles();
    // Keyboards
    static void init_keyboard_styles();
    // Popups
    static void init_popup_styles();
    // Theme updates
    static void update_theme_colors();

private:
    static ThemeMode m_currentTheme;
    static Core::AMap<ThemeMode, ThemePalette, MAX_THEMES> m_themes;
};
    
}// namespace UI


