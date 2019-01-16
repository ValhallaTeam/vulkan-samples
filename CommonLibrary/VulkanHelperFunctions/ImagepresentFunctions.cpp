#include "InstanceAndDevice.h"
#include "ImagePresentFunctions.h"

namespace VulkanSampleFramework
{
	bool CreatePresentationSurface(VkInstance instance, WindowParameters windowParameters, VkSurfaceKHR &presentationSurface)
	{
		VkResult result;

#ifdef VK_USE_PLATFORM_WIN32_KHR

		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo =
		{
			VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,	// VkStructureType                 sType
			nullptr,											// const void                    * pNext
			0,													// VkWin32SurfaceCreateFlagsKHR    flags
			windowParameters.m_hInstance,						// HINSTANCE                       hinstance
			windowParameters.m_hWnd								// HWND                            hwnd
		};

		result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);

#elif defined VK_USE_PLATFORM_XLIB_KHR

		VkXlibSurfaceCreateInfoKHR surface_create_info = {
			VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,		// VkStructureType                 sType
			nullptr,											// const void                    * pNext
			0,													// VkXlibSurfaceCreateFlagsKHR     flags
			windowParameters.m_Dpy,								// Display                       * dpy
			windowParameters.m_Window							// Window                          window
		};

		result = vkCreateXlibSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);

#elif defined VK_USE_PLATFORM_XCB_KHR

		VkXcbSurfaceCreateInfoKHR surface_create_info = {
			VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,		// VkStructureType                 sType
			nullptr,											// const void                    * pNext
			0,													// VkXcbSurfaceCreateFlagsKHR      flags
			windowParameters.m_Connection,						// xcb_connection_t              * connection
			windowParameters.m_Window							// xcb_window_t                    window
		};

		result = vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);

