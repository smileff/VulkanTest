// VulkanTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "VulkanApplication.h"
#include "GLFWWindow.h"

// The application.

int main()
{    
	GLFWWindow win;
	VulkanApplication app;

	try {
		win.Run(&app);
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
