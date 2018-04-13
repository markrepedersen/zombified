//Header
#include "viewHelper.hpp"

ViewHelper* ViewHelper::instance = 0;

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