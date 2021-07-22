// VulkanTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <functional>

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

// The application.

#define VkCall(func) if ((func) != VK_SUCCESS) throw std::runtime_error(std::string("failed to call " #func ", file: ") + std::string(__FILE__) + std::string(", line: ") + std::to_string(__LINE__));

#define ENABLE_DEBUG_CALLBACK 0

#if ENABLE_DEBUG_CALLBACK
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, VkDebugUtilsMessageTypeFlagsEXT msgTypes, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    if (msgTypes >= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        std::cerr << "Callback message: " << pCallbackData->pMessage << std::endl;
    }
    
    return VK_FALSE;    // Return VK_TRUE will terminate the application.
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance vkInst, const VkDebugUtilsMessengerCreateInfoEXT *createInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *vkMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInst, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(vkInst, createInfo, pAllocator, vkMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void DestroyDebugUtilsMessengerEXT(VkInstance vkInst, VkDebugUtilsMessengerEXT vkMessenger, VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInst, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(vkInst, vkMessenger, pAllocator);
    }
}
#endif

class HelloTriangleApplication
{
public:
	void run();
private:
	void initGLFWWindow();
	void initVulkan();
    void pickPhysicalDeivce(std::function<bool(VkPhysicalDeviceProperties, VkPhysicalDeviceFeatures)> isSuitableFunc);
	void mainLoop();
	void cleanUp();

	bool isRequiredExtensionSupported(uint32_t extCount, const char **extensions);

	bool isRequiredLayerSupported(uint32_t layerCount, const char **layers);

	GLFWwindow *m_window = nullptr;
	VkInstance m_vkInstance = VK_NULL_HANDLE;
    VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
    
#if ENABLE_DEBUG_CALLBACK
    VkDebugUtilsMessengerEXT m_vkDebugMessager;
#endif
    
#ifndef NDEBUG
    const bool m_enableValidationLayers = true;
#else
    const bool m_enableValidationLayers = false;
#endif
};

void HelloTriangleApplication::run()
{
	initGLFWWindow();
	initVulkan();
    
    pickPhysicalDeivce([](VkPhysicalDeviceProperties deviceProps, VkPhysicalDeviceFeatures deviceFeatures)->bool{
        // return (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader);
        return true;
    });
    
	mainLoop();
    
	cleanUp();
}

void HelloTriangleApplication::initGLFWWindow()
{
	glfwInit();	

	// Config.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	// Don't need OpenGL API.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);		// Don't resize the window.
	
	m_window = glfwCreateWindow(800, 600, "VulkanTest", nullptr, nullptr);
}

void HelloTriangleApplication::initVulkan()
{
	// Query Vulkan extensions required by GLFW
	uint32_t glfwExtCount = 0;
	const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    
    std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtCount);
    requiredExtensions.push_back("VK_EXT_debug_utils");

	if (!isRequiredExtensionSupported((uint32_t)requiredExtensions.size(), requiredExtensions.data()))
    {
		throw std::runtime_error("Some extensions are not supported.");
	}

	std::vector<const char*> requiredLayers;
    if (m_enableValidationLayers)
    {
        // Layers
        requiredLayers.push_back("VK_LAYER_KHRONOS_validation"); 
        
        if (!isRequiredLayerSupported((uint32_t)requiredLayers.size(), requiredLayers.data())) {
            throw std::runtime_error("Some layer are not supported.");
        }
    }

    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Simu";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = (uint32_t)requiredLayers.size();
	createInfo.ppEnabledLayerNames = requiredLayers.data();
	createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
	createInfo.ppEnabledExtensionNames= requiredExtensions.data();
	
#if ENABLE_DEBUG_CALLBACK
    VkDebugUtilsMessengerCreateInfoEXT debugMsgCreateInfo;
    debugMsgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMsgCreateInfo.pNext = nullptr;
    debugMsgCreateInfo.flags = 0;
    debugMsgCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    debugMsgCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMsgCreateInfo.pfnUserCallback = debugCallback;
    debugMsgCreateInfo.pUserData = nullptr;
    
    if (CreateDebugUtilsMessengerEXT(m_vkInstance, &debugMsgCreateInfo, nullptr, &m_vkDebugMessager) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create debug utils messenger.");
    }
    createInfo.pNext = &debugMsgCreateInfo;
#endif
    
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
#if ENABLE_DEBUG_CALLBACK
    DestroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugMessager, nullptr);
#endif
    
    // Destroy instance
	vkDestroyInstance(m_vkInstance, nullptr);
    
    // Shutdown glfw
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

void HelloTriangleApplication::pickPhysicalDeivce(std::function<bool(VkPhysicalDeviceProperties, VkPhysicalDeviceFeatures)> isSuitableFunc) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);
    
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find physical device supporting Vulkan.");
    }
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    VkCall(vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data()));
    
    for (auto device : devices) {
        // Get device properties and features.
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        
        // Check if suitable.
        if (isSuitableFunc(deviceProperties, deviceFeatures)) {
            // Pick this.
            m_vkPhysicalDevice = device;
            break;
        }
    }
    
    if (m_vkPhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU.");
    }
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
