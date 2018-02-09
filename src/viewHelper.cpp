//Header
#include "viewHelper.hpp"
#include <iostream>
#include <list>
#include <cmath>

ViewHelper* ViewHelper::getInstance(GLFWwindow* m_window)
 {
     if (instance == 0)
     {
        instance = new ViewHelper();
        
        int frameW, frameH;
        glfwGetFramebufferSize(m_window, &frameW, &frameH);

	    int windowW, windowH;
        glfwGetWindowSize(m_window, &windowW, &windowH);

        instance->m_ratio = (float)frameW/windowW;
     }
    return instance;
 }

ViewHelper::ViewHelper()
{}

 float ViewHelper::getRatio()
 {
    return instance->m_ratio;
 }