// VulkanTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

// The application.

class HelloTriangleApplication
{
public:
	void run();
private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanUp();

	bool isRequiredExtensionSupported(uint32_t extCount, const char **extensions);

	bool isRequiredLayerSupported(uint32_t layerCount, const char **layers);

	GLFWwindow *m_window = nullptr;
	VkInstance m_vkInstance;
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

	// Config.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	// Don't need OpenGL API.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);		// Don't resize the window.
	
	m_window = glfwCreateWindow(800, 600, "VulkanTest", nullptr, nullptr);
}

void HelloTriangleApplication::initVulkan()
{	
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Simu";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Query Vulkan extensions required by GLFW
	uint32_t glfwExtCount = 0;
	const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);

	if (!isRequiredExtensionSupported(glfwExtCount, glfwExtensions)) {
		throw std::runtime_error("Some extensions are not supported.");
	}

	std::vector<const char*> requiredLayers;

#if !NDEBUG
	requiredLayers.push_back("VK_LAYER_KHRONOS_validation");

	if (!isRequiredLayerSupported((uint32_t)requiredLayers.size(), requiredLayers.data())) {
		throw std::runtime_error("Some layer are not supported.");
	}
#endif

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = (uint32_t)requiredLayers.size();
	createInfo.ppEnabledLayerNames = requiredLayers.data();
	createInfo.enabledExtensionCount = glfwExtCount;
	createInfo.ppEnabledExtensionNames= glfwExtensions;	
	
	if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan instance.");
	}
}

void HelloTriangleApplication::mainLoop()
{
	if (m_window) {
		while (!glfwWindowShouldClose(m_window)) {
			glfwPollEvents();
		}
	}
	
}

void HelloTriangleApplication::cleanUp()
{
	vkDestroyInstance(m_vkInstance, nullptr);
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool HelloTriangleApplication::isRequiredExtensionSupported(uint32_t extCount, const char **extensions)
{
	// Obtain Vulkan extensions supported.
	uint32_t instExtPropCount = 0;
	if (vkEnumerateInstanceExtensionProperties(nullptr, &instExtPropCount, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("Failed to enumerate instance extension properties.");
	};

	std::vector<VkExtensionProperties> instExtProps(instExtPropCount);
	if (vkEnumerateInstanceExtensionProperties(nullptr, &instExtPropCount, instExtProps.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to enumerate instance extension properties.");
	}

	std::cout << "Instance Extension Properties:" << std::endl;
	for (const auto &prop : instExtProps) {
		std::cout << "* " << prop.extensionName << std::endl;
	}
	std::cout << std::endl;

	// Check if all required extension are supported.	
	std::cout << "Required extension: " << std::endl;
	for (uint32_t i = 0; i < extCount; ++i) {
		const char *ext = extensions[i];

		auto iter = std::find_if(instExtProps.begin(), instExtProps.end(), [ext](const auto &x){
			return strcmp(x.extensionName, ext) == 0;
		});
		if (iter != instExtProps.end()) {
			std::cout << "* " << ext << " is supported." << std::endl;
		} else {
			std::cout << "Required extension: " << ext << " is not supported." << std::endl;
			return false;
		}
	}
	std::cout << std::endl;

	return true;
}

bool HelloTriangleApplication::isRequiredLayerSupported(uint32_t layerCount, const char **layers)
{
	// Obtain Vulkan layers supported.
	uint32_t instLayerPropCount = 0;
	if (vkEnumerateInstanceLayerProperties(&instLayerPropCount, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("Failed to enumerate instance layer properties.");
	}

	std::vector<VkLayerProperties> instLayerProps(instLayerPropCount);
	if (vkEnumerateInstanceLayerProperties(&instLayerPropCount, instLayerProps.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to enumerate instance layer properties.");
	}

	std::cout << "Instance Layer Properties:" << std::endl;
	for (const auto &prop : instLayerProps) {
		std::cout << "* " << prop.layerName << ": " << prop.description << std::endl;
	}
	std::cout << std::endl;

	// Check if all required layer are supported.
	std::cout << "Required layer: " << std::endl;
	for (uint32_t i = 0; i < layerCount; ++i) {
		const char *layer = layers[i];

		auto iter = std::find_if(instLayerProps.begin(), instLayerProps.end(), [layer](const VkLayerProperties &x) {
			return strcmp(layer, x.layerName) == 0;
		});

		if (iter != instLayerProps.end()) {
			std::cout << "* " << layer << " is supported." << std::endl;
		} else {
			return false;
		}
	}
	std::cout << std::endl;

	return true;
}

// The driver function.

int main()
{    
	HelloTriangleApplication app;

	try {
		app.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}