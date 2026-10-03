/**
 * @file IBasePopup.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

extern "C"
{
#include <lvgl/lvgl.h>
}

#include "ui/UIStates.hpp"

namespace UI
{

class UIService;

class IBasePopup
{
public:
    IBasePopup() = default;
    explicit IBasePopup(UIService* uiservice);
    virtual ~IBasePopup();

    virtual ScreenID getID() const = 0;

    static void setAction(lv_obj_t* obj, UIAction action);
    static UIAction getAction(lv_obj_t* obj);

    void showPopup() noexcept;
    void hidePopup() noexcept;

protected:
    static void asyncHidePopup(void* user_data) noexcept;
    static void asyncShowPopup(void* user_data) noexcept;

protected:
    UIService* m_uiservice = nullptr;
    lv_obj_t* m_popup = nullptr;
};

}// namespace UI