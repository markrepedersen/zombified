
#pragma once

// internal
#include "common.hpp"
#include "button.hpp"
// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

// startpage
class Startworld
{
public:
    Startworld();
    ~Startworld();
    
    // Creates a window, sets up events and begins the game
    bool init(vec2 screen);
    
    // Releases all associated resources
    void destroy();
    
    // Steps the game ahead by ms milliseconds
   // bool update(float ms);
    
    // Renders our scene
    void draw();
    
    bool buttonclicked();
    
private:
    // !!! INPUT CALLBACK FUNCTIONS
    void on_key(GLFWwindow*, int key, int, int action, int mod);
    void on_mouse_move(GLFWwindow* window, int button, int action, int mod);
    
private:
    // Window hjandle
    GLFWwindow* m_window;
    
    // Game entities
    Button m_button;
    
};
