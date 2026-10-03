/**
 * @file Palette.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

extern "C"
{
#include <lvgl/lvgl.h>
}

namespace UI
{

struct ThemePalette
{
    lv_color_t bkg_color;
    // buttons
    lv_color_t square_button_bkg;
    lv_color_t square_button_accent;
    lv_color_t square_button_outline;
    // Swipers
    lv_color_t menu_swiper_card_bkg;
    lv_color_t menu_swiper_container_outline;
    // Text
    lv_color_t text_primary;
    lv_color_t text_on_accent;
    // keyboards
    lv_color_t keyboard_bkg;
    lv_color_t keyboard_keys;
    lv_color_t keyboard_overlay_bkg;
    lv_color_t keyboard_cancel;
    lv_color_t keyboard_ok;
    // containers
    lv_color_t label_container_bkg;
    lv_color_t label_container_outline;
    // popups
    lv_color_t popup_bkg;
    lv_opa_t   popup_opacity;
    lv_color_t popup_outline;
};

// PALETTES
inline static const ThemePalette DefaultPalette = 
{
    lv_color_hex(0x1E1D21),    // bkg_color
    // buttons
    lv_color_hex(0xA2A6B4),    // square_button_bkg
    lv_color_hex(0x12CBFD),    // square_button_accent
    lv_color_hex(0xD9D9D9),    // square_button_outline
    // Swipers
    lv_color_hex(0xE38744),     // menu_swiper_card_bkg;
    lv_color_hex(0x68D9D2),     // menu_swiper_co
    // Text
    lv_color_white(),           // text_primary
    lv_color_white(),           // text_on_accent
    // keyboards
    lv_color_hex(0xADADAD),     // keyboard_bkg
    lv_color_hex(0xD1E1FF),     // keyboard_keys
    lv_color_hex(0x555F61),     // keyboard_overlay_bkg
    lv_color_hex(0xD15C5C),     // keyboard_cancel
    lv_color_hex(0x5CD187),     // keyboard_ok
    // containers   
    lv_color_hex(0x999999),     // label_container_bkg;
    lv_color_hex(0x3BC1DB),     // label_container_outline;
    // popups
    lv_color_hex(0x757D85),     // popup_bkg;
    LV_OPA_70,                  // popup_opacity;
    lv_color_hex(0xF0F5F2)      // popup_outline;
};

}// namespace UI