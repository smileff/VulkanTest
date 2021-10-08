#ifndef __VULKAN_FUNCTION_H__
#define __VULKAN_FUNCTION_H__

#include "vulkan/vulkan.h"

#include <vector>

void PrintSupportedExtentions();
bool IsExtensionsSupported(const std::vector<const char*>& extensions);

void PrintSupportedLayers();
bool IsLayersSupported(const std::vector<const char*>& layers);

bool CreateVulkanInstance(const std::vector<const char*>& requiredExtensions, const std::vector<const char*>& requiredLayers, VkInstance* vkInst);

bool PickPhysicalDevice(VkInstance vkInst, VkPhysicalDevice* physicalDevice);


VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, VkDebugUtilsMessageTypeFlagsEXT msgTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

VkDebugUtilsMessengerCreateInfoEXT MakeVkDebugUtilsMessengerCreateInfoEXT();

VkResult CreateDebugUtilsMessengerEXT(VkInstance vkInst, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* vkMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance vkInst, VkDebugUtilsMessengerEXT vkMessenger, VkAllocationCallbacks* pAllocator);


#endif // __VULKAN_FUNCTION_H__
