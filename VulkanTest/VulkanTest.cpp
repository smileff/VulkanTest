// VulkanTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

// The application.

#define VkCall(func) if ((func) != VK_SUCCESS) throw std::runtime_error(std::string("Failed to call " #func ", file: ") + std::string(__FILE__) + std::string(", line: ") + std::to_string(__LINE__));

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
	void mainLoop();
	void cleanUp();

    bool isRequiredLayerSupported(uint32_t layerCount, const char **layers);
	bool isRequiredInstanceExtensionSupported(uint32_t extCount, const char **extensions);
    bool isRequiredDeviceExtensionSupported(VkPhysicalDevice vkPhysicalDevice, uint32_t extCount, const char **extensions);

	GLFWwindow *m_window = nullptr;
	VkInstance m_vkInstance = VK_NULL_HANDLE;
    VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_vkDevice = VK_NULL_HANDLE;
    VkQueue m_vkQueue = VK_NULL_HANDLE;
    
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
	// Required extensions.

	uint32_t glfwExtCount = 0;
	const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    
    std::vector<const char*> requiredInstanceExtensions(glfwExtensions, glfwExtensions + glfwExtCount);
    requiredInstanceExtensions.push_back("VK_EXT_debug_utils");
    requiredInstanceExtensions.push_back("VK_KHR_get_physical_device_properties2");
	if (!isRequiredInstanceExtensionSupported((uint32_t)requiredInstanceExtensions.size(), requiredInstanceExtensions.data()))
    {
		throw std::runtime_error("Some extensions are not supported.");
	}

	// Required layers

	std::vector<const char*> requiredLayers;
    if (m_enableValidationLayers) {
        requiredLayers.push_back("VK_LAYER_KHRONOS_validation");                 
    }
	if (!isRequiredLayerSupported((uint32_t)requiredLayers.size(), requiredLayers.data())) {
		throw std::runtime_error("Some layer are not supported.");
	}

	// Create vulkan instance.

    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Simu";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
	VkInstanceCreateInfo instCreateInfo;
	instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instCreateInfo.pNext = nullptr;
	instCreateInfo.flags = 0;
	instCreateInfo.pApplicationInfo = &appInfo;
	instCreateInfo.enabledLayerCount = (uint32_t)requiredLayers.size();
	instCreateInfo.ppEnabledLayerNames = requiredLayers.data();
	instCreateInfo.enabledExtensionCount = (uint32_t)requiredInstanceExtensions.size();
	instCreateInfo.ppEnabledExtensionNames= requiredInstanceExtensions.data();
	
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
    
    VkCall(vkCreateInstance(&instCreateInfo, nullptr, &m_vkInstance));

    // Pick physical device.
    
    uint32_t physicalDeviceCount = 0;
    VkCall(vkEnumeratePhysicalDevices(m_vkInstance, &physicalDeviceCount, nullptr));
    if (physicalDeviceCount == 0) {
        throw std::runtime_error("Failed to find physical device supporting Vulkan.");
    }
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    VkCall(vkEnumeratePhysicalDevices(m_vkInstance, &physicalDeviceCount, physicalDevices.data()));

    // Find the first physical device support graphic queue.
    uint32_t queueFamilyIndex = 0;
    for (auto device : physicalDevices) {
        // Get device properties and features.
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        
        uint32_t queueFamilyPropertiesCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount, nullptr);
        
        if (queueFamilyPropertiesCount > 0) {
            std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount, queueFamilyProperties.data());
            
            queueFamilyIndex = 0;
            for (const VkQueueFamilyProperties &prop : queueFamilyProperties) {
                if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    // Found
                    m_vkPhysicalDevice = device;
                    break;
                }
                ++queueFamilyIndex;
            }
        }
        
        if (m_vkPhysicalDevice != VK_NULL_HANDLE) {
            break;
        }
    }
    if (m_vkPhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU.");
    }
    
    // Create the logical device.
    
    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = nullptr;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    deviceQueueCreateInfo.queueCount = 1;
    float queuePriorities = 1.0f;
    deviceQueueCreateInfo.pQueuePriorities = &queuePriorities;
    
    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;

    // Required device extensions.
    std::vector<const char*> requiredDeviceExtensions;
    requiredDeviceExtensions.push_back("VK_KHR_portability_subset");
    if (!isRequiredDeviceExtensionSupported(m_vkPhysicalDevice, (uint32_t)requiredDeviceExtensions.size(), requiredDeviceExtensions.data())) {
        throw std::runtime_error("Some device extensions are not supported.");
    }
    deviceCreateInfo.enabledExtensionCount = (uint32_t)requiredDeviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

    deviceCreateInfo.pEnabledFeatures = nullptr;

    VkCall(vkCreateDevice(m_vkPhysicalDevice, &deviceCreateInfo, nullptr, &m_vkDevice));
    
    // Get queue
    vkGetDeviceQueue(m_vkDevice, queueFamilyIndex, 0, &m_vkQueue);
    if (m_vkQueue == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to get device queue.");
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
    vkDestroyDevice(m_vkDevice, nullptr);
	vkDestroyInstance(m_vkInstance, nullptr);
    
    // Shutdown glfw
	glfwDestroyWindow(m_window);
	glfwTerminate();
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

    /*
    std::cout << "Instance Layer Properties:" << std::endl;
    for (const auto &prop : instLayerProps) {
        std::cout << "* " << prop.layerName << ": " << prop.description << std::endl;
    }
    std::cout << std::endl;
     */

    // Check if all required layer are supported.
    std::cout << "Required instance layer: " << std::endl;
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

bool HelloTriangleApplication::isRequiredInstanceExtensionSupported(uint32_t extCount, const char **extensions)
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

    /*
	std::cout << "Instance Extension Properties:" << std::endl;
	for (const auto &prop : instExtProps) {
		std::cout << "* " << prop.extensionName << std::endl;
	}
	std::cout << std::endl;
     */

	// Check if all required extension are supported.	
	std::cout << "Required instance extension: " << std::endl;
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

bool HelloTriangleApplication::isRequiredDeviceExtensionSupported(VkPhysicalDevice vkPhysicalDevice, uint32_t extCount, const char **extensions)
{
    // Obtain Vulkan extensions supported.
    uint32_t extPropCount = 0;
    VkCall(vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extPropCount, nullptr));

    std::vector<VkExtensionProperties> extProps(extPropCount);
    VkCall(vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extPropCount, extProps.data()));

    /*
    std::cout << "Device Extension Properties:" << std::endl;
    for (const auto &prop : extProps) {
        std::cout << "* " << prop.extensionName << std::endl;
    }
    std::cout << std::endl;
     */

    // Check if all required extension are supported.
    std::cout << "Required device extension: " << std::endl;
    for (uint32_t i = 0; i < extCount; ++i) {
        const char *ext = extensions[i];

        auto iter = std::find_if(extProps.begin(), extProps.end(), [ext](const auto &x){
            return strcmp(x.extensionName, ext) == 0;
        });
        if (iter != extProps.end()) {
            std::cout << "* " << ext << " is supported." << std::endl;
        } else {
            std::cout << "Required extension: " << ext << " is not supported." << std::endl;
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
