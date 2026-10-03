/**
 * @file MainScreen.hpp
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
 * 
 */

#pragma once

#include "ui/screens/IBaseScreen.hpp"
#include "ui/widgets/Dashboard.hpp"

namespace UI
{

class MainScreen : public IBaseScreen 
{
public:
    explicit MainScreen(UIService* uiservice, lv_obj_t* parent=NULL);
    ~MainScreen() = default;
    
    ScreenID getID() const override { return m_id; }

private:
    static constexpr ScreenID m_id = ScreenID::Main;
    Dashboard m_dashboard;

    //lv_obj_t* m_freeBtnScroll;
};

}// namespace UI
