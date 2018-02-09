#pragma once

#include "common.hpp"

//game screen toolbox manager
class ViewHelper
{
public:

    //method to initialize the viewManager
    static ViewHelper* getInstance(GLFWwindow* m_window);
    static float getRatio();


private:
    ViewHelper();
    static  ViewHelper* instance; //ratio of window size to screen resolution
    float m_ratio;
};

// ViewHelper* ViewHelper::instance = 0;