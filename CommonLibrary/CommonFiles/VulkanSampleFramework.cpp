#include "VulkanSampleFramework.h"

namespace VulkanSampleFramework
{
	MouseStateParameters::MouseStateParameters() 
	{
		Buttons[0].IsPressed = false;
		Buttons[0].WasClicked = false;
		Buttons[0].WasRelease = false;
		Buttons[1].IsPressed = false;
		Buttons[1].WasClicked = false;
		Buttons[1].WasRelease = false;
		Position.X = 0;
		Position.Y = 0;
		Position.Delta.X = 0;
		Position.Delta.Y = 0;
		Wheel.WasMoved = false;
		Wheel.Distance = 0.0f;
	}

	MouseStateParameters::~MouseStateParameters()
	{
	}

	float TimerStateParameters::GetTime() const
	{
		auto duration = Time.time_since_epoch();
		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		return static_cast<float>(milliseconds * 0.001f);
	}

	float TimerStateParameters::GetDeltaTime() const
	{
		return DeltaTime.count();
	}

	void TimerStateParameters::Update()
	{
		auto previous_time = Time;
		Time = std::chrono::high_resolution_clock::now();
		DeltaTime = std::chrono::high_resolution_clock::now() - previous_time;
	}

	TimerStateParameters::TimerStateParameters()
	{
		Update();
	}

	TimerStateParameters::~TimerStateParameters()
	{
	}

	VulkanSampleBase::VulkanSampleBase() :
		m_VulkanLibrary(nullptr),
		m_Ready(false)
	{
	}

	VulkanSampleBase::~VulkanSampleBase()
	{
		//ReleaseVulkanLoaderLibrary(m_VulkanLibrary);
	}

	void VulkanSampleBase::MouseClick(size_t buttonIndex, bool state)
	{
		if (2 > buttonIndex)
		{
			m_MouseState.Buttons[buttonIndex].IsPressed = state;
			m_MouseState.Buttons[buttonIndex].WasClicked = state;
			m_MouseState.Buttons[buttonIndex].WasRelease = !state;
			OnMouseEvent();
		}
	}

	void VulkanSampleBase::MouseMove(int x, int y)
	{
		m_MouseState.Position.Delta.X = x - m_MouseState.Position.X;
		m_MouseState.Position.Delta.Y = y - m_MouseState.Position.Y;
		m_MouseState.Position.X = x;
		m_MouseState.Position.Y = y;

		OnMouseEvent();
	}

	void VulkanSampleBase::MouseWheel(float distance)
	{
		m_MouseState.Wheel.WasMoved = true;
		m_MouseState.Wheel.Distance = distance;
		OnMouseEvent();
	}

	void VulkanSampleBase::MouseReset()
	{
		m_MouseState.Position.Delta.X = 0;
		m_MouseState.Position.Delta.Y = 0;
		m_MouseState.Buttons[0].WasClicked = false;
		m_MouseState.Buttons[0].WasRelease = false;
		m_MouseState.Buttons[1].WasClicked = false;
		m_MouseState.Buttons[1].WasRelease = false;
		m_MouseState.Wheel.WasMoved = false;
		m_MouseState.Wheel.Distance = 0.0f;
	}

	void VulkanSampleBase::UpdateTime()
	{
		m_TimerState.Update();
	}

	bool VulkanSampleBase::IsReady()
	{
		return m_Ready;
	}

	void VulkanSampleBase::OnMouseEvent()
	{
		// Override this in a derived class to know when a mouse event occured
	}

