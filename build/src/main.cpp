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

const int width = 1280;
const int height = 720;

// Entry point
int main(int argc, char* argv[])
{
	World world;
	if (!world.init({ (float)width, (float)height }))
	{
		// Time to read the error message
//		std::cout << "Press any key to exit" << std::endl;
//		std::cin.get();
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
        world.draw();
	}
	
	world.destroy();

	
	return EXIT_SUCCESS;
}
