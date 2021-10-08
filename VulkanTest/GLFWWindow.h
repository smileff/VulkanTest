#ifndef _GLFW_APPLICATION_H_
#define _GLFW_APPLICATION_H_

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "VulkanApplication.h"

class GLFWWindow
{
public:
	GLFWWindow(int winWidth = 800, int winHeight = 600, const char* caption = "GLFW Window");

	void Run(VulkanApplication *vulkanApp);

private:
	int m_winWidth, m_winHeight;
	std::string m_winCaption;
	GLFWwindow *m_window = nullptr;
};


#endif