/**
 * @file IBaseScreen.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "ui/screens/IBaseScreen.hpp"

#include "services/UIService.hpp"
#include "ui/components/UIComponents.hpp"
#include "ui/components/StyleManager.hpp"
#include "ui/resources.h"
#include "utils/logger.hpp"

namespace UI
{
  
IBaseScreen::IBaseScreen(UIService* uiservice, lv_obj_t* parent) 
    :   m_root(lv_obj_create(parent)),
        m_uiservice{uiservice}
{
    // Ensure the root object covers the parent/screen area by default
    lv_obj_set_size(m_root, LV_PCT(100), LV_PCT(100));
}

IBaseScreen::~IBaseScreen() 
{
    if (m_root) 
    {
        lv_obj_remove_event_cb(m_root, nullptr);
        // Free the C resource
        lv_obj_delete(m_root); 
        // Invalidate object
        m_root = nullptr;
    }
}

lv_obj_t* IBaseScreen::createHeader(int32_t width, int32_t height) noexcept
{
    lv_obj_t* header_container = UIComponents::create_container(m_root, {width, height});

    return header_container;
}

void IBaseScreen::setAction(lv_obj_t* obj, UIAction action)
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

UIAction IBaseScreen::getAction(lv_obj_t* obj)
{
    return static_cast<UIAction>(reinterpret_cast<uintptr_t>(lv_obj_get_user_data(obj)));
}

}// namespace UI
