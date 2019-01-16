#include "CommandRecordingAndDrawing.h"

namespace VulkanSampleFramework
{
	void ClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
		std::vector<VkImageSubresourceRange> const &imageSubresourceRanges, VkClearColorValue &clearColor)
	{
		vkCmdClearColorImage(commandBuffer, image, imageLayout, &clearColor, static_cast<uint32_t>(imageSubresourceRanges.size()), imageSubresourceRanges.data());
	}

	void ClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
		std::vector<VkImageSubresourceRange> const &imageSubresourceRanges, VkClearDepthStencilValue &clearValue)
	{
		vkCmdClearDepthStencilImage(commandBuffer, image, imageLayout, &clearValue, static_cast<uint32_t>(imageSubresourceRanges.size()),
			imageSubresourceRanges.data());
	}

	void ClearRenderPassAttachments(VkCommandBuffer commandBuffer, std::vector<VkClearAttachment> const &attachments, std::vector<VkClearRect> const &rects)
	{
		vkCmdClearAttachments(commandBuffer, static_cast<uint32_t>(attachments.size()), attachments.data(), static_cast<uint32_t>(rects.size()), rects.data());
	}

	void BindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, std::vector<VertexBufferParameters> const &buffersParameters)
	{
		if (buffersParameters.size() > 0)
		{
			std::vector<VkBuffer> buffers;
			std::vector<VkDeviceSize> offsets;
			for (auto & bufferParameters : buffersParameters)
			{
				buffers.push_back(bufferParameters.m_Buffer);
				offsets.push_back(bufferParameters.m_MemoryOffset);
			}
			vkCmdBindVertexBuffers(commandBuffer, firstBinding, static_cast<uint32_t>(buffersParameters.size()), buffers.data(), offsets.data());
		}
	}

	void BindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize memoryOffset, VkIndexType indexType)
	{
		vkCmdBindIndexBuffer(commandBuffer, buffer, memoryOffset, indexType);
	}

	void ProvideDataToShadersThroughPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VkShaderStageFlags pipelineStages,
		uint32_t offset, uint32_t size, void *data)
	{
		vkCmdPushConstants(commandBuffer, pipelineLayout, pipelineStages, offset, size, data);
	}

	void SetViewportStateDynamically(VkCommandBuffer commandBuffer, uint32_t firstViewport, std::vector<VkViewport> const &viewports)
	{
		vkCmdSetViewport(commandBuffer, firstViewport, static_cast<uint32_t>(viewports.size()), viewports.data());
	}

	void SetScissorStateDynamically(VkCommandBuffer commandBuffer, uint32_t firstScissor, std::vector<VkRect2D> const &scissors)
	{
		vkCmdSetScissor(commandBuffer, firstScissor, static_cast<uint32_t>(scissors.size()), scissors.data());
	}

	void SetLineWidthStateDynamically(VkCommandBuffer commandBuffer, float lineWidth)
	{
		vkCmdSetLineWidth(commandBuffer, lineWidth);
	}

	void SetDepthBiasStateDynamically(VkCommandBuffer commandBuffer, float constantFactor, float clamp, float slopeFactor)
	{
		vkCmdSetDepthBias(commandBuffer, constantFactor, clamp, slopeFactor);
	}

	void SetBlendConstantsStateDynamically(VkCommandBuffer commandBuffer, std::array<float, 4> const &blendConstants)
	{
		vkCmdSetBlendConstants(commandBuffer, blendConstants.data());
	}

	void DrawGeometry(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void DrawIndexedGeometry(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset,
		uint32_t firstInstance)
	{
		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void DispatchComputeWork(VkCommandBuffer commandBuffer, uint32_t xSize, uint32_t ySize, uint32_t zSize)
	{
		vkCmdDispatch(commandBuffer, xSize, ySize, zSize);
	}

	void ExecuteSecondaryCommandBufferInsidePrimaryCommandBuffer(VkCommandBuffer commandBuffer, std::vector<VkCommandBuffer> const &secondaryCommandBuffers)
	{
		if (secondaryCommandBuffers.size() > 0)
		{
			vkCmdExecuteCommands(commandBuffer, static_cast<uint32_t>(secondaryCommandBuffers.size()), secondaryCommandBuffers.data());
		}
	}

	bool RecordCommandBuffersOnMultipleThreads(std::vector<CommandBufferRecordingThreadParameters> const &threadsParameters, VkQueue queue,
		std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkSemaphore> signalSemaphores, VkFence fence)
	{
		std::vector<std::thread> threads(threadsParameters.size());
		for (size_t i = 0; i < threadsParameters.size(); ++i)
		{
			threads[i] = std::thread(threadsParameters[i].m_RecordingFunction, threadsParameters[i].m_CommandBuffer);
		}

		std::vector<VkCommandBuffer> commandBuffers(threadsParameters.size());
		for (size_t i = 0; i < threadsParameters.size(); ++i)
		{
			threads[i].join();
			commandBuffers[i] = threadsParameters[i].m_CommandBuffer;
		}

		return true;
	}

	bool PrepareSingleFrameOfAnimation(VkDevice logicalDevice, VkQueue graphicsQueue, VkQueue presentQueue, VkSwapchainKHR swapchain, VkExtent2D swapchainSize,
		std::vector<VkImageView> const &swapchainImageViews, VkImageView depthAttachment, std::vector<WaitSemaphoreInfo> const &waitInfos,
		VkSemaphore imageAcquiredSemaphore, VkSemaphore readyToPresentSemaphore, VkFence finishedDrawingFence,
		std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> recordCommandBuffer, VkCommandBuffer commandBuffer, VkRenderPass renderPass,
		VkFramebuffer &framebuffer)

	{
		uint32_t imageIndex;
		if (!AcquireSwapchainImage(logicalDevice, swapchain, imageAcquiredSemaphore, VK_NULL_HANDLE, imageIndex))
		{
			return false;
		}

		std::vector<VkImageView> attachments = { swapchainImageViews[imageIndex] };
		if (VK_NULL_HANDLE != depthAttachment)
		{
			attachments.push_back(depthAttachment);
		}
		
		if (!CreateFramebuffer(logicalDevice, renderPass, attachments, swapchainSize.width, swapchainSize.height, 1, framebuffer))
		{
			return false;
		}

		if (!recordCommandBuffer(commandBuffer, imageIndex, framebuffer))
		{
			return false;
		}

		std::vector<WaitSemaphoreInfo> waitSemaphoreInfos = waitInfos;
		waitSemaphoreInfos.push_back
		(
			{
				imageAcquiredSemaphore,								// VkSemaphore            Semaphore
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT		// VkPipelineStageFlags   WaitingStage
			}
		);

		if (!SubmitCommandBuffersToQueue(graphicsQueue, waitSemaphoreInfos, { commandBuffer }, { readyToPresentSemaphore }, finishedDrawingFence))
		{
			return false;
		}

		PresentInfo presentInfo =
		{
			swapchain,										// VkSwapchainKHR         Swapchain
			imageIndex										// uint32_t               ImageIndex
		};

		if (!PresentImage(presentQueue, { readyToPresentSemaphore }, { presentInfo }))
		{
			return false;
		}

		return true;
	}

	bool IncreasePerformanceThroughIncreasingTheNumberOfSeparatelyRenderedFrames(VkDevice logicalDevice, VkQueue graphicsQueue, VkQueue presentQueue,
		VkSwapchainKHR swapchain, VkExtent2D swapchainSize, std::vector<VkImageView> const &swapchainImageViews, VkRenderPass renderPass,
		std::vector<WaitSemaphoreInfo> const &waitInfos, std::function<bool(VkCommandBuffer, uint32_t, VkFramebuffer)> recordCommandBuffer,
		std::vector<FrameResources> &frameResources)
	{
		static uint32_t frameIndex = 0;
		FrameResources & currentFrame = frameResources[frameIndex];

		if (!WaitForFences(logicalDevice, {currentFrame.m_DrawingFinishedFence}, false, 2000000000))
		{
			return false;
		}

		if (!ResetFences(logicalDevice, {currentFrame.m_DrawingFinishedFence}))
		{
			return false;
		}

		if (!PrepareSingleFrameOfAnimation(logicalDevice, graphicsQueue, presentQueue, swapchain, swapchainSize, swapchainImageViews,
			currentFrame.m_DepthAttachment, waitInfos, currentFrame.m_ImageAcquiredSemaphore, currentFrame.m_ReadyToPresentSemaphore,
			currentFrame.m_DrawingFinishedFence, recordCommandBuffer, currentFrame.m_CommandBuffer, renderPass, currentFrame.m_Framebuffer))
		{
			return false;
		}

		frameIndex = (frameIndex + 1) % frameResources.size();
		return true;
	}
}