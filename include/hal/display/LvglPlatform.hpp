/**
 * @file LvglPlatform.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

extern "C"
{
#include "lvgl/lvgl.h"
}

#include <cstdint>

#include "data/IDataReadUIService.hpp"

namespace UI
{

class LvglPlatform 
{

    const std::string DRM_DISPLAY_FILE = Data::IDataReadUIService::getDrmDispalyFile();
    const std::string DRM_INPUT_FILE = Data::IDataReadUIService::getDrmInputFile();
    const int INDEV_SCROLL_LIMIT = Data::IDataReadUIService::getIndevScrollLimit();
    const int GESTURE_MIN_VELOCITY = Data::IDataReadUIService::getGestureMinVelocity();
    const int GESTURE_MAX_DISTANCE = Data::IDataReadUIService::getGestureMinDistance();

public:
    LvglPlatform(int width, int height, uint8_t resolution_divider=1, bool showCursor=false, bool useCustomInput=false, bool mirrorX=false, bool mirrorY=false);
    ~LvglPlatform();

    // Delete copy/move to ensure single static lifetime
    LvglPlatform(const LvglPlatform&) = delete;
    LvglPlatform& operator=(const LvglPlatform&) = delete;

    // Manually advance steps - without calling run() - main advised method 
    static void step();

    static void evdev_read_cb(lv_indev_t *indev, lv_indev_data_t *data);

    // runs the main loop, not needed if step() is used !
    // cppcheck-suppress functionConst
    void run();
    
    void stop();
    void start();

    static uint32_t get_sleep_time();

private:

#ifdef USE_DRM
    void init_drm(const char* displayFile, const char* inputFile, bool showCursor, bool useCustomInput, bool mirrorX, bool mirrorY);
#else
    void init_sdl(int width, int height, uint8_t resolution_divider);
#endif

    lv_obj_t * create_cursor_representation();

    void getScreenAndInputInfo(lv_display_t* lvglDisplay, const char* inputFile);
    
    lv_display_t* display_{nullptr};
    lv_indev_t* input_{nullptr};
    lv_obj_t* cursor_{nullptr};

    struct EvdevState 
    {
        int evdev_fd = -1;
        int x = 0;
        int y = 0;
        bool pressed = false;
        uint32_t abs_x_max = 0;
        uint32_t abs_y_max = 0;
        int32_t hor_res=1;
        int32_t ver_res=1;
    };

    EvdevState evdev_state_;

    bool running_{true};
};

}// namespace UI
