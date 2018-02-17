// internal
#include "common.hpp"
//#include "startworld.hpp"
#include "world.hpp"

#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

using Clock = std::chrono::high_resolution_clock;

// Global
//Startworld startworld;
World world;
const int width = 1280;
const int height = 720;
const char* title = "Your Title Here";

// Entry point
int main(int argc, char* argv[])
{

    //while startworld is destroyed
//    if (!startworld.init({ (float)width, (float)height }))
//    {
//        // Time to read the error message
//        std::cout << "Press any key to exit" << std::endl;
//        std::cin.get();
//        return EXIT_FAILURE;
//    }
//    
//    // start page
//    while (!startworld.buttonclicked())
//    {
//        // Processes system messages, if this wasn't present the window would become unresponsive
//        glfwPollEvents();
//        startworld.draw();
//    }
//    
//    // GAME DOESNT START UNTIL THE BUTTON IS CLICKED
//    //fprintf(stderr, "start screen should be destroyed");
//    startworld.destroy();
    
	
	// Initializing world (after renderer.init().. sorry)
	if (!world.init({ (float)width, (float)height }))
	{
		// Time to read the error message
		std::cout << "Press any key to exit" << std::endl;
		std::cin.get();
		return EXIT_FAILURE;
	}

	auto t = Clock::now();

	// variable timestep loop.. can be improved (:
	while (!world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		world.update(elapsed_sec);
        //if (!world.game_started)
          //  world.startworld_draw();
        //else
            world.draw();
	}

	world.destroy();

	return EXIT_SUCCESS;
}