#endif

		if ((VK_SUCCESS != result) || (VK_NULL_HANDLE == presentationSurface))
		{
			std::cout << "Could not create presentation surface." << std::endl;
			return false;
		}
		return true;
	}

	bool SelectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface,
		std::vector<VkQueueFamilyProperties> &queuFamiliesProperties, uint32_t &queueFamilyIndex)
	{
		for (uint32_t index = 0; index < static_cast<uint32_t>(queuFamiliesProperties.size()); ++index)
		{
			VkBool32 presentationSupported = VK_FALSE;
			VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, presentationSurface, &presentationSupported);
			if ((VK_SUCCESS == result) && (VK_TRUE == presentationSupported))
			{
				queueFamilyIndex = index;
				return true;
			}
		}
		return false;
	}

	bool SelectDesiredPresentationMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkPresentModeKHR desiredPresentMode,
		VkPresentModeKHR &presentMode)
	{
		// Enumerate supported present modes
		uint32_t presentModesCount = 0;
		VkResult result = VK_SUCCESS;

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentModesCount, nullptr);
		if ((VK_SUCCESS != result) || (0 == presentModesCount))
		{
			std::cout << "Could not get the number of supported present modes." << std::endl;
			return false;
		}

		std::vector<VkPresentModeKHR> presentModes(presentModesCount);
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentModesCount, presentModes.data());
		if ((VK_SUCCESS != result) || (0 == presentModesCount))
		{
			std::cout << "Could not enumerate present modes." << std::endl;
			return false;
		}

		// Select present mode
		for (auto & currentPresentMode : presentModes)
		{
			if (currentPresentMode == desiredPresentMode)
			{
				presentMode = desiredPresentMode;
				return true;
			}
		}

		std::cout << "Desired present mode is not supported. Selecting default FIFO mode." << std::endl;
		for (auto & currentPresentMode : presentModes)
		{
			if (currentPresentMode == VK_PRESENT_MODE_FIFO_KHR)
			{
				presentMode = VK_PRESENT_MODE_FIFO_KHR;
				return true;
			}
		}

		std::cout << "VK_PRESENT_MODE_FIFO_KHR is not supported though it's mandatory for all drivers!" << std::endl;
		return false;
	}

	bool GetCapabilitiesOfPresentationSurface(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkSurfaceCapabilitiesKHR &surfaceCapabilities)
	{
		VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, presentationSurface, &surfaceCapabilities);

		if (VK_SUCCESS != result)
		{
			std::cout << "Could not get the capabilities of a presentation surface." << std::endl;
			return false;
		}

		return true;
	}

	bool SelectNumberOfSwapchainImages(VkSurfaceCapabilitiesKHR const &surfaceCapabilities, uint32_t &numberOfImages)
	{
		numberOfImages = surfaceCapabilities.minImageCount + 1;
		if((surfaceCapabilities.maxImageCount > 0) && (numberOfImages > surfaceCapabilities.maxImageCount))
		{
			numberOfImages = surfaceCapabilities.maxImageCount;
		}

		return true;
	}

	bool ChooseSizeOfSwapchainImages(VkSurfaceCapabilitiesKHR const &surfaceCapabilities, VkExtent2D &sizeOfImages)
	{
		if (0xFFFFFFFF == surfaceCapabilities.currentExtent.width)
		{
			sizeOfImages = {1280, 720};

			if (sizeOfImages.width < surfaceCapabilities.minImageExtent.width)
			{
				sizeOfImages.width = surfaceCapabilities.minImageExtent.width;
			}
			else if (sizeOfImages.width > surfaceCapabilities.maxImageExtent.width)
			{
				sizeOfImages.width = surfaceCapabilities.maxImageExtent.width;
			}

			if (sizeOfImages.height < surfaceCapabilities.minImageExtent.height)
			{
				sizeOfImages.height = surfaceCapabilities.minImageExtent.height;
			}
			else if (sizeOfImages.height > surfaceCapabilities.maxImageExtent.height)
			{
				sizeOfImages.height = surfaceCapabilities.maxImageExtent.height;
			}
		}
		else
		{
			sizeOfImages = surfaceCapabilities.currentExtent;
		}
		return true;
	}

	bool SelectDesiredUsageScenariosOfSwapchainImages(VkSurfaceCapabilitiesKHR const &surfaceCapabilities, VkImageUsageFlags desiredUsages,
		VkImageUsageFlags &imageUsage)
	{
		imageUsage = desiredUsages & surfaceCapabilities.supportedUsageFlags;
		return desiredUsages == imageUsage;
	}

	bool SelectTransformationOfSwapchainImages(VkSurfaceCapabilitiesKHR const &surfaceCapabilities, VkSurfaceTransformFlagBitsKHR desiredTransform,
		VkSurfaceTransformFlagBitsKHR  &surfaceTransform)
	{
		if (surfaceCapabilities.supportedTransforms & desiredTransform)
		{
			surfaceTransform = desiredTransform;
		}
		else
		{
			surfaceTransform = surfaceCapabilities.currentTransform;
		}

		return true;
	}

	bool SelectFormatOfSwapchainImages(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkSurfaceFormatKHR desiredSurfaceFormat,
		VkFormat &imagFormat, VkColorSpaceKHR &imageColorSpace)
	{
		// Enumerate supported formats
		uint32_t formatsCount = 0;
		VkResult result = VK_SUCCESS;

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &formatsCount, nullptr);
		if ((VK_SUCCESS != result) || (0 == formatsCount))
		{
			std::cout << "Could not get the number of supported surface formats." << std::endl;
			return false;
		}

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &formatsCount, surfaceFormats.data());
		if ((VK_SUCCESS != result) || (0 == formatsCount))
		{
			std::cout << "Could not enumerate supported surface formats." << std::endl;
			return false;
		}

		// Select surface format
		// No restriction for format and colorspace
		if ((1 == surfaceFormats.size()) &&	(VK_FORMAT_UNDEFINED == surfaceFormats[0].format))
		{
			imagFormat = desiredSurfaceFormat.format;
			imageColorSpace = desiredSurfaceFormat.colorSpace;
			return true;
		}

		for (auto & surfaceFormat : surfaceFormats)
		{
			if ((desiredSurfaceFormat.format == surfaceFormat.format) && (desiredSurfaceFormat.colorSpace == surfaceFormat.colorSpace))
			{
				imagFormat = desiredSurfaceFormat.format;
				imageColorSpace = desiredSurfaceFormat.colorSpace;
				return true;
			}
		}

		for (auto & surfaceFormat : surfaceFormats)
		{
			if ((desiredSurfaceFormat.format == surfaceFormat.format))
			{
				imagFormat = desiredSurfaceFormat.format;
				imageColorSpace = desiredSurfaceFormat.colorSpace;
				std::cout << "Desired combination of format and colorspace is not supported. Selecting other colorspace." << std::endl;
				return true;
			}
		}

		imagFormat = surfaceFormats[0].format;
		imageColorSpace = surfaceFormats[0].colorSpace;
		std::cout << "Desired format is not supported. Selecting available format - colorspace combination." << std::endl;
		return true;
	}

	bool CreateSwapchain(VkDevice logicalDevice, VkSurfaceKHR presentationSurface, uint32_t imageCount, VkSurfaceFormatKHR surfaceFormat,
		VkExtent2D imageSize, VkImageUsageFlags imageUsage, VkSurfaceTransformFlagBitsKHR surfaceTransform, VkPresentModeKHR presentMode,
		VkSwapchainKHR &oldSwapchain, VkSwapchainKHR &swapchain)
	{
		VkSwapchainCreateInfoKHR swapchainCreateInfo = 
		{
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,	// VkStructureType                  sType
			nullptr,										// const void                     * pNext
			0,												// VkSwapchainCreateFlagsKHR        flags
			presentationSurface,							// VkSurfaceKHR                     surface
			imageCount,										// uint32_t                         minImageCount
			surfaceFormat.format,							// VkFormat                         imageFormat
			surfaceFormat.colorSpace,						// VkColorSpaceKHR                  imageColorSpace
			imageSize,										// VkExtent2D                       imageExtent
			1,												// uint32_t                         imageArrayLayers
			imageUsage,										// VkImageUsageFlags                imageUsage
			VK_SHARING_MODE_EXCLUSIVE,						// VkSharingMode                    imageSharingMode
			0,												// uint32_t                         queueFamilyIndexCount
			nullptr,										// const uint32_t                 * pQueueFamilyIndices
			surfaceTransform,								// VkSurfaceTransformFlagBitsKHR    preTransform
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,				// VkCompositeAlphaFlagBitsKHR      compositeAlpha
			presentMode,									// VkPresentModeKHR                 presentMode
			VK_TRUE,										// VkBool32                         clipped
			oldSwapchain									// VkSwapchainKHR                   oldSwapchain the olad swapchain that was created previously
		};

		VkResult result = vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapchain);
		if ((VK_SUCCESS != result) || (VK_NULL_HANDLE == swapchain))
		{
			std::cout << "Could not create a swapchain." << std::endl;
			return false;
		}

		// Now the old swapchain wasn't useful
		if (VK_NULL_HANDLE != oldSwapchain)
		{
			vkDestroySwapchainKHR(logicalDevice, oldSwapchain, nullptr);
			oldSwapchain = VK_NULL_HANDLE;
		}

		return true;
	}

	bool GetHandlesOfSwapchainImages(VkDevice logicalDevice, VkSwapchainKHR swapchain, std::vector<VkImage> & swapchainImages)
	{
		uint32_t imagesCount = 0;
		VkResult result = VK_SUCCESS;

		result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imagesCount, nullptr);
		if ((VK_SUCCESS != result) || (0 == imagesCount))
		{
			std::cout << "Could not get the number of swapchain images." << std::endl;
			return false;
		}

		swapchainImages.resize(imagesCount);
		result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imagesCount, swapchainImages.data());
		if ((VK_SUCCESS != result) || (0 == imagesCount))
		{
			std::cout << "Could not enumerate swapchain images." << std::endl;
			return false;
		}

		return true;
	}

	bool AcquireSwapchainImage(VkDevice logicalDevice, VkSwapchainKHR swapchain, VkSemaphore semaphore, VkFence fence, uint32_t &imageIndex)
	{
		VkResult result;

		result = vkAcquireNextImageKHR(logicalDevice, swapchain, 2000000000 /*nanosecond*/, semaphore, fence, &imageIndex);  
		switch (result)
		{
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			return true;
		default:
			return false;
		}
	}

	bool PresentImage(VkQueue queue, std::vector<VkSemaphore> renderingSemaphores, std::vector<PresentInfo> imagesToPresent)
	{
		VkResult result;
		std::vector<VkSwapchainKHR> swapchains;
		std::vector<uint32_t> imageIndicies;

		for (auto & imageToPresent : imagesToPresent)
		{
			swapchains.emplace_back(imageToPresent.m_Swapchain);
			imageIndicies.emplace_back(imageToPresent.m_ImageIndex);
		}

		VkPresentInfoKHR presentInfo =
		{
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,						// VkStructureType          sType
			nullptr,												// const void*              pNext
			static_cast<uint32_t>(renderingSemaphores.size()),		// uint32_t                 waitSemaphoreCount
			renderingSemaphores.data(),								// const VkSemaphore      * pWaitSemaphores
			static_cast<uint32_t>(swapchains.size()),				// uint32_t                 swapchainCount
			swapchains.data(),										// const VkSwapchainKHR   * pSwapchains
			imageIndicies.data(),									// const uint32_t         * pImageIndices
			nullptr													// VkResult*                pResults
		};

		result = vkQueuePresentKHR(queue, &presentInfo);
		switch (result)
		{
		case VK_SUCCESS:
			return true;
		default:
			return false;
		}
	}

	void DestroySwapchain(VkDevice logicalDevice, VkSwapchainKHR &swapchain)
	{
		if (swapchain)
		{
			vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
			swapchain = VK_NULL_HANDLE;
		}
	}

	void DestroyPresentationSurface(VkInstance instance, VkSurfaceKHR &presentationSurface)
	{
		if (presentationSurface)
		{
			vkDestroySurfaceKHR(instance, presentationSurface, nullptr);
			presentationSurface = VK_NULL_HANDLE;
		}
	}

}
