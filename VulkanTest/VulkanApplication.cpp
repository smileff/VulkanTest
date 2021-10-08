#include "VulkanApplication.h"
#include <iostream>


void VulkanApplication::InitVulkan(std::vector<const char*> requiredExtensions, std::vector<const char*> requiredLayers)
{
#if ENABLE_VULKAN_VALIDATION_LAYERS
	// Also require debug extension.
	requiredExtensions.push_back("VK_EXT_debug_utils");
	requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	// Validate required extension.
	if (!IsExtensionsSupported(requiredExtensions))
	{
		std::cerr << "Some extensions are not supported." << std::endl;
	}

	// Validate required layers	
	if (!IsLayersSupported(requiredLayers)) {
		std::cerr << "Some layer are not supported." << std::endl;
	}


	if (!CreateVulkanInstance(requiredExtensions, requiredLayers, &m_vkInstance)) {
		throw std::runtime_error("Failed to create vulkan instance.");
	}

#if ENABLE_DEBUG_MESSENGER
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = MakeVkDebugUtilsMessengerCreateInfoEXT();
	if (CreateDebugUtilsMessengerEXT(m_vkInstance, &debugMessengerCreateInfo, nullptr, &m_vkDebugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create vulkan debug messenger");
	}
#endif
}

bool VulkanApplication::InitApplication()
{
	return true;
}

void VulkanApplication::RunFrame()
{
	
}

void VulkanApplication::CleanupApplication()
{	
}

void VulkanApplication::CleanupVulkan()
{
#if ENABLE_DEBUG_MESSENGER
	DestroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugMessenger, nullptr);
#endif

	vkDestroyInstance(m_vkInstance, nullptr);
}