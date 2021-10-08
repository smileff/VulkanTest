
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include "GLFWWindow.h"

GLFWWindow::GLFWWindow(int winWidth, int winHeight, const char* caption)
{
	m_winWidth = winWidth;
	m_winHeight = winHeight;
	m_winCaption = caption;
}

void GLFWWindow::Run(VulkanApplication* vulkanApp)
{
	// Initialize window
	glfwInit();	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	// Don't need OpenGL API.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);		// Don't resize the window.

	m_window = glfwCreateWindow(m_winWidth, m_winHeight, m_winCaption.c_str(), nullptr, nullptr);	
	if (!m_window) {
		return;
	}

	// Initialize vulkan
	std::vector<const char*> requiredExtensions;
	std::vector<const char*> requiredLayers;

	// Query Vulkan extensions required by GLFW
	uint32_t glfwExtCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
	requiredExtensions.assign(glfwExtensions, glfwExtensions + glfwExtCount);
	
	vulkanApp->InitVulkan(requiredExtensions, requiredLayers);

	if (!vulkanApp->InitApplication()) {
		return;
	}

	if (m_window) 
	{
		while (!glfwWindowShouldClose(m_window)) 
		{
			glfwPollEvents();

			vulkanApp->RunFrame();
		}
	}

	vulkanApp->CleanupApplication();

	vulkanApp->CleanupVulkan();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}
