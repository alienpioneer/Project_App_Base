/**
 * @file StyleManager.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "ui/components/StyleManager.hpp"

namespace UI
{

// Buttons styles
lv_style_t StyleManager::style_round_button;
lv_style_t StyleManager::style_square_button;
lv_style_t StyleManager::style_square_button_checked;
lv_style_t StyleManager::style_main_button;
lv_style_t StyleManager::style_main_button_checked;
lv_style_t StyleManager::style_random_button;
lv_style_t StyleManager::style_random_button_checked;

lv_style_t StyleManager::style_start_button_checked;

// Background styles
lv_style_t StyleManager::style_bkg_gradient;

// Cards and containers styles
lv_style_t StyleManager::style_card;
lv_style_t StyleManager::style_containers;
lv_style_t StyleManager::style_main_swiper_card;
lv_style_t StyleManager::style_main_swiper_container;

// Keyboards
lv_style_t StyleManager::style_pswd_keyboard;
lv_style_t StyleManager::style_pswd_overlay;

// Containers
lv_style_t StyleManager::style_label_container;

// Popups
lv_style_t StyleManager::style_popup;

// COLORS
lv_grad_dsc_t StyleManager::bkg_gradient;

// THEMES
ThemeMode StyleManager::m_currentTheme = ThemeMode::Default;
Core::AMap<ThemeMode, ThemePalette, MAX_THEMES> StyleManager::m_themes;


void StyleManager::init()
{
    static bool is_initialized = false;

    if (is_initialized)
    {
        return;
    }

    // Add here more palettes and associate them with a theme
    m_themes.insert({ThemeMode::Default, DefaultPalette});

    // Buttons
    init_round_buttons_styles();
    init_main_buttons_styles();
    //Background
    init_background_gd_styles();
    // Cards and containers
    init_card_styles();
    // Swipers
    init_swiper_card_styles();
    // Keyboards
    init_keyboard_styles();
    // Containers
    init_containers_styles();
    //Popups
    init_popup_styles();

    setTheme(ThemeMode::Default);

    is_initialized = true;
}

// ============================================== INIT BUTTONS STYLES ===========================================================

void StyleManager::init_round_buttons_styles()
{
    // Round Button Style
    lv_style_init(&style_round_button);
    lv_style_init(&style_random_button);
    lv_style_init(&style_random_button_checked);
    lv_style_init(&style_square_button_checked);
    lv_style_init(&style_start_button_checked);

    // Set colors
    lv_style_set_bg_opa(&style_round_button, LV_OPA_COVER);
    lv_style_set_bg_color(&style_round_button, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_text_color(&style_round_button, lv_color_white());
    lv_style_set_text_font(&style_round_button, &lv_font_montserrat_24);

    // Add button gradient
    lv_style_set_bg_grad_color(&style_round_button, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_dir(&style_round_button, LV_GRAD_DIR_VER);

    // Add Button shadow
    lv_style_set_shadow_width(&style_round_button, 16);
    lv_style_set_shadow_color(&style_round_button, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_offset_y(&style_round_button, 6);

    // Random buton styles
    lv_style_set_bg_opa(&style_random_button, LV_OPA_COVER);
    lv_style_set_bg_color(&style_random_button, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_color(&style_random_button, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_dir(&style_random_button, LV_GRAD_DIR_VER);

    lv_style_set_shadow_width(&style_random_button, 16);
    lv_style_set_shadow_color(&style_random_button, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_offset_y(&style_random_button, 6);

    // Checked buttons
    lv_style_set_bg_color(&style_random_button_checked, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_color(&style_square_button_checked, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_color(&style_start_button_checked, lv_palette_main(LV_PALETTE_GREY));

    lv_style_set_outline_width(&style_start_button_checked, 2);
    lv_style_set_outline_color(&style_start_button_checked, lv_color_hex(0xFF0000));
}

void StyleManager::init_main_buttons_styles()
{
    // Round Button Style
    lv_style_init(&style_main_button);
    lv_style_init(&style_main_button_checked);
    lv_style_init(&style_square_button);

    // Set colors
    lv_style_set_bg_opa(&style_main_button, LV_OPA_COVER);
    lv_style_set_bg_color(&style_main_button, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_opa(&style_square_button, LV_OPA_COVER);
    lv_style_set_bg_color(&style_square_button, lv_palette_main(LV_PALETTE_BLUE));

    // Gradient
    lv_style_set_bg_grad_color(&style_main_button, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_dir(&style_main_button, LV_GRAD_DIR_VER);
    lv_style_set_bg_grad_color(&style_square_button, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_dir(&style_square_button, LV_GRAD_DIR_VER);

    // Outline
    lv_style_set_outline_width(&style_main_button, 2);
    lv_style_set_outline_color(&style_main_button, lv_color_hex(0xFF0000));
    // lv_style_set_outline_pad(&style_main_button, 4);
    lv_style_set_outline_width(&style_square_button, 2);
    lv_style_set_outline_color(&style_square_button, lv_color_hex(0xFF0000));

    // Shadow
    lv_style_set_shadow_width(&style_main_button, 16);
    lv_style_set_shadow_color(&style_main_button, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_offset_y(&style_main_button, 6);

    lv_style_set_shadow_width(&style_square_button, 12);
    lv_style_set_shadow_color(&style_square_button, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_offset_y(&style_square_button, 4);

    // Text
    lv_style_set_text_font(&style_main_button, &microsport_bold_32);
    lv_style_set_text_color(&style_main_button, lv_color_white());
    lv_style_set_text_font(&style_square_button, &lv_font_montserrat_32);
    lv_style_set_text_color(&style_square_button, lv_color_white());

    // Checked
    lv_style_set_bg_color(&style_main_button_checked, lv_palette_main(LV_PALETTE_GREY));
}

// ============================================== INIT BACKGROUND STYLES =========================================================

void StyleManager::init_background_gd_styles()
{
    static constexpr lv_color_t bkg_grad_colors[2] = {  LV_COLOR_MAKE(0xD0, 0xD0, 0xD0), LV_COLOR_MAKE(0x62, 0x61, 0x6D)};
    // Background gradient Style
    lv_style_init(&style_bkg_gradient);
    lv_grad_init_stops(&bkg_gradient, bkg_grad_colors, NULL, NULL, 2);
    // Make a radial gradient with the center in the middle of the object, extending to the farthest corner
    lv_grad_radial_init(&bkg_gradient, LV_GRAD_CENTER, LV_GRAD_CENTER, LV_GRAD_RIGHT, LV_GRAD_BOTTOM, LV_GRAD_EXTEND_PAD);
    // Set gradient as background
    lv_style_set_bg_grad(&style_bkg_gradient, &bkg_gradient);
}

// ============================================== INIT CONTAINER STYLES ==========================================================

void  StyleManager::init_containers_styles()
{
    lv_style_init(&style_containers);

    lv_style_set_bg_opa(&style_containers, LV_OPA_TRANSP);

    lv_style_set_outline_width(&style_containers, 1);
    lv_style_set_outline_color(&style_containers, lv_color_hex(0x333333));

    lv_style_set_shadow_width(&style_containers, 6);
    lv_style_set_pad_all(&style_containers, 0);

    lv_style_init(&style_label_container);

    lv_style_set_bg_opa(&style_label_container, LV_OPA_COVER);
    lv_style_set_bg_color(&style_label_container, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_radius(&style_label_container, 10);
    
    lv_style_set_outline_width(&style_label_container, 1);
    lv_style_set_outline_color(&style_label_container, lv_color_hex(0x333333));
}

// ============================================== INIT CARDS STYLES ==============================================================

void StyleManager::init_swiper_card_styles()
{
    lv_style_init(&style_main_swiper_card);
    lv_style_init(&style_main_swiper_container);

    lv_style_set_radius(&style_main_swiper_card, 10);
    lv_style_set_radius(&style_main_swiper_container, 10);

    //Outline
    lv_style_set_outline_width(&style_main_swiper_container, 2);
    lv_style_set_outline_color(&style_main_swiper_container, lv_color_hex(0xFF0000));

    // Set colors
    lv_style_set_bg_opa(&style_main_swiper_card, LV_OPA_COVER);
    lv_style_set_bg_color(&style_main_swiper_card, lv_palette_main(LV_PALETTE_BLUE));

    // Text
    lv_style_set_text_font(&style_main_swiper_card, &lv_font_montserrat_32);
    lv_style_set_text_color(&style_main_swiper_card, lv_color_white());
}

void StyleManager::init_card_styles()
{
    // Base Card Style
    lv_style_init(&style_card);
    lv_style_set_bg_color(&style_card, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_radius(&style_card, 10);
    lv_style_set_border_width(&style_card, 0);
}

// ============================================== KEYBOARDS ======================================================================

void StyleManager::init_keyboard_styles()
{
    lv_style_init(&style_pswd_keyboard);
    lv_style_init(&style_pswd_overlay);

    lv_style_set_bg_color(&style_pswd_keyboard, lv_color_darken(lv_color_white(), 32));
    lv_style_set_bg_opa(&style_pswd_keyboard, LV_OPA_COVER);
    lv_style_set_text_color(&style_pswd_keyboard, lv_color_white());
    lv_style_set_radius(&style_pswd_keyboard, 10);

    lv_style_set_bg_color(&style_pswd_overlay, lv_color_darken(lv_color_white(), 32));
    lv_style_set_bg_opa(&style_pswd_overlay, LV_OPA_TRANSP);
}

void StyleManager::init_popup_styles()
{
    lv_style_init(&style_popup);

    lv_style_set_bg_color(&style_popup, lv_color_darken(lv_color_white(), 32));
    lv_style_set_bg_opa(&style_popup, LV_OPA_50);
    lv_style_set_radius(&style_popup, 10);

    lv_style_set_outline_width(&style_popup, 2);
    lv_style_set_outline_color(&style_popup, lv_color_hex(0xFF0000));
}

// ============================================== UPDATE STYLES ON THEME ========================================================

void StyleManager::update_theme_colors() 
{
    const auto& palette = get_current_palette();

    lv_style_set_bg_color(&style_square_button, palette.square_button_accent);
    lv_style_set_bg_grad_color(&style_square_button, palette.square_button_bkg);
    lv_style_set_text_color(&style_square_button, palette.text_on_accent);
    lv_style_set_outline_color(&style_square_button, palette.square_button_outline);


    // Containers
    lv_style_set_outline_color(&style_main_swiper_container, palette.menu_swiper_container_outline);
    lv_style_set_bg_color(&style_main_swiper_card, palette.menu_swiper_card_bkg);


    lv_style_set_outline_color(&style_label_container, palette.label_container_outline);
    lv_style_set_bg_color(&style_label_container, palette.label_container_bkg);

    // Keyboards
    lv_style_set_bg_color(&style_pswd_keyboard, palette.keyboard_bkg);
    lv_style_set_bg_color(&style_pswd_overlay, palette.keyboard_overlay_bkg);

    // Popups
    lv_style_set_bg_color(&style_popup, palette.popup_bkg);
    lv_style_set_bg_opa(&style_popup, palette.popup_opacity);
    lv_style_set_outline_color(&style_popup, palette.popup_outline);

    // Update Background Gradients
    static lv_color_t grad_colors[2];
    grad_colors[0] = palette.bkg_color;
    grad_colors[1] = lv_color_black();
    lv_grad_init_stops(&bkg_gradient, grad_colors, NULL, NULL, 2);
}

// ============================================== THEMES ========================================================================

void StyleManager::setTheme(ThemeMode theme) 
{
    m_currentTheme = theme;
    update_theme_colors();
    
    // Notify LVGL to refresh all objects using these styles
    lv_obj_report_style_change(NULL);
}

const ThemePalette StyleManager::get_current_palette() 
{
    auto it = m_themes.find(m_currentTheme);

    if(it != m_themes.end())
    {
        return it->second;
    }

    return DefaultPalette;
}

}// namespace UI

