#pragma once

#include <chrono>
#include "AllHelperFunctionsHeader.h"
#include "OS.h"
#include "Tools.h"

namespace VulkanSampleFramework
{

	// Storage for mouse state parameters
	class MouseStateParameters
	{
	public:
		struct ButtonsState
		{
			bool IsPressed;
			bool WasClicked;
			bool WasRelease;
		} Buttons[2];

		struct Position
		{
			int X;
			int Y;
			struct Delta
			{
				int X;
				int Y;
			} Delta;
		} Position;

		struct WheelState
		{
			bool  WasMoved;
			float Distance;
		} Wheel;

		MouseStateParameters();
		~MouseStateParameters();
	};

	// Class for simple time manipulations

	class TimerStateParameters
	{
	public:
		float   GetTime() const;
		float   GetDeltaTime() const;

		void    Update();

		TimerStateParameters();
		~TimerStateParameters();

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> Time;
		std::chrono::duration<float>                                DeltaTime;
	};

	// Simple containers for resources
	struct QueueParameters
	{
		VkQueue m_Handle;
		uint32_t m_FamilyIndex;
	};

	struct SwapchainParameters
	{
		VkSwapchainKHR m_Handle;
		VkFormat m_Format;
		VkExtent2D m_Size;
		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;

		void DestroyResources(VkDevice logicalDevice)
		{
			for (uint32_t i = 0; i < m_ImageViews.size(); ++i)
			{
				DestroyImageView(logicalDevice, m_ImageViews[i]);
			}
			m_ImageViews.clear();

			DestroySwapchain(logicalDevice, m_Handle);
		}
	};

	// Base class for implementing Vulkan Cookbook code samples
	class VulkanSampleBase 
	{
	public:
		VulkanSampleBase();
		virtual  ~VulkanSampleBase();

		virtual bool Initialize(WindowParameters window_parameters) = 0;
		virtual bool Draw() = 0;
		virtual bool Resize() = 0;
		virtual void Deinitialize() = 0;
		virtual void MouseClick(size_t buttonIndex, bool state) final;
		virtual void MouseMove(int x, int y) final;
		virtual void MouseWheel(float distance) final;
		virtual void MouseReset() final;
		virtual void UpdateTime() final;
		virtual bool IsReady() final;

	protected:
		virtual void OnMouseEvent();

		LIBRARY_TYPE m_VulkanLibrary;
		bool m_Ready;
		MouseStateParameters m_MouseState;
		TimerStateParameters m_TimerState;
	};

	// Base class for code samples with default instance, device and swapchain creation

	class VulkanSample : public VulkanSampleBase
	{
	public:
		VkInstance m_Instance;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_LogicalDevice;
		VkSurfaceKHR m_PresentationSurface;
		QueueParameters m_GraphicsQueue;
		QueueParameters m_ComputeQueue;
		QueueParameters m_PresentQueue;
		SwapchainParameters m_Swapchain;
		VkCommandPool m_CommandPool;
		VkPhysicalDeviceMemoryProperties m_PhysicalDeviceMemoryProperties;
		std::vector<VkImage> m_DepthImages;
		std::vector<VkDeviceMemory> m_DepthImagesMemory;
		std::vector<FrameResources> m_FramesResources;
		static uint32_t const m_FramesCount = 3;
		static VkFormat const m_DepthFormat = VK_FORMAT_D16_UNORM;

		virtual bool InitializeVulkan(WindowParameters windowParameters, VkPhysicalDeviceFeatures *desiredDeviceFeatures = nullptr,
			VkImageUsageFlags swapchainImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, bool useDepth = true,
			VkImageUsageFlags depthAttachmentUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) final;
		virtual bool CreateSwapchain(VkImageUsageFlags swapchainImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, bool useDepth = true,
			VkImageUsageFlags depthAttachmentUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) final;
		virtual void  Deinitialize();

	private:
		bool CreateSwapChainCustom(VkImageUsageFlags swapchainImageUsage, VkFormat desireFormat, VkPresentModeKHR desirePresentMode, VkColorSpaceKHR desireColorSpace, 
			VkSwapchainKHR &oldSwapchain);
	};

	// Application starting point implementation

#define VULKAN_SAMPLE_FRAMEWORK( title, x, y, width, height, sampleType )					\
																							\
	int main()																				\
	{																						\
		sampleType sample;																	\
		WindowFramework window( "Vulkan Cookbook #" title, x, y, width, height, sample );	\
																							\
		window.Render();																	\
																							\
		return 0;																			\
	}

}


