/**
 * @file LvglPlatform.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "hal/display/LvglPlatform.hpp"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#include <chrono>
#include <iostream>

#ifndef USE_DRM
#include <SDL2/SDL_render.h>
#endif

#include "utils/logger.hpp"

namespace UI
{

LvglPlatform::LvglPlatform(int width, int height, uint8_t resolution_divider, bool showCursor, bool useCustomInput, bool mirrorX, bool mirrorY)
{

#ifdef USE_DRM
    (void)width;
    (void)height;
    (void)resolution_divider;
#else
    (void)showCursor;
    (void)useCustomInput;
#endif

// Register custom logging
#if LV_USE_LOG
    lv_log_register_print_cb([](lv_log_level_t level, const char* buf) {

        switch(level) 
        {
            case LV_LOG_LEVEL_ERROR: LOG_ERROR(buf); break;
            case LV_LOG_LEVEL_WARN:  LOG_WARN(buf); break;
            case LV_LOG_LEVEL_INFO:  LOG_INFO(buf); break;
            default:                 LOG_TRACE(buf); break;
        }
        
    });
#endif

    lv_init();

    using Clock = std::chrono::steady_clock;

    lv_tick_set_cb([]() -> uint32_t 
        {
            static auto start = Clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
        }
    );

#ifdef USE_DRM
    init_drm(DRM_DISPLAY_FILE.c_str(), DRM_INPUT_FILE.c_str(), showCursor, useCustomInput, mirrorX, mirrorY);
#else
    init_sdl(width, height, resolution_divider);
#endif
}

// cppcheck-suppress functionConst
void LvglPlatform::run() 
{
    while (running_) 
    {
        lv_timer_periodic_handler();
    }
}

void LvglPlatform::stop() 
{ 
    running_ = false; 
}

void LvglPlatform::start()
{
    running_ = true; 
}

void LvglPlatform::step() 
{
    lv_timer_periodic_handler();
}

#ifdef USE_DRM
void LvglPlatform::init_drm(const char* displayFile, const char* inputFile, bool showCursor, bool useCustomInput, bool mirrorX, bool mirrorY) 
{
    // Create display driver
    display_ = lv_linux_drm_create();

    if (!display_ ) 
    {
        LV_LOG_ERROR("Failed to create DRM display");
    }

    // lv_display_set_render_mode(display_, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_color_format(display_, LV_COLOR_FORMAT_XRGB8888);
    lv_linux_drm_set_file(display_, displayFile, -1);
    lv_display_set_default(display_);

    getScreenAndInputInfo(display_, inputFile);

    if (useCustomInput)
    {
        // Register the custom input driver with LVGL
        input_ = lv_indev_create();

        if (!input_) 
        {
            LV_LOG_ERROR("Failed to create input device!");
        }

        lv_indev_set_type(input_, LV_INDEV_TYPE_POINTER);

        lv_indev_set_read_cb(input_, evdev_read_cb);
        
        lv_indev_set_user_data(input_, (void *)(intptr_t)&evdev_state_);
    }
    else
    {
        // Use default LVGL evdev driver
        input_ = lv_evdev_create(LV_INDEV_TYPE_POINTER, inputFile);

        if (evdev_state_.evdev_fd >= 0)
        {
            close(evdev_state_.evdev_fd);
        }
    }
    
    lv_indev_set_display(input_, display_);

    //lv_display_set_rotation(display_, LV_DISPLAY_ROTATION_90);

    if (mirrorX && !mirrorY)
    {
        lv_evdev_set_calibration(input_, evdev_state_.abs_x_max, 0, 0, evdev_state_.abs_y_max);
    }
    else if (!mirrorX && mirrorY)
    {
        lv_evdev_set_calibration(input_, 0, evdev_state_.abs_y_max, evdev_state_.abs_x_max, 0);
    }
    else if (mirrorX && mirrorY)
    {
        lv_evdev_set_calibration(input_, evdev_state_.abs_x_max, evdev_state_.abs_y_max, 0, 0);
    }
    else
    {
        lv_evdev_set_calibration(input_, 0, 0, evdev_state_.abs_x_max, evdev_state_.abs_y_max);
    }

    lv_indev_set_scroll_limit(input_, static_cast<uint8_t>(INDEV_SCROLL_LIMIT));
    lv_indev_set_gesture_min_velocity(input_, static_cast<uint8_t>(GESTURE_MIN_VELOCITY));
    lv_indev_set_gesture_min_distance(input_, static_cast<uint8_t>(GESTURE_MAX_DISTANCE));
    // lv_timer_set_period(lv_indev_get_read_timer(input_), 10);

    if(showCursor)
    {
        cursor_ = create_cursor_representation();
        lv_indev_set_cursor(input_, cursor_);
        lv_obj_move_foreground(cursor_);
    }
}
#else
void LvglPlatform::init_sdl(int width, int height, uint8_t resolution_divider) 
{
    float scale = 1.0f / resolution_divider;

    display_ = lv_sdl_window_create(width, height); 

    lv_sdl_window_set_zoom(display_, scale);

    SDL_Renderer *renderer = (SDL_Renderer *)lv_sdl_window_get_renderer(display_);

    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);

#ifndef NDEBUG
    std::cout << "Renderer: " << info.name << " \n";
    std::cout << "Flags: " << info.flags << " (accelerated= " << ((info.flags & SDL_RENDERER_ACCELERATED) != 0) << ") \n" << std::endl;
#endif

// Beware, the library MUST be build with sdlsw option to use the cpu!
#ifdef USE_SDL_DRAW
    SDL_RenderSetScale(renderer, scale, scale);
#endif

    // Create input after zooming
    input_ = lv_sdl_mouse_create();
    lv_display_set_default(display_);
}
#endif

// Query next task time
uint32_t LvglPlatform::get_sleep_time() 
{
    return lv_timer_handler_run_in_period(0); 
}

void LvglPlatform::getScreenAndInputInfo(lv_display_t* lvglDisplay, const char* inputFile)
{
    evdev_state_.evdev_fd = open(inputFile, O_RDONLY | O_NONBLOCK | O_NOFOLLOW | O_CLOEXEC);

    if (evdev_state_.evdev_fd < 0) 
    {
        perror("Cannot open mouse device");
    }
    
    // Detect raw input range
    struct input_absinfo abs_x_info, abs_y_info;
    ioctl(evdev_state_.evdev_fd, EVIOCGABS(ABS_X), &abs_x_info);
    ioctl(evdev_state_.evdev_fd, EVIOCGABS(ABS_Y), &abs_y_info);
    evdev_state_.abs_x_max = abs_x_info.maximum;
    evdev_state_.abs_y_max = abs_y_info.maximum;

    // Fetch native resolution BEFORE rotating the display
    evdev_state_.hor_res = lv_display_get_horizontal_resolution(lvglDisplay);
    evdev_state_.ver_res = lv_display_get_vertical_resolution(lvglDisplay);

#ifndef NDEBUG
    std::cout << "evdev_state_.abs_x_max: " << evdev_state_.abs_x_max << std::endl;
    std::cout << "evdev_state_.abs_y_max: " << evdev_state_.abs_y_max << std::endl;
    std::cout << "evdev_state_.hor_res: " << evdev_state_.hor_res << std::endl;
    std::cout << "evdev_state_.ver_res: " << evdev_state_.ver_res << std::endl;
#endif
}

LvglPlatform::~LvglPlatform() 
{
    LOG_DEBUG("LvglPlatform::~LvglPlatform()  LvglPlatform DESTROY thread");

    if (evdev_state_.evdev_fd >= 0) 
    {
        close(evdev_state_.evdev_fd);
        evdev_state_.evdev_fd = -1;
    }

    lv_deinit();
}

void LvglPlatform::evdev_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    auto* state = static_cast<EvdevState*>(lv_indev_get_user_data(indev));

    struct input_event in_event;
    bool moved = false;
    bool frame_ready = false;

    while (read(state->evdev_fd, &in_event, sizeof(in_event)) > 0) 
    {
        if (in_event.type == EV_ABS) // Virtual machine, touch
        {
            if (in_event.code == ABS_X) 
            {
                if (state->abs_x_max > 0)
                {
                    // Scale based on detected maximum
                    state->x = (in_event.value * state->hor_res) / state->abs_x_max;
                }
            }
            else if (in_event.code == ABS_Y) 
            {
                if (state->abs_y_max > 0)
                {
                    // Scale based on detected maximum
                    state->y = (in_event.value * state->ver_res) / state->abs_y_max;
                }
            }
            moved = true;
        } 
        else if (in_event.type == EV_REL) // Native Linux
        {
            if (in_event.code == REL_X)
            {
                 state->x += in_event.value;
            } 
            else if (in_event.code == REL_Y)
            {
                 state->y += in_event.value;
            } 
            moved = true;
        } 
        else if (in_event.type == EV_KEY) 
        {
            if (in_event.code == BTN_LEFT || in_event.code == BTN_TOUCH )
            {
                state->pressed = (in_event.value > 0);
            }
        }
        else if (in_event.type == EV_SYN && in_event.code == SYN_REPORT)
        {
            frame_ready = true;
        }
    }

    if (frame_ready)
    {
        if (state->x < 0) state->x = 0;
        if (state->y < 0) state->y = 0;
        if (state->x >= state->hor_res) state->x = state->hor_res - 1;
        if (state->y >= state->ver_res) state->y = state->ver_res - 1;

        data->point.x = state->x;
        data->point.y = state->y;
        
        data->state = state->pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;

        if (moved)
        {
            printf("Raw X: %d, Raw Y: %d\n", data->point.x, data->point.y);
            LV_LOG_USER("Mouse moved to : %d, %d\n", data->point.x, data->point.y);
        }

        if (state->pressed)
        {
            LV_LOG_USER("Mouse pressed: %d, %d\n", data->point.x, data->point.y);
        }
    }
}

lv_obj_t * LvglPlatform::create_cursor_representation()
{
    lv_obj_t *cursor = lv_obj_create(lv_layer_sys());
    
    // Create a simple circle cursor
    lv_obj_set_size(cursor, 5, 5);
    lv_obj_set_style_radius(cursor, LV_RADIUS_CIRCLE, 0);
    // lv_obj_set_style_border_color(cursor, lv_color_white(), 0);
    lv_obj_set_style_border_color(cursor, lv_color_hex(0x58337E), 0);
    lv_obj_set_style_border_width(cursor, 2, 0);

    return cursor;
}

}// namespace UI
