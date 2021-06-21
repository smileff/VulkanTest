
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include "HelloTriangleApplication.h"


// The application.

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, VkDebugUtilsMessageTypeFlagsEXT msgTypes, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
	if (msgTypes >= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
		std::cerr << "Callback message: " << pCallbackData->pMessage << std::endl;
	}

	return VK_FALSE;    // Return VK_TRUE will terminate the application.
}

void HelloTriangleApplication::run()
{
	initWindow();
	initVulkan();

	mainLoop();


	cleanUpVulkan();
	cleanUpWindow();	
}

void HelloTriangleApplication::initWindow()
{
	glfwInit();

	// Config.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	// Don't need OpenGL API.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);		// Don't resize the window.

	m_window = glfwCreateWindow(800, 600, "VulkanTest", nullptr, nullptr);
}

void HelloTriangleApplication::cleanUpWindow()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void HelloTriangleApplication::initVulkan()
{
	// Required extension and layers.
	std::vector<const char*> requiredExtensions;
	std::vector<const char*> requiredLayers;

	// Query Vulkan extensions required by GLFW
	uint32_t glfwExtCount = 0;
	const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);	
	requiredExtensions.assign(glfwExtensions, glfwExtensions + glfwExtCount);	
	
#if ENABLE_VULKAN_VALIDATION_LAYERS
	// Also require debug extension.
	requiredExtensions.push_back("VK_EXT_debug_utils");
	requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	if (!createVulkanInstance(requiredExtensions, requiredLayers, &m_vkInstance)) {
		throw std::runtime_error("Failed to create vulkan instance.");
	}
	
#if ENABLE_DEBUG_MESSENGER
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = makeVkDebugUtilsMessengerCreateInfoEXT();
	if (createDebugUtilsMessenger(m_vkInstance, &debugMessengerCreateInfo, nullptr, &m_vkDebugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create vulkan debug messenger");
	}
#endif
}

void HelloTriangleApplication::cleanUpVulkan()
{
#if ENABLE_DEBUG_MESSENGER
	DestroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugMessenger, nullptr);
#endif

	vkDestroyInstance(m_vkInstance, nullptr);
}

void HelloTriangleApplication::mainLoop()
{
	if (m_window) {
		while (!glfwWindowShouldClose(m_window)) {
			glfwPollEvents();
		}
	}
}



bool HelloTriangleApplication::createVulkanInstance(	
	const std::vector<const char*> &requiredExtensions,
	const std::vector<const char*> &requiredLayers,
	VkInstance *vkInst)
{
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Simu";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Validate required extension.
	if (!isRequiredExtensionSupported((uint32_t)requiredExtensions.size(), requiredExtensions.data()))
	{
		std::cerr << "Some extensions are not supported." << std::endl;
		return false;
	}

	// Validate required layers	
	if (!isRequiredLayerSupported((uint32_t)requiredLayers.size(), requiredLayers.data())) {
		std::cerr << "Some layer are not supported." << std::endl;
		return false;
	}

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = (uint32_t)requiredLayers.size();
	createInfo.ppEnabledLayerNames = requiredLayers.data();
	createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	
	if (vkCreateInstance(&createInfo, nullptr, vkInst) != VK_SUCCESS) {
		std::cerr << "Failed to create Vulkan instance." << std::endl;
		return false;
	}

	return true;
}

bool HelloTriangleApplication::isRequiredExtensionSupported(uint32_t extCount, const char* const *extensions)
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

	//	std::cout << "Instance Extension Properties:" << std::endl;
	//	for (const auto &prop : instExtProps) {
	//		std::cout << "* " << prop.extensionName << std::endl;
	//	}
	//	std::cout << std::endl;

		// Check if all required extension are supported.	
	std::cout << "Required extension: " << std::endl;
	for (uint32_t i = 0; i < extCount; ++i) {
		const char *ext = extensions[i];

		auto iter = std::find_if(instExtProps.begin(), instExtProps.end(), [ext](const auto &x) {
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

bool HelloTriangleApplication::isRequiredLayerSupported(uint32_t layerCount, const char* const *layers)
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

	//	std::cout << "Instance Layer Properties:" << std::endl;
	//	for (const auto &prop : instLayerProps) {
	//		std::cout << "* " << prop.layerName << ": " << prop.description << std::endl;
	//	}
	//	std::cout << std::endl;

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

#if ENABLE_DEBUG_MESSENGER
VkDebugUtilsMessengerCreateInfoEXT HelloTriangleApplication::makeVkDebugUtilsMessengerCreateInfoEXT()
{
	VkDebugUtilsMessengerCreateInfoEXT debugMsgCreateInfo;
	debugMsgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugMsgCreateInfo.pNext = nullptr;
	debugMsgCreateInfo.flags = 0;
	debugMsgCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	debugMsgCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugMsgCreateInfo.pfnUserCallback = debugCallback;
	debugMsgCreateInfo.pUserData = nullptr;

	return debugMsgCreateInfo;	
}


VkResult HelloTriangleApplication::createDebugUtilsMessenger(VkInstance vkInst, const VkDebugUtilsMessengerCreateInfoEXT *createInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *vkMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInst, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(vkInst, createInfo, pAllocator, vkMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void HelloTriangleApplication::DestroyDebugUtilsMessengerEXT(VkInstance vkInst, VkDebugUtilsMessengerEXT vkMessenger, VkAllocationCallbacks *pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInst, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(vkInst, vkMessenger, pAllocator);
	}
}

#endif
