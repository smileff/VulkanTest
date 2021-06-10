// VulkanTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

class HelloTriangleApplication
{
public:
	void run();
private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanUp();

	GLFWwindow *m_win = nullptr;
};

void HelloTriangleApplication::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanUp();
}

void HelloTriangleApplication::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
	m_win = glfwCreateWindow(800, 600, "VulkanTest", nullptr, nullptr);
}

void HelloTriangleApplication::initVulkan()
{}

void HelloTriangleApplication::mainLoop()
{
	if (m_win) {
		while (!glfwWindowShouldClose(m_win)) {
			glfwPollEvents();
		}
	}
	
}

void HelloTriangleApplication::cleanUp()
{
	glfwDestroyWindow(m_win);
	glfwTerminate();
}


int main()
{
    //std::cout << "Hello World!\n"; 

	HelloTriangleApplication app;
	try {
		app.run();
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