	bool VulkanSample::InitializeVulkan(WindowParameters windowParameters, VkPhysicalDeviceFeatures *desiredDeviceFeatures,
		VkImageUsageFlags swapchainImageUsage, bool useDepth, VkImageUsageFlags depthAttachmentUsage)
	{
		if (!ConnectWithVulkanLoaderLibrary(m_VulkanLibrary))
		{
			return false;
		}

		if (!LoadFunctionExportedFromVulkanLoaderLibrary(m_VulkanLibrary))
		{
			return false;
		}

		if (!LoadGlobalLevelFunctions())
		{
			return false;
		}

		std::vector<char const *> instanceExtensions;
		instanceExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
		instanceExtensions.emplace_back(
#ifdef VK_USE_PLATFORM_WIN32_KHR
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME

#elif defined VK_USE_PLATFORM_XCB_KHR
			VK_KHR_XCB_SURFACE_EXTENSION_NAME

#elif defined VK_USE_PLATFORM_XLIB_KHR
			VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif
		);

		if (!CreateVulkanInstance(instanceExtensions, "Vulkan Sample", m_Instance))
		{
			return false;
		}

		if (!LoadInstanceLevelFunctions(m_Instance, instanceExtensions))
		{
			return false;
		}

		if (!CreatePresentationSurface(m_Instance, windowParameters, m_PresentationSurface))
		{
			return false;
		}

		std::vector<VkPhysicalDevice> physicalDevices;
		EnumerateAvailablePhysicalDevices(m_Instance, physicalDevices);


		for (auto & physicalDevice : physicalDevices)
		{
			std::vector<VkQueueFamilyProperties> queueFamilies;
			if (!CheckAvailableQueueFamiliesAndTheirProperties(physicalDevice, queueFamilies))
			{
				continue;
			}

			if (!SelectIndexOfQueueFamilyWithDesiredCapabilities(physicalDevice, VK_QUEUE_GRAPHICS_BIT, queueFamilies, m_GraphicsQueue.m_FamilyIndex))
			{
				continue;
			}

			if (!SelectIndexOfQueueFamilyWithDesiredCapabilities(physicalDevice, VK_QUEUE_COMPUTE_BIT, queueFamilies, m_ComputeQueue.m_FamilyIndex))
			{
				continue;
			}

			if (!SelectQueueFamilyThatSupportsPresentationToGivenSurface(physicalDevice, m_PresentationSurface, queueFamilies, m_PresentQueue.m_FamilyIndex))
			{
				continue;
			}

			std::vector<QueueInfo> requestedQueues = {{m_GraphicsQueue.m_FamilyIndex, {1.0f}}};
			if (m_GraphicsQueue.m_FamilyIndex != m_ComputeQueue.m_FamilyIndex)
			{
				requestedQueues.push_back({m_ComputeQueue.m_FamilyIndex, {1.0f}});
			}

			if ((m_GraphicsQueue.m_FamilyIndex != m_PresentQueue.m_FamilyIndex) && (m_ComputeQueue.m_FamilyIndex != m_PresentQueue.m_FamilyIndex))
			{
				requestedQueues.push_back({m_PresentQueue.m_FamilyIndex, {1.0f}});
			}

			std::vector<char const *> deviceExtensions;
			deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			if (!CreateLogicalDevice(physicalDevice, requestedQueues, deviceExtensions, desiredDeviceFeatures, m_LogicalDevice))
			{
				continue;
			}
			else
			{
				m_PhysicalDevice = physicalDevice;
				LoadDeviceLevelFunctions(m_LogicalDevice, deviceExtensions);
				GetDeviceQueue(m_LogicalDevice, m_GraphicsQueue.m_FamilyIndex, 0, m_GraphicsQueue.m_Handle);
				GetDeviceQueue(m_LogicalDevice, m_ComputeQueue.m_FamilyIndex, 0, m_ComputeQueue.m_Handle);
				GetDeviceQueue(m_LogicalDevice, m_PresentQueue.m_FamilyIndex, 0, m_PresentQueue.m_Handle);
				break;
			}
		}

		// Get physical device memory properties, that use for memory object
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_PhysicalDeviceMemoryProperties);

		if (!m_LogicalDevice)
		{
			return false;
		}

		// Prepare frame resources
		// If we want to use command buffer in different thread, maybe we should crate multi command pool here 
		if (!CreateCommandPool(m_LogicalDevice, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_GraphicsQueue.m_FamilyIndex, m_CommandPool))
		{
			return false;
		}

		for (uint32_t i = 0; i < m_FramesCount; ++i)
		{
			m_FramesResources.emplace_back(FrameResources());
			m_FramesResources.back().Initiallize(m_LogicalDevice, m_CommandPool);
		}

		if (!CreateSwapchain(swapchainImageUsage, useDepth, depthAttachmentUsage)) 
		{
			return false;
		}

