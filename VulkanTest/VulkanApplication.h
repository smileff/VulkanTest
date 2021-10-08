#ifndef _VULKAN_TEST_APPLICATION_H_
#define _VULKAN_TEST_APPLICATION_H_

#include "VulkanFunctions.h"

#define ENABLE_VULKAN_VALIDATION_LAYERS (!NDEBUG)
#define ENABLE_DEBUG_MESSENGER 1

class VulkanApplication
{
public:

	void InitVulkan(std::vector<const char*> requiredExtensions, std::vector<const char*> requiredLayers);

	virtual bool InitApplication();

	virtual void RunFrame();

	virtual void CleanupApplication();

	void CleanupVulkan();
	
private:
	// Create Vulkan instance
	VkInstance m_vkInstance = VK_NULL_HANDLE;

	// Phsyical device
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

	// Custom callback.
#if ENABLE_DEBUG_MESSENGER
	VkDebugUtilsMessengerEXT m_vkDebugMessenger = VK_NULL_HANDLE;
#endif	

};

#endif
