#ifndef _HELLO_TRIANGLE_APPLICATION_H_
#define _HELLO_TRIANGLE_APPLICATION_H_

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define ENABLE_VULKAN_VALIDATION_LAYERS (!NDEBUG)
#define ENABLE_DEBUG_MESSENGER 1


class HelloTriangleApplication
{
public:
	void run();

private:
	void mainLoop();

	// GLFW window
private:
	void initWindow();
	void cleanUpWindow();
	GLFWwindow *m_window = nullptr;

	// Vulkan
private:
	void initVulkan();
	void cleanUpVulkan();
	VkInstance m_vkInstance = VK_NULL_HANDLE;

	static bool createVulkanInstance(const std::vector<const char*> &requiredExtensions, const std::vector<const char*> &requiredLayers, VkInstance *vkInst);

	static bool isRequiredExtensionSupported(uint32_t extCount, const char* const *extensions);
	static bool isRequiredLayerSupported(uint32_t layerCount, const char * const *layers);
	
	// Debug Messenger.
#if ENABLE_DEBUG_MESSENGER	
	VkDebugUtilsMessengerEXT m_vkDebugMessenger;
	VkDebugUtilsMessengerCreateInfoEXT makeVkDebugUtilsMessengerCreateInfoEXT();

	static VkResult createDebugUtilsMessenger(VkInstance vkInst, const VkDebugUtilsMessengerCreateInfoEXT *createInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *vkMessenger);

	void DestroyDebugUtilsMessengerEXT(VkInstance vkInst, VkDebugUtilsMessengerEXT vkMessenger, VkAllocationCallbacks *pAllocator);
#endif

};


#endif