		return true;
	}

	bool VulkanSample::CreateSwapchain(VkImageUsageFlags swapchainImageUsage, bool useDepth, VkImageUsageFlags depthAttachmentUsage)
	{
		WaitForAllSubmittedCommandsToBeFinished(m_LogicalDevice);

		m_Ready = false;

		m_Swapchain.DestroyResources(m_LogicalDevice);

		VkSwapchainKHR oldSwapchain = std::move(m_Swapchain.m_Handle);
		if (!CreateSwapChainCustom(swapchainImageUsage, VK_FORMAT_R8G8B8A8_UNORM, VK_PRESENT_MODE_MAILBOX_KHR, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, oldSwapchain))
		{
			return false;
		}

		if (!m_Swapchain.m_Handle)
		{
			return true;
		}

		for (size_t i = 0; i < m_Swapchain.m_Images.size(); ++i)
		{
			m_Swapchain.m_ImageViews.emplace_back(VkImageView());
			if (!CreateImageView(m_LogicalDevice, m_Swapchain.m_Images[i], VK_IMAGE_VIEW_TYPE_2D, m_Swapchain.m_Format, VK_IMAGE_ASPECT_COLOR_BIT,
				m_Swapchain.m_ImageViews.back()))
			{
				return false;
			}
		}

		// When we want to use depth buffering, we need to use a depth attachment
		// It must have the same size as the swapchain, so we need to recreate it along with the swapchain
		m_DepthImages.clear();
		m_DepthImagesMemory.clear();

		if (useDepth)
		{
			for (uint32_t i = 0; i < m_FramesCount; ++i) 
			{
				m_DepthImages.emplace_back(VkImage());
				m_DepthImagesMemory.emplace_back(VkDeviceMemory());

				if (!CreateImage(m_LogicalDevice, VK_IMAGE_TYPE_2D, m_DepthFormat, { m_Swapchain.m_Size.width, m_Swapchain.m_Size.height, 1 },
					1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, false, m_DepthImages.back()))
				{
					return false;
				}

				if (!AllocateAndBindMemoryObjectToImage(m_LogicalDevice, m_DepthImages.back(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					m_PhysicalDeviceMemoryProperties, m_DepthImagesMemory.back()))
				{
					return false;
				}

				if (!CreateImageView(m_LogicalDevice, m_DepthImages.back(), VK_IMAGE_VIEW_TYPE_2D, m_DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT,
					m_FramesResources[i].m_DepthAttachment))
				{
					return false;
				}
			}
		}

		DestroySwapchain(m_LogicalDevice, oldSwapchain);
		m_Ready = true;
		return true;
	}


	void VulkanSample::Deinitialize()
	{
		if (m_LogicalDevice) 
		{
			WaitForAllSubmittedCommandsToBeFinished(m_LogicalDevice);

			for (int i = 0; i < m_FramesResources.size(); ++i)
			{
				m_FramesResources[i].Destroy(m_LogicalDevice);
			}
			m_FramesResources.clear();

			for (int i = 0; i < m_DepthImagesMemory.size(); ++i)
			{
				FreeMemoryObject(m_LogicalDevice, m_DepthImagesMemory[i]);
			}
			m_DepthImagesMemory.clear();

			for (int i = 0; i < m_DepthImages.size(); ++i)
			{
				DestroyImage(m_LogicalDevice, m_DepthImages[i]);
			}
			m_DepthImages.clear();

			DestroyCommandPool(m_LogicalDevice, m_CommandPool);
			//m_Swapchain.DestroyResources(m_LogicalDevice);
			DestroyPresentationSurface(m_Instance, m_PresentationSurface);
			DestroyLogicalDevice(m_LogicalDevice);
			DestroyVulkanInstance(m_Instance);
		}
	}

	bool VulkanSample::CreateSwapChainCustom(VkImageUsageFlags swapchainImageUsage,	VkFormat desireFormat, VkPresentModeKHR desirePresentMode, VkColorSpaceKHR desireColorSpace,
		VkSwapchainKHR &oldSwapchain)
	{
		VkPresentModeKHR selectedPresentMode;
		if (!SelectDesiredPresentationMode(m_PhysicalDevice, m_PresentationSurface, desirePresentMode, selectedPresentMode))
		{
			return false;
		}

		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		if (!GetCapabilitiesOfPresentationSurface(m_PhysicalDevice, m_PresentationSurface, surfaceCapabilities))
		{
			return false;
		}

		uint32_t numberOfImages;
		if (!SelectNumberOfSwapchainImages(surfaceCapabilities, numberOfImages))
		{
			return false;
		}

		if (!ChooseSizeOfSwapchainImages(surfaceCapabilities, m_Swapchain.m_Size))
		{
			return false;
		}

		if ((0 == m_Swapchain.m_Size.width) || (0 == m_Swapchain.m_Size.height))
		{
			return true;
		}

		VkImageUsageFlags imageUsage;
		if (!SelectDesiredUsageScenariosOfSwapchainImages(surfaceCapabilities, swapchainImageUsage, imageUsage))
		{
			return false;
		}

		// Use default transform
		VkSurfaceTransformFlagBitsKHR surfaceTransform;
		SelectTransformationOfSwapchainImages(surfaceCapabilities, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, surfaceTransform);

		VkColorSpaceKHR imageColorSpace;
		if (!SelectFormatOfSwapchainImages(m_PhysicalDevice, m_PresentationSurface, {desireFormat, desireColorSpace}, m_Swapchain.m_Format, imageColorSpace))
		{
			return false;
		}

		if (!VulkanSampleFramework::CreateSwapchain(m_LogicalDevice, m_PresentationSurface, numberOfImages, {m_Swapchain.m_Format, imageColorSpace}, m_Swapchain.m_Size, imageUsage, surfaceTransform,
			selectedPresentMode, oldSwapchain, m_Swapchain.m_Handle))
		{
			return false;
		}

		if (!GetHandlesOfSwapchainImages(m_LogicalDevice, m_Swapchain.m_Handle, m_Swapchain.m_Images))
		{
			return false;
		}
		return true;
	}

}