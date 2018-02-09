// Header
#include "startworld.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <math.h>

// Same as static in c, local to compilation unit
namespace
{
    namespace
    {
        void glfw_err_cb(int error, const char* desc)
        {
            fprintf(stderr, "%d: %s", error, desc);
        }
    }
}

Startworld::Startworld()
{

}

Startworld::~Startworld()
{
    
}

// Startworld initialization
bool Startworld::init(vec2 screen)
{
    //-------------------------------------------------------------------------
    // GLFW / OGL Initialization
    // Core Opengl 3.
    glfwSetErrorCallback(glfw_err_cb);
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW");
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, 0);
    m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "Zombified", nullptr, nullptr);
    if (m_window == nullptr)
        return false;
    
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync
    
    // Load OpenGL function pointers
    gl3w_init();
    
    // Setting callbacks to member functions (that's why the redirect is needed)
    // Input is handled using GLFW, for more info see
    // http://www.glfw.org/docs/latest/input_guide.html
    glfwSetWindowUserPointer(m_window, this);
    //auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((Startworld*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
    auto mouse_button_callback = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((Startworld*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1, _2); };
    //glfwSetKeyCallback(m_window, key_redirect);
    //glfwSetCursorPosCallback(m_window, cursor_pos_redirect);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);
    
    // TODO: load music

    return m_button.init() ;
}

// Releases all the associated resources
void Startworld::destroy()
{
    m_button.destroy();
    glfwDestroyWindow(m_window);
    
}

//// Update our game Startworld
//bool Startworld::update(float elapsed_ms)
//{
//    int w, h;
//    glfwGetFramebufferSize(m_window, &w, &h);
//    
//    if(m_button.is_clicked()){
//        float left = 0.f;// *-0.5;
//        float top = 0.f;// (float)h * -0.5;
//        float right = (float)w;// *0.5;
//        float bottom = (float)h;// *0.5;
//        
//        float sx = 2.f / (right - left);
//        float sy = 2.f / (top - bottom);
//        float tx = -(right + left) / (right - left);
//        float ty = -(top + bottom) / (top - bottom);
//        mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };
//        
//        m_button.draw(projection_2D);
//    }
//    
//    return true;
//}

// Render our game Startworld
void Startworld::draw()
{
    // Clearing error buffer
    gl_flush_errors();
    
    // Getting size of window
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    
    
    // Updating window title with points
    //std::stringstream title_ss;
    //title_ss << "Points: " << m_points;
    //glfwSetWindowTitle(m_window, title_ss.str().c_str());
    
    // Clearing backbuffer
    glViewport(0, 0, w, h);
    glDepthRange(0.00001, 10);
    const float clear_color[3] = { 0.3f, 0.3f, 0.8f };
    glClearColor(clear_color[1], clear_color[1], clear_color[1], 1.0);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Fake projection matrix, scales with respect to window coordinates
    // PS: 1.f / w in [1][1] is correct.. do you know why ? (:
    float left = 0.f;// *-0.5;
    float top = 0.f;// (float)h * -0.5;
    float right = (float)w;// *0.5;
    float bottom = (float)h;// *0.5;
    
    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };
    
    m_button.draw(projection_2D);
    
    // Presenting
    glfwSwapBuffers(m_window);
}

void Startworld::on_mouse_move(GLFWwindow* window, int button, int action, int mod)
{
    // TODO: check that the part clicked is the button
    //      will do when figure out how things are scaled properly ***
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1)
    {
        m_button.clickicon();
    }
    if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_1)
    {
        m_button.click();
    }
}

bool Startworld::buttonclicked()
{
    if(m_button.is_clicked()) // true if the button has been clicked
    {
        glfwWindowShouldClose(m_window);
    }
    return m_button.is_clicked();
}
