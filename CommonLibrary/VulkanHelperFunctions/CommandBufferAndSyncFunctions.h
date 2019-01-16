#pragma once
#include "../CommonFiles/Common.h"

namespace VulkanSampleFramework
{
	struct WaitSemaphoreInfo
	{
		VkSemaphore           m_Semaphore;
		VkPipelineStageFlags  m_WaitingStage;
	};

	bool CreateCommandPool(VkDevice logicalDevice, VkCommandPoolCreateFlags parameters, uint32_t queueFamily, VkCommandPool &commandPool);
	bool AllocateCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, VkCommandBufferLevel level, uint32_t count,
		std::vector<VkCommandBuffer> & commandBuffers);
	bool BeginCommandBufferRecordingOperation(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usage,
		VkCommandBufferInheritanceInfo * secondaryCommandBufferInfo);
	bool EndCommandBufferRecordingOperation(VkCommandBuffer commandBuffer);
	bool ResetCommandBuffer(VkCommandBuffer commandBuffer, bool releaseResources);
	bool ResetCommandPool(VkDevice logicalDevice, VkCommandPool commandPool, bool releaseResources);
	bool CreateSemaphore(VkDevice logicalDevice, VkSemaphore &semaphore);
	bool CreateFence(VkDevice logicalDevice, bool signaled, VkFence &fence);
	bool WaitForFences(VkDevice logicalDevice, std::vector<VkFence> const &fences, VkBool32 waitForAll, uint64_t timeout /*nanosecond*/);
	bool ResetFences(VkDevice logicalDevice, std::vector<VkFence> const & fences);
	bool SubmitCommandBuffersToQueue(VkQueue queue, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkCommandBuffer> commandBuffers,
		std::vector<VkSemaphore> signalSemaphores, VkFence fence);
	bool WaitUntilAllCommandsSubmittedToQueueAreFinished(VkQueue queue);
	bool WaitForAllSubmittedCommandsToBeFinished(VkDevice logicalDevice);
	void DestroyFence(VkDevice logicalDevice, VkFence &fence);
	void DestroySemaphore(VkDevice logicalDevice, VkSemaphore &semaphore);
	void FreeCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, std::vector<VkCommandBuffer> &commandBuffers);
	void DestroyCommandPool(VkDevice logicalDevice, VkCommandPool &commandPool);
}
