#pragma once
#include "../CommonFiles/Common.h"
#include "CommandBufferAndSyncFunctions.h"
#include "ImagePresentFunctions.h"
#include "RenderPassAndFramebufferFunctions.h"
#include "ResourcesAndMemoryFunctions.h"

namespace VulkanSampleFramework
{
	struct VertexBufferParameters
	{
		VkBuffer      m_Buffer;
		VkDeviceSize  m_MemoryOffset;
	};

	struct CommandBufferRecordingThreadParameters
	{
		VkCommandBuffer m_CommandBuffer;
		std::function<bool(VkCommandBuffer)> m_RecordingFunction;
	};

	struct FrameResources
	{
		// All resource should be controled in this structur
		VkCommandBuffer             m_CommandBuffer;					
		VkSemaphore					m_ImageAcquiredSemaphore;
		VkSemaphore					m_ReadyToPresentSemaphore;
		VkFence						m_DrawingFinishedFence;
		VkImageView					m_DepthAttachment;
		VkFramebuffer				m_Framebuffer;

		FrameResources()
		{
		}

		FrameResources(FrameResources && other)
		{
			*this = std::move(other);
		}

		FrameResources& operator=(FrameResources && other)
		{
			if (this != &other)
			{
				VkCommandBuffer commandBuffer = m_CommandBuffer;

				m_CommandBuffer = other.m_CommandBuffer;
				other.m_CommandBuffer = commandBuffer;
				m_ImageAcquiredSemaphore = std::move(other.m_ImageAcquiredSemaphore);
				m_ReadyToPresentSemaphore = std::move(other.m_ReadyToPresentSemaphore);
				m_DrawingFinishedFence = std::move(other.m_DrawingFinishedFence);
				m_DepthAttachment = std::move(other.m_DepthAttachment);
				m_Framebuffer = std::move(other.m_Framebuffer);
			}
			return *this;
		}

		FrameResources(FrameResources const &) = delete;
		FrameResources& operator=(FrameResources const &) = delete;

		// Except depth attachment and framebuffer, that must be create in different place
		bool Initiallize(VkDevice logicalDevice, VkCommandPool commandPool)
		{
			std::vector<VkCommandBuffer> commandBuffers;
			if (!AllocateCommandBuffers(logicalDevice, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, commandBuffers))
			{
				return false;
			}
			m_CommandBuffer = commandBuffers[0];

			if (!CreateSemaphore(logicalDevice, m_ImageAcquiredSemaphore))
			{
				return false;
			}

			if (!CreateSemaphore(logicalDevice, m_ReadyToPresentSemaphore))
			{
				return false;
			}

			if (!CreateFence(logicalDevice, true, m_DrawingFinishedFence))
			{
				return false;
			}

			m_DepthAttachment = VK_NULL_HANDLE;
			m_Framebuffer = VK_NULL_HANDLE;

			return true;
		}

		// Because I move the resource to this struct, so I have to free this by call destroy function, except command buffer
		void Destroy(VkDevice logicalDevice)
		{
			m_CommandBuffer = VK_NULL_HANDLE;
			DestroySemaphore(logicalDevice, m_ImageAcquiredSemaphore);
			DestroySemaphore(logicalDevice, m_ReadyToPresentSemaphore);
			DestroyFence(logicalDevice, m_DrawingFinishedFence);
			DestroyImageView(logicalDevice, m_DepthAttachment);
			DestroyFramebuffer(logicalDevice, m_Framebuffer);
		}
	};


	void ClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
		std::vector<VkImageSubresourceRange> const &image_subresource_ranges, VkClearColorValue &clearColor);
	void ClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
		std::vector<VkImageSubresourceRange> const &imageSubresourceRanges, VkClearDepthStencilValue &clearValue);
	void ClearRenderPassAttachments(VkCommandBuffer commandBuffer, std::vector<VkClearAttachment> const &attachments, std::vector<VkClearRect> const &rects);
	void BindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, std::vector<VertexBufferParameters> const &buffersParameters);
	void BindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize memoryOffset, VkIndexType indexType);
	void ProvideDataToShadersThroughPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkShaderStageFlags pipelineStages,
		uint32_t offset, uint32_t size, void *data);
	void SetViewportStateDynamically(VkCommandBuffer commandBuffer, uint32_t firstViewport, std::vector<VkViewport> const &viewports);
	void SetScissorStateDynamically(VkCommandBuffer commandBuffer, uint32_t firstScissor, std::vector<VkRect2D> const &scissors);
	void SetLineWidthStateDynamically(VkCommandBuffer commandBuffer, float lineWidth);
	void SetDepthBiasStateDynamically(VkCommandBuffer commandBuffer, float constantFactor, float clamp, float slopeFactor);
	void SetBlendConstantsStateDynamically(VkCommandBuffer commandBuffer, std::array<float, 4> const &blendConstants);
	void DrawGeometry(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
	void DrawIndexedGeometry(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset,
		uint32_t firstInstance);
	void DispatchComputeWork(VkCommandBuffer commandBuffer, uint32_t xSize, uint32_t ySize, uint32_t zSize);
	void ExecuteSecondaryCommandBufferInsidePrimaryCommandBuffer(VkCommandBuffer commandBuffer, std::vector<VkCommandBuffer> const &secondaryCommandBuffers);
	bool RecordCommandBuffersOnMultipleThreads(std::vector<CommandBufferRecordingThreadParameters> const &threadsParameters, VkQueue queue,
		std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkSemaphore> signalSemaphores, VkFence fence);
	bool PrepareSingleFrameOfAnimation(VkDevice logicalDevice, VkQueue graphicsQueue, VkQueue presentQueue, VkSwapchainKHR swapchain, VkExtent2D swapchainSize,
		std::vector<VkImageView> const &swapchainImageViews, VkImageView depthAttachment, std::vector<WaitSemaphoreInfo> const &waitInfos,
		VkSemaphore imageAcquiredSemaphore, VkSemaphore readyToPresentSemaphore, VkFence finishedDrawingFence,
		std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> recordCommandBuffer, VkCommandBuffer commandBuffer, VkRenderPass renderPass,
		VkFramebuffer &framebuffer);
	bool IncreasePerformanceThroughIncreasingTheNumberOfSeparatelyRenderedFrames(VkDevice logicalDevice, VkQueue graphicsQueue, VkQueue presentQueue,
		VkSwapchainKHR swapchain, VkExtent2D swapchainSize, std::vector<VkImageView> const &swapchainImageViews, VkRenderPass renderPass,
		std::vector<WaitSemaphoreInfo> const &waitInfos, std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> recordCommandBuffer,
		std::vector<FrameResources> &frameResources);
}