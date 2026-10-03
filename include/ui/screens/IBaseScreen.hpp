/**
 * @file IBaseScreen.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

extern "C"
{
#include <lvgl/lvgl.h>
}

#include <functional>

#include "core/AMap.hpp"
#include "core/Event.hpp"
#include "ui/UIStates.hpp"

namespace UI
{
    
class UIService;

class IBaseScreen 
{
public:
    IBaseScreen() = default;
    explicit IBaseScreen(UIService* uiservice, lv_obj_t* parent = nullptr);
    virtual ~IBaseScreen();

    virtual ScreenID getID() const = 0;
    virtual void closeAllPopups() noexcept {};
    [[nodiscard]] virtual lv_obj_t* createHeader(int32_t width, int32_t height) noexcept;

    // Get current screen object
    inline lv_obj_t* getScreen() { return m_root; }

    // Set an action to the LVGL object
    static void setAction(lv_obj_t* obj, UIAction action);
    // get action from the LVGL object
    static UIAction getAction(lv_obj_t* obj);

protected:
    lv_obj_t* m_root = nullptr;
    UIService* m_uiservice = nullptr;
};

}// namespace UI
