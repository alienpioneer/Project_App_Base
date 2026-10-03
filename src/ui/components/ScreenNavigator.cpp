/**
 * @file ScreenNavigator.cpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#include "ui/components/ScreenNavigator.hpp"

#include "services/UIService.hpp"
#include "ui/screens/IBaseScreen.hpp"
#include "utils/logger.hpp"

namespace UI
{

ScreenNavigator::ScreenNavigator(ScreenArray& screens)
    : m_screens(screens)
{
}

void ScreenNavigator::loadScreen(ScreenID id, lv_screen_load_anim_t anim) 
{
    lv_obj_t* screen_root = getScreenPtr(id);

    if (!screen_root) 
    {
        LV_LOG_WARN("Attempted to load invalid ScreenID: %d", static_cast<int>(id));
        return; 
    }

    if(anim != LV_SCREEN_LOAD_ANIM_NONE)
    {
        lv_screen_load_anim(screen_root, anim, 300, 0, false);
    }
    else
    {
        lv_screen_load(screen_root);
    }
}

void ScreenNavigator::switchTo(ScreenID id) 
{
    // LOG_DEBUG("ScreenNavigator::switchTo() Switching to:"+std::to_string(static_cast<int>(id)));

    if (id == m_currentScreenId)
    {
        return;
    } 

    if( m_screens[static_cast<std::size_t>(m_currentScreenId)] )
    {
        m_screens[static_cast<std::size_t>(m_currentScreenId)]->closeAllPopups();
    }

    bool isBack = (id == ScreenID::Back);

    // Determine animation: Forward (Move Left) vs Backward (Move Right)
    lv_screen_load_anim_t anim = isBack ? 
        LV_SCREEN_LOAD_ANIM_MOVE_LEFT : 
        LV_SCREEN_LOAD_ANIM_MOVE_RIGHT;

    // If moving forward, save the current screen to history
    if (isBack)
    {
        if(!m_navigationStack.empty())
        {
            m_currentScreenId = m_navigationStack.back();
            m_navigationStack.pop_back();
        }
    }
    else
    {
        // Use with AVector - Doesn't work with std::vector !
        if (m_navigationStack.size() >= m_navigationStack.capacity()) 
        {
            m_navigationStack.erase(m_navigationStack.begin()); 
        }

        // Home button resets the stack
        if (id == ScreenID::Main)
        {
            anim = LV_SCREEN_LOAD_ANIM_MOVE_LEFT;
            m_navigationStack.clear();
        }
        else
        {
            m_navigationStack.push_back(m_currentScreenId);
        }

        m_currentScreenId = id;
    }

    // LV_LOG_INFO("m_currentScreenId: %d", static_cast<int>(m_currentScreenId));
    // LOG_DEBUG("ScreenNavigator::switchTo() m_currentScreenId: "+std::to_string(static_cast<int>(m_currentScreenId)));
    
    loadScreen(m_currentScreenId, anim);
}

lv_obj_t* ScreenNavigator::getScreenPtr(ScreenID id)
{
    if (id >= ScreenID::COUNT) return nullptr;

    lv_obj_t* screen = m_screens[static_cast<std::size_t>(id)]->getScreen();
    return screen ? screen : nullptr;
}

}// namespace UI