#pragma once
#include "../CommonFiles/Common.h"

namespace VulkanSampleFramework
{
	struct PresentInfo
	{
		VkSwapchainKHR  m_Swapchain;
		uint32_t        m_ImageIndex;
	};

	bool CreatePresentationSurface(VkInstance instance, WindowParameters windowParameters, VkSurfaceKHR &presentationSurface);
	bool SelectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface,
		std::vector<VkQueueFamilyProperties> &queuFamiliesProperties, uint32_t &queueFamilyIndex);
	bool SelectDesiredPresentationMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkPresentModeKHR desiredPresentMode,
		VkPresentModeKHR &presentMode);
	bool GetCapabilitiesOfPresentationSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkSurfaceCapabilitiesKHR &surfaceCapabilities);
	bool SelectNumberOfSwapchainImages(VkSurfaceCapabilitiesKHR const &surfaceCapabilities, uint32_t &numberOfImages);
	bool ChooseSizeOfSwapchainImages(VkSurfaceCapabilitiesKHR const &surfaceCapabilities, VkExtent2D &sizeOfImages);
	bool SelectDesiredUsageScenariosOfSwapchainImages(VkSurfaceCapabilitiesKHR const &surfaceCapabilities, VkImageUsageFlags desiredUsages,
		VkImageUsageFlags &imageUsage);
	bool SelectTransformationOfSwapchainImages(VkSurfaceCapabilitiesKHR const &surfaceCapabilities, VkSurfaceTransformFlagBitsKHR desiredTransform,
		VkSurfaceTransformFlagBitsKHR  &surfaceTransform);
	bool SelectFormatOfSwapchainImages(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkSurfaceFormatKHR desiredSurfaceFormat,
		VkFormat &imagFormat, VkColorSpaceKHR &imageColorSpace);
	bool CreateSwapchain(VkDevice logicalDevice, VkSurfaceKHR presentationSurface, uint32_t imageCount, VkSurfaceFormatKHR surfaceFormat,
		VkExtent2D imageSize, VkImageUsageFlags imageUsage, VkSurfaceTransformFlagBitsKHR surfaceTransform, VkPresentModeKHR presentMode,
		VkSwapchainKHR &oldSwapchain, VkSwapchainKHR &swapchain);
	bool GetHandlesOfSwapchainImages(VkDevice logicalDevice, VkSwapchainKHR swapchain, std::vector<VkImage> & swapchainImages);
	bool AcquireSwapchainImage(VkDevice logicalDevice, VkSwapchainKHR swapchain, VkSemaphore semaphore, VkFence fence, uint32_t &imageIndex);
	bool PresentImage(VkQueue queue, std::vector<VkSemaphore> renderingSemaphores, std::vector<PresentInfo> imagesToPresent);
	void DestroySwapchain(VkDevice logicalDevice, VkSwapchainKHR &swapchain);
	void DestroyPresentationSurface(VkInstance instance, VkSurfaceKHR &presentationSurface);
}