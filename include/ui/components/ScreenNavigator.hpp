/**
 * @file ScreenNavigator.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

extern "C" 
{
#include <lvgl/lvgl.h>
}

#include <array>
#include <memory>
#include "core/AVector.hpp"
#include "ui/UIStates.hpp"

namespace UI
{

class UIService;
class IBaseScreen;

constexpr std::size_t MAX_SCREEN_NAV_STACK = 100;

class ScreenNavigator
{
public:
    using ScreenArray = std::array<std::unique_ptr<IBaseScreen>, static_cast<std::size_t>(ScreenID::COUNT)>;

    explicit ScreenNavigator(ScreenArray& screens);

    void switchTo(ScreenID id);
    void loadScreen(ScreenID id, lv_screen_load_anim_t anim = LV_SCREEN_LOAD_ANIM_NONE);

    ScreenID currentScreen() const noexcept { return m_currentScreenId; }

    lv_obj_t* getScreenPtr(ScreenID id);

private:

    ScreenArray& m_screens;
    ScreenID m_currentScreenId{ScreenID::Main};
    Core::AVector<ScreenID, MAX_SCREEN_NAV_STACK> m_navigationStack;
};

} // namespace UI
