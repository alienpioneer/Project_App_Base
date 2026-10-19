/**
 * @file IBasePopup.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "ui/screens/IBasePopup.hpp"

#include "services/UIService.hpp"
#include "ui/components/UIComponents.hpp"
#include "ui/components/StyleManager.hpp"
#include "ui/resources.h"
#include "utils/logger.hpp"

namespace UI
{

IBasePopup::IBasePopup(UIService* uiservice) 
    :   m_uiservice{uiservice}
{
}

IBasePopup::~IBasePopup() 
{
    if (m_popup) 
    {
        lv_obj_remove_event_cb(m_popup, nullptr);
        // Free the C resource
        lv_obj_delete(m_popup); 
        // Invalidate object
        m_popup = nullptr;
    }
}

void IBasePopup::setAction(lv_obj_t* obj, UIAction action)
{
    if (action != UI::UIAction::COUNT) 
    {
        lv_obj_set_user_data(obj, reinterpret_cast<void*>(static_cast<uintptr_t>(action)));
    }
    else
    {
        LOG_ERROR("IBaseScreen::setAction() Illegal use of UI::UIAction::COUNT");
    }
}

UIAction IBasePopup::getAction(lv_obj_t* obj)
{
    return static_cast<UIAction>(reinterpret_cast<uintptr_t>(lv_obj_get_user_data(obj)));
}

void IBasePopup::showPopup() noexcept
{
    lv_async_call(asyncShowPopup, this);
}

void IBasePopup::hidePopup() noexcept
{
    lv_async_call(asyncHidePopup, this);
}

void IBasePopup::asyncHidePopup(void* user_data) noexcept
{
    auto* cur_instance = static_cast<UI::IBasePopup*>(user_data);

    if (!cur_instance || !cur_instance->m_popup)
    {
        return;
    } 

    lv_obj_set_hidden(cur_instance->m_popup,true);
}

void IBasePopup::asyncShowPopup(void* user_data) noexcept
{
    auto* cur_instance = static_cast<UI::IBasePopup*>(user_data);

    if (!cur_instance || !cur_instance->m_popup)
    {
        return;
    } 

    lv_obj_set_hidden(cur_instance->m_popup,false);
}

}// namespace UI