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
	void Run();

	// GLFW window
private:
	static GLFWwindow* InitGLFW(int winWidth = 800, int winHeight = 600, const char *caption = "Vulkan Window");
	static void RunGLFWWindowMainLoop(GLFWwindow *win);
	static void CleanupGLFW(GLFWwindow *win);	

	// Vulkan
private:
	void initVulkan();
	void cleanUpVulkan();

	// Vulkan instance
	VkInstance m_vkInstance = VK_NULL_HANDLE;

	static bool createVulkanInstance(const std::vector<const char*> &requiredExtensions, const std::vector<const char*> &requiredLayers, VkInstance *vkInst);

	static bool isRequiredExtensionSupported(uint32_t extCount, const char* const *extensions);
	static bool isRequiredLayerSupported(uint32_t layerCount, const char * const *layers);

	// Phsyical device
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	static bool pickPhysicalDevice(VkInstance vkInst, VkPhysicalDevice *physicalDevice);

	
	// Debug Messenger.
#if ENABLE_DEBUG_MESSENGER	
	VkDebugUtilsMessengerEXT m_vkDebugMessenger = VK_NULL_HANDLE;
	VkDebugUtilsMessengerCreateInfoEXT makeVkDebugUtilsMessengerCreateInfoEXT();

	static VkResult createDebugUtilsMessenger(VkInstance vkInst, const VkDebugUtilsMessengerCreateInfoEXT *createInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *vkMessenger);

	void DestroyDebugUtilsMessengerEXT(VkInstance vkInst, VkDebugUtilsMessengerEXT vkMessenger, VkAllocationCallbacks *pAllocator);
#endif

};


#endif