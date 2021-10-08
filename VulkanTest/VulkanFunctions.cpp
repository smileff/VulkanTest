#include "VulkanFunctions.h"

#include <stdlib.h>
#include <iostream>

#define VkCall(func) if ((func) != VK_SUCCESS) throw std::runtime_error(std::string("Failed to call " #func ", file: ") + std::string(__FILE__) + std::string(", line: ") + std::to_string(__LINE__));

// -----------------------------------------

// VulkanInstance


void PrintSupportedExtentions()
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
	for (const auto& prop : instExtProps) {
		std::cout << "* " << prop.extensionName << std::endl;
	}
	std::cout << std::endl;
}

bool IsExtensionsSupported(const std::vector<const char*> &extensions)
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
		
	// Check if all required extension are supported.	
	std::cout << "Required extension: " << std::endl;
	for (uint32_t i = 0; i < extensions.size(); ++i) 
	{
		const char* ext = extensions[i];

		auto iter = std::find_if(instExtProps.begin(), instExtProps.end(), [ext](const auto& x) {
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

void PrintSupportedLayers()
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
	for (const auto& prop : instLayerProps) {
		std::cout << "* " << prop.layerName << ": " << prop.description << std::endl;
	}
	std::cout << std::endl;
}

bool IsLayersSupported(const std::vector<const char*> &layers)
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

	// Check if all required layer are supported.
	std::cout << "Required layer: " << std::endl;
	for (uint32_t i = 0; i < layers.size(); ++i) {
		const char* layer = layers[i];

		auto iter = std::find_if(instLayerProps.begin(), instLayerProps.end(), [layer](const VkLayerProperties& x) {
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

bool CreateVulkanInstance(
	const std::vector<const char*>& requiredExtensions,
	const std::vector<const char*>& requiredLayers,
	VkInstance* vkInst)
{
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
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	if (vkCreateInstance(&createInfo, nullptr, vkInst) != VK_SUCCESS) {
		std::cerr << "Failed to create Vulkan instance." << std::endl;
		return false;
	}

	return true;
}

bool PickPhysicalDevice(VkInstance vkInst, VkPhysicalDevice* physicalDevice)
{
	// Enumerate devices
	uint32_t deviceCount = 0;

	if (vkEnumeratePhysicalDevices(vkInst, &deviceCount, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("Failed to enumerate physical devices.");
	}

	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);

	if (vkEnumeratePhysicalDevices(vkInst, &deviceCount, physicalDevices.data())) {
		throw std::runtime_error("Failed to enumerate physical devices.");
	}

	// List physical devices
	//std::cout << "Physical Devices:" << std::endl;
	//for (const auto &device : physicalDevices) {
	//	std::cout << "* " << device.
	//}

	return true;
}



VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, VkDebugUtilsMessageTypeFlagsEXT msgTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	if (msgTypes >= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
		std::cerr << "Callback message: " << pCallbackData->pMessage << std::endl;
	}

	return VK_FALSE;    // Return VK_TRUE will terminate the application.
}

VkDebugUtilsMessengerCreateInfoEXT MakeVkDebugUtilsMessengerCreateInfoEXT()
{
	VkDebugUtilsMessengerCreateInfoEXT debugMsgCreateInfo;
	debugMsgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugMsgCreateInfo.pNext = nullptr;
	debugMsgCreateInfo.flags = 0;
	debugMsgCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	debugMsgCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugMsgCreateInfo.pfnUserCallback = DebugCallback;
	debugMsgCreateInfo.pUserData = nullptr;

	return debugMsgCreateInfo;
}


VkResult CreateDebugUtilsMessengerEXT(VkInstance vkInst, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* vkMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInst, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(vkInst, createInfo, pAllocator, vkMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance vkInst, VkDebugUtilsMessengerEXT vkMessenger, VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInst, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(vkInst, vkMessenger, pAllocator);
	}
}
