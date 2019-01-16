#include "CommandBufferAndSyncFunctions.h"

namespace VulkanSampleFramework
{
	bool CreateCommandPool(VkDevice logicalDevice, VkCommandPoolCreateFlags parameters, uint32_t queueFamily, VkCommandPool &commandPool)
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,		// VkStructureType              sType
			nullptr,										// const void                 * pNext
			parameters,										// VkCommandPoolCreateFlags     flags
			queueFamily										// uint32_t                     queueFamilyIndex
		};

		VkResult result = vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create command pool." << std::endl;
			return false;
		}
		return true;
	}

	bool AllocateCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, VkCommandBufferLevel level, uint32_t count,
		std::vector<VkCommandBuffer> & commandBuffers)
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,		// VkStructureType          sType
			nullptr,											// const void             * pNext
			commandPool,										// VkCommandPool            commandPool
			level,												// VkCommandBufferLevel     level
			count												// uint32_t                 commandBufferCount
		};

		commandBuffers.resize(count);

		VkResult result = vkAllocateCommandBuffers(logicalDevice, &commandBufferAllocateInfo, commandBuffers.data());
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not allocate command buffers." << std::endl;
			return false;
		}
		return true;
	}

	bool BeginCommandBufferRecordingOperation(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usage,
		VkCommandBufferInheritanceInfo * secondaryCommandBufferInfo)
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,		// VkStructureType                        sType
			nullptr,											// const void							  *pNext
			usage,												// VkCommandBufferUsageFlags              flags

			//< If we want to record secondary command buffer ans excute in the render pass, we have to fill this struct
			secondaryCommandBufferInfo							// const VkCommandBufferInheritanceInfo   *pInheritanceInfo 
		};

		VkResult result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
		if (VK_SUCCESS != result) 
		{
			std::cout << "Could not begin command buffer recording operation." << std::endl;
			return false;
		}
		return true;
	}

	bool EndCommandBufferRecordingOperation(VkCommandBuffer commandBuffer)
	{
		VkResult result = vkEndCommandBuffer(commandBuffer);
		if (VK_SUCCESS != result)
		{
			std::cout << "Error occurred during command buffer recording." << std::endl;
			return false;
		}
		return true;
	}

	bool ResetCommandBuffer(VkCommandBuffer commandBuffer, bool releaseResources)
	{
		VkResult result = vkResetCommandBuffer(commandBuffer, releaseResources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
		if (VK_SUCCESS != result)
		{
			std::cout << "Error occurred during command buffer reset." << std::endl;
			return false;
		}
		return true;
	}

	bool ResetCommandPool(VkDevice logicalDevice, VkCommandPool commandPool, bool releaseResources)
	{
		VkResult result = vkResetCommandPool(logicalDevice, commandPool, releaseResources ? VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : 0);
		if (VK_SUCCESS != result)
		{
			std::cout << "Error occurred during command pool reset." << std::endl;
			return false;
		}
		return true;
	}

	bool CreateSemaphore(VkDevice logicalDevice, VkSemaphore &semaphore)
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo =
		{
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,    // VkStructureType            sType
			nullptr,                                    // const void               * pNext
			0                                           // VkSemaphoreCreateFlags     flags
		};

		VkResult result = vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &semaphore);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create a semaphore." << std::endl;
			return false;
		}
		return true;
	}

	bool CreateFence(VkDevice logicalDevice, bool signaled, VkFence &fence)
	{
		VkFenceCreateInfo fenceCreateInfo =
		{
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,			// VkStructureType        sType
			nullptr,										// const void           * pNext
			signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u,	// VkFenceCreateFlags     flags
		};

		VkResult result = vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &fence);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create a fence." << std::endl;
			return false;
		}
		return true;
	}

	bool WaitForFences(VkDevice logicalDevice, std::vector<VkFence> const &fences, VkBool32 waitForAll, uint64_t timeout /*nanosecond*/)
	{
		if (fences.size() > 0)
		{
			VkResult result = vkWaitForFences(logicalDevice, static_cast<uint32_t>(fences.size()), fences.data(), waitForAll, timeout);
			if (VK_SUCCESS != result)
			{
				std::cout << "Waiting on fence failed." << std::endl;
				return false;
			}
			return true;
		}
		return false;
	}

	bool ResetFences(VkDevice logicalDevice, std::vector<VkFence> const & fences)
	{
		if (fences.size() > 0)
		{
			VkResult result = vkResetFences(logicalDevice, static_cast<uint32_t>(fences.size()), fences.data());
			if (VK_SUCCESS != result)
			{
				std::cout << "Error occurred when tried to reset fences." << std::endl;
				return false;
			}
			return VK_SUCCESS == result;
		}
		return false;
	}

	bool SubmitCommandBuffersToQueue(VkQueue queue, std::vector<WaitSemaphoreInfo> waitSemaphoreInfos, std::vector<VkCommandBuffer> commandBuffers,
		std::vector<VkSemaphore> signalSemaphores, VkFence fence)
	{
		std::vector<VkSemaphore>          waitSemaphoreHandles;
		std::vector<VkPipelineStageFlags> waitSemaphoreStages;

		for (auto & waitSemaphoreInfo : waitSemaphoreInfos)
		{
			waitSemaphoreHandles.emplace_back(waitSemaphoreInfo.m_Semaphore);
			waitSemaphoreStages.emplace_back(waitSemaphoreInfo.m_WaitingStage);
		}

		VkSubmitInfo submitInfo =
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,							// VkStructureType                sType
			nullptr,												// const void                   * pNext
			static_cast<uint32_t>(waitSemaphoreInfos.size()),		// uint32_t                       waitSemaphoreCount
			waitSemaphoreHandles.data(),							// const VkSemaphore            * pWaitSemaphores
			waitSemaphoreStages.data(),								// const VkPipelineStageFlags   * pWaitDstStageMask
			static_cast<uint32_t>(commandBuffers.size()),			// uint32_t                       commandBufferCount
			commandBuffers.data(),									// const VkCommandBuffer        * pCommandBuffers
			static_cast<uint32_t>(signalSemaphores.size()),			// uint32_t                       signalSemaphoreCount
			signalSemaphores.data()								// const VkSemaphore            * pSignalSemaphores
		};

		VkResult result = vkQueueSubmit(queue, 1, &submitInfo, fence);
		if (VK_SUCCESS != result)
		{
			std::cout << "Error occurred during command buffer submission." << std::endl;
			return false;
		}
		return true;
	}

	bool WaitUntilAllCommandsSubmittedToQueueAreFinished(VkQueue queue)
	{
		VkResult result = vkQueueWaitIdle(queue);
		if (VK_SUCCESS != result)
		{
			std::cout << "Waiting for all operations submitted to queue failed." << std::endl;
			return false;
		}
		return true;
	}

	bool WaitForAllSubmittedCommandsToBeFinished(VkDevice logicalDevice)
	{
		VkResult result = vkDeviceWaitIdle(logicalDevice);
		if (VK_SUCCESS != result)
		{
			std::cout << "Waiting on a device failed." << std::endl;
			return false;
		}
		return true;
	}

	void DestroyFence(VkDevice logicalDevice, VkFence &fence)
	{
		if (VK_NULL_HANDLE != fence)
		{
			vkDestroyFence(logicalDevice, fence, nullptr);
			fence = VK_NULL_HANDLE;
		}
	}

	void DestroySemaphore(VkDevice logicalDevice, VkSemaphore &semaphore)
	{
		if (VK_NULL_HANDLE != semaphore)
		{
			vkDestroySemaphore(logicalDevice, semaphore, nullptr);
			semaphore = VK_NULL_HANDLE;
		}
	}

	void FreeCommandBuffers(VkDevice logicalDevice, VkCommandPool commandPool, std::vector<VkCommandBuffer> &commandBuffers)
	{
		if (commandBuffers.size() > 0)
		{
			vkFreeCommandBuffers(logicalDevice, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
			commandBuffers.clear();
		}
	}

	void DestroyCommandPool(VkDevice logicalDevice, VkCommandPool &commandPool)
	{
		if (VK_NULL_HANDLE != commandPool)
		{
			vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
			commandPool = VK_NULL_HANDLE;
		}
	}

}

