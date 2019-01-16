#include "CommandBufferAndSyncFunctions.h"
#include "ResourcesAndMemoryFunctions.h"

namespace VulkanSampleFramework
{
	bool CreateBuffer(VkDevice logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer &buffer)
	{
		VkBufferCreateInfo bufferCreateInfo =
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,   // VkStructureType        sType
			nullptr,                                // const void           * pNext
			0,                                      // VkBufferCreateFlags    flags
			size,                                   // VkDeviceSize           size
			usage,                                  // VkBufferUsageFlags     usage

			//< Better performance, but need use memory barrier when buffer have to be used by different queue familiy
			VK_SHARING_MODE_EXCLUSIVE,              // VkSharingMode          sharingMode
			0,                                      // uint32_t               queueFamilyIndexCount
			nullptr                                 // const uint32_t       * pQueueFamilyIndices
		};

		VkResult result = vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, &buffer);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create a buffer." << std::endl;
			return false;
		}
		return true;
	}

	uint32_t CheckMemoryObjectTypeFromBuffer(VkDevice logicalDevice, VkBuffer buffer, VkMemoryPropertyFlagBits memoryProperties,
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, VkMemoryRequirements &memoryRequirements)
	{
		vkGetBufferMemoryRequirements(logicalDevice, buffer, &memoryRequirements);

		for (uint32_t type = 0; type < physicalDeviceMemoryProperties.memoryTypeCount; ++type)
		{
			if ((memoryRequirements.memoryTypeBits & (1 << type)) &&
				((physicalDeviceMemoryProperties.memoryTypes[type].propertyFlags & memoryProperties) == memoryProperties))
			{
				return type;
			}
		}

		std::cout << "No sutiable memory type for buffer" << std::endl;
		return physicalDeviceMemoryProperties.memoryTypeCount;
	}

	uint32_t CheckMemoryObjectTypeFromImage(VkDevice logicalDevice, VkImage image, VkMemoryPropertyFlagBits memoryProperties,
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, VkMemoryRequirements &memoryRequirements)
	{
		vkGetImageMemoryRequirements(logicalDevice, image, &memoryRequirements);

		for (uint32_t type = 0; type < physicalDeviceMemoryProperties.memoryTypeCount; ++type)
		{
			if ((memoryRequirements.memoryTypeBits & (1 << type)) &&
				((physicalDeviceMemoryProperties.memoryTypes[type].propertyFlags & memoryProperties) == memoryProperties))
			{
				return type;
			}
		}

		std::cout << "No sutiable memory type for image" << std::endl;
		return physicalDeviceMemoryProperties.memoryTypeCount;
	}

	bool AllocateMemoryObject(VkDevice logicalDevice, VkDeviceSize size, uint32_t memoryType, VkDeviceMemory &memoryObject)
	{
		VkMemoryAllocateInfo bufferMemoryAllocateInfo =
		{
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,		// VkStructureType    sType
			nullptr,									// const void       * pNext
			size,										// VkDeviceSize       allocationSize
			memoryType									// uint32_t           memoryTypeIndex
		};

		VkResult result = vkAllocateMemory(logicalDevice, &bufferMemoryAllocateInfo, nullptr, &memoryObject);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not allocate memory object." << std::endl;
			return false;
		}
		return true;
	}

	bool BindMemoryObjectToBufer(VkDevice logicalDevice, VkDeviceMemory memoryObject, VkBuffer buffer, uint32_t memoryOffset/*in bytes*/)
	{
		VkResult result = vkBindBufferMemory(logicalDevice, buffer, memoryObject, memoryOffset);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not bind memory object to a buffer." << std::endl;
			return false;
		}
		return true;
	}

	void SetBufferMemoryBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages,
		std::vector<BufferTransition> bufferTransitions)
	{
		std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers;

		for (auto & bufferTransition : bufferTransitions)
		{
			bufferMemoryBarriers.push_back
			(
				{
					VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,		// VkStructureType    sType
					nullptr,										// const void       * pNext
					bufferTransition.m_CurrentAccess,				// VkAccessFlags      srcAccessMask
					bufferTransition.m_NewAccess,					// VkAccessFlags      dstAccessMask
					bufferTransition.m_CurrentQueueFamily,			// uint32_t           srcQueueFamilyIndex
					bufferTransition.m_NewQueueFamily,				// uint32_t           dstQueueFamilyIndex
					bufferTransition.m_Buffer,						// VkBuffer           buffer
					0,												// VkDeviceSize       offset
					VK_WHOLE_SIZE									// VkDeviceSize       size
				}
			);
		}

		if (bufferMemoryBarriers.size() > 0)
		{
			vkCmdPipelineBarrier(commandBuffer, generatingStages, consumingStages, 0, 0, nullptr, static_cast<uint32_t>(bufferMemoryBarriers.size()), bufferMemoryBarriers.data(), 0, nullptr);
		}
	}

	bool AllocateAndBindMemoryObjectToBuffer(VkDevice logicalDevice, VkBuffer buffer, VkMemoryPropertyFlagBits memoryProperties,
		VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties, VkDeviceMemory &memoryObject)
	{
		uint32_t memoryType;
		VkMemoryRequirements memoryRequirements;
		memoryType = CheckMemoryObjectTypeFromBuffer(logicalDevice, buffer, memoryProperties, physicalDeviceMemoryProperties, memoryRequirements);

		if (memoryType >= physicalDeviceMemoryProperties.memoryTypeCount)
		{
			std::cout << "Could not find sutiable memory type." << std::endl;
			return false;
		}

		VkMemoryAllocateInfo bufferMemoryAllocateInfo =
		{
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,			// VkStructureType    sType
			nullptr,										// const void       * pNext
			memoryRequirements.size,						// VkDeviceSize       allocationSize
			memoryType                                      // uint32_t           memoryTypeIndex
		};

		vkAllocateMemory(logicalDevice, &bufferMemoryAllocateInfo, nullptr, &memoryObject);
		if (VK_NULL_HANDLE == memoryObject)
		{
			return false;
		}

		bool result = BindMemoryObjectToBufer(logicalDevice, memoryObject, buffer, 0);
		if (!result)
		{
			return false;
		}

		return true;
	}

	bool CreateBufferView(VkDevice logicalDevice, VkBuffer buffer, VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange, VkBufferView & bufferView)
	{
		VkBufferViewCreateInfo bufferViewCreateInfo =
		{
			VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,		// VkStructureType            sType
			nullptr,										// const void               * pNext
			0,												// VkBufferViewCreateFlags    flags
			buffer,											// VkBuffer                   buffer
			format,											// VkFormat                   format
			memoryOffset,									// VkDeviceSize               offset
			memoryRange										// VkDeviceSize               range
		};

		VkResult result = vkCreateBufferView(logicalDevice, &bufferViewCreateInfo, nullptr, &bufferView);
		if (VK_SUCCESS != result) {
			std::cout << "Could not creat buffer view." << std::endl;
			return false;
		}
		return true;
	}

	bool CreateImage(VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers,
		VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap, VkImage &image)
	{
		VkImageCreateInfo imageCreateInfo =
		{
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,					// VkStructureType          sType
			nullptr,												// const void             * pNext
			cubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0u,		// VkImageCreateFlags       flags
			type,													// VkImageType              imageType
			format,													// VkFormat                 format
			size,													// VkExtent3D               extent
			numMipmaps,												// uint32_t                 mipLevels
			cubemap ? 6 * numLayers : numLayers,					// uint32_t                 arrayLayers
			samples,												// VkSampleCountFlagBits    samples
			VK_IMAGE_TILING_OPTIMAL,								// VkImageTiling            tiling
			usageScenarios,											// VkImageUsageFlags        usage

			// Same as buffer, better performance
			VK_SHARING_MODE_EXCLUSIVE,								// VkSharingMode            sharingMode
			0,														// uint32_t                 queueFamilyIndexCount
			nullptr,												// const uint32_t         * pQueueFamilyIndices
			VK_IMAGE_LAYOUT_UNDEFINED								// VkImageLayout            initialLayout
		};

		VkResult result = vkCreateImage(logicalDevice, &imageCreateInfo, nullptr, &image);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create an image." << std::endl;
			return false;
		}
		return true;
	}

	bool AllocateAndBindMemoryObjectToImage(VkDevice logicalDevice, VkImage image, VkMemoryPropertyFlagBits memoryProperties,
		VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties, VkDeviceMemory &memoryObject)
	{
		uint32_t memoryType;
		VkMemoryRequirements memoryRequirements;
		memoryType = CheckMemoryObjectTypeFromImage(logicalDevice, image, memoryProperties, physicalDeviceMemoryProperties, memoryRequirements);

		
		if (memoryType >= physicalDeviceMemoryProperties.memoryTypeCount)
		{
			std::cout << "Could not find sutiable memory type." << std::endl;
			return false;
		}

		AllocateMemoryObject(logicalDevice, memoryRequirements.size, memoryType, memoryObject);
		if (VK_NULL_HANDLE == memoryObject)
		{
			return false;
		}


		bool result = BindMemoryObjectToImage(logicalDevice, image, memoryObject, 0);
		if (!result)
		{
			return false;
		}

		return true;
	}

	bool BindMemoryObjectToImage(VkDevice logicalDevice, VkImage image, VkDeviceMemory & memoryObject, uint32_t memoryOffset)
	{
		VkResult result = vkBindImageMemory(logicalDevice, image, memoryObject, memoryOffset);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not bind memory object to an image." << std::endl;
			return false;
		}
		return true;
	}

	void SetImageMemoryBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages,
		std::vector<ImageTransition> imageTransitions)
	{
		std::vector<VkImageMemoryBarrier> imageMemoryBarriers;

		for (auto & imageTransition : imageTransitions)
		{
			imageMemoryBarriers.push_back
			(
				{
					VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,		// VkStructureType            sType
					nullptr,									// const void               * pNext
					imageTransition.m_CurrentAccess,			// VkAccessFlags              srcAccessMask
					imageTransition.m_NewAccess,				// VkAccessFlags              dstAccessMask
					imageTransition.m_CurrentLayout,			// VkImageLayout              oldLayout
					imageTransition.m_NewLayout,				// VkImageLayout              newLayout
					imageTransition.m_CurrentQueueFamily,		// uint32_t                   srcQueueFamilyIndex
					imageTransition.m_NewQueueFamily,			// uint32_t                   dstQueueFamilyIndex
					imageTransition.m_Image,					// VkImage                    image
					{											// VkImageSubresourceRange    subresourceRange
						imageTransition.m_Aspect,				// VkImageAspectFlags         aspectMask

						// If we just want to barrier part of image, maybe we should add parameter here
						0,                                      // uint32_t                   baseMipLevel
						VK_REMAINING_MIP_LEVELS,                // uint32_t                   levelCount
						0,                                      // uint32_t                   baseArrayLayer
						VK_REMAINING_ARRAY_LAYERS               // uint32_t                   layerCount
					}
				}
			);
		}

		if (imageMemoryBarriers.size() > 0)
		{
			vkCmdPipelineBarrier(commandBuffer, generatingStages, consumingStages, 0, 0, nullptr, 0, nullptr, static_cast<uint32_t>(imageMemoryBarriers.size()),
				imageMemoryBarriers.data());
		}
	}

	bool CreateImageView(VkDevice logicalDevice, VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect, VkImageView &imageView)
	{
		VkImageViewCreateInfo imageViewCreateInfo =
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,		// VkStructureType            sType
			nullptr,										// const void               * pNext
			0,												// VkImageViewCreateFlags     flags
			image,											// VkImage                    image
			viewType,										// VkImageViewType            viewType
			format,											// VkFormat                   format
			{												// VkComponentMapping         components
				VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle         r
				VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle         g
				VK_COMPONENT_SWIZZLE_IDENTITY,              // VkComponentSwizzle         b
				VK_COMPONENT_SWIZZLE_IDENTITY               // VkComponentSwizzle         a
			},
			{												// VkImageSubresourceRange    subresourceRange
				aspect,                                     // VkImageAspectFlags         aspectMask

				// Just use whole image, if want to set part of image, we should parameterize it
				0,                                          // uint32_t                   baseMipLevel
				VK_REMAINING_MIP_LEVELS,                    // uint32_t                   levelCount
				0,                                          // uint32_t                   baseArrayLayer
				VK_REMAINING_ARRAY_LAYERS                   // uint32_t                   layerCount
			}
		};

		VkResult result = vkCreateImageView(logicalDevice, &imageViewCreateInfo, nullptr, &imageView);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create an image view." << std::endl;
			return false;
		}
		return true;
	}

	bool MapUpdateAndUnmapHostVisibleMemory(VkDevice logicalDevice, VkDeviceMemory memoryObject, VkDeviceSize offset, VkDeviceSize dataSize, void *data,
		bool unmap, void **pointer)
	{
		VkResult result;
		void *localPointer;
		result = vkMapMemory(logicalDevice, memoryObject, offset, dataSize, 0, &localPointer);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not map memory object." << std::endl;
			return false;
		}

		std::memcpy(localPointer, data, static_cast<size_t>(dataSize));

		std::vector<VkMappedMemoryRange> memoryRanges =
		{
			{
				VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,		// VkStructureType    sType
				nullptr,									// const void       * pNext
				memoryObject,								// VkDeviceMemory     memory
				offset,										// VkDeviceSize       offset
				VK_WHOLE_SIZE								// VkDeviceSize       size
			}
		};

		vkFlushMappedMemoryRanges(logicalDevice, static_cast<uint32_t>(memoryRanges.size()), memoryRanges.data());
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not flush mapped memory." << std::endl;
			return false;
		}

		if (unmap)
		{
			vkUnmapMemory(logicalDevice, memoryObject);
		}
		else if (nullptr != pointer)
		{
			*pointer = localPointer;
		}

		return true;
	}

	void CopyDataBetweenBuffers(VkCommandBuffer commandBuffer, VkBuffer sourceBuffer, VkBuffer destinationBuffer, std::vector<VkBufferCopy> regions)
	{
		if (regions.size() > 0)
		{
			vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, static_cast<uint32_t>(regions.size()), regions.data());
		}
	}

	void CopyDataFromBufferToImage(VkCommandBuffer commandBuffer, VkBuffer sourceBuffer, VkImage destinationImage, VkImageLayout imageLayout,
		std::vector<VkBufferImageCopy> regions)
	{
		if (regions.size() > 0)
		{
			vkCmdCopyBufferToImage(commandBuffer, sourceBuffer, destinationImage, imageLayout, static_cast<uint32_t>(regions.size()), regions.data());
		}
	}

	void CopyDataFromImageToBuffer(VkCommandBuffer commandBuffer, VkImage sourceImage, VkImageLayout imageLayout, VkBuffer destinationBuffer,
		std::vector<VkBufferImageCopy> regions)
	{
		if (regions.size() > 0)
		{
			vkCmdCopyImageToBuffer(commandBuffer, sourceImage, imageLayout, destinationBuffer, static_cast<uint32_t>(regions.size()), regions.data());
		}
	}

	bool UseStagingBufferToUpdateBufferWithDeviceLocalMemoryBound(VkDevice logicalDevice, VkDeviceSize dataSize, void *data, VkBuffer destinationBuffer,
		VkDeviceSize destinationOffset, VkAccessFlags destinationBufferCurrentAccess, VkAccessFlags destinationBufferNewAccess,
		VkPipelineStageFlags destinationBufferGeneratingStages, VkPipelineStageFlags destinationBufferConsumingStages, VkQueue queue, VkCommandBuffer commandBuffer,
		std::vector<VkSemaphore> signalSemaphores, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties)
	{
		VkBuffer stagingBuffer;
		if (!CreateBuffer(logicalDevice, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer))
		{
			return false;
		}

		uint32_t memoryType;
		VkMemoryRequirements memoryRequirements;
		memoryType = CheckMemoryObjectTypeFromBuffer(logicalDevice, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, physicalDeviceMemoryProperties,
			memoryRequirements);

		if (memoryType >= physicalDeviceMemoryProperties.memoryTypeCount)
		{
			return false;
		}

		VkDeviceMemory memoryObject;
		if (!AllocateMemoryObject(logicalDevice, memoryRequirements.size, memoryType, memoryObject))
		{
			return false;
		}

		if (!BindMemoryObjectToBufer(logicalDevice, memoryObject, stagingBuffer, 0))
		{
			return false;
		}

		if (!MapUpdateAndUnmapHostVisibleMemory(logicalDevice, memoryObject, 0, dataSize, data, true, nullptr))
		{
			return false;
		}

		if (!BeginCommandBufferRecordingOperation(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr))
		{
			return false;
		}

		SetBufferMemoryBarrier(commandBuffer, destinationBufferGeneratingStages, VK_PIPELINE_STAGE_TRANSFER_BIT,
			{ {destinationBuffer, destinationBufferCurrentAccess, VK_ACCESS_TRANSFER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED} });

		CopyDataBetweenBuffers(commandBuffer, stagingBuffer, destinationBuffer, { {0, destinationOffset, dataSize} });

		SetBufferMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, destinationBufferConsumingStages,
			{ {destinationBuffer, VK_ACCESS_TRANSFER_WRITE_BIT, destinationBufferNewAccess, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED} });

		if (!EndCommandBufferRecordingOperation(commandBuffer))
		{
			return false;
		}

		VkFence fence;
		if (!CreateFence(logicalDevice, false, fence)) {
			return false;
		}

		if (!SubmitCommandBuffersToQueue(queue, {}, { commandBuffer }, signalSemaphores, fence))
		{
			return false;
		}

		if (!WaitForFences(logicalDevice, { fence }, VK_FALSE, 200000000))
		{
			return false;
		}

		FreeMemoryObject(logicalDevice, memoryObject);
		DestroyBuffer(logicalDevice, stagingBuffer);
		DestroyFence(logicalDevice, fence);

		return true;
	}

	bool UseStagingBufferToUpdateImageWithDeviceLocalMemoryBound(VkDevice logicalDevice, VkDeviceSize dataSize, void *data, VkImage destinationImage,
		VkImageSubresourceLayers destinationImageSubresource, VkOffset3D destinationImageOffset, VkExtent3D destinationImageSize,
		VkImageLayout destinationImageCurrentLayout, VkImageLayout destinationImageNewLayout, VkAccessFlags destinationImageCurrentAccess,
		VkAccessFlags destinationImageNewAccess, VkImageAspectFlags destinationImageAspect, VkPipelineStageFlags destinationImageGeneratingStages,
		VkPipelineStageFlags destinationImageConsumingStages, VkQueue queue, VkCommandBuffer commandBuffer, std::vector<VkSemaphore> signalSemaphores,
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties)
	{
		VkBuffer stagingBuffer;
		if (!CreateBuffer(logicalDevice, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer))
		{
			return false;
		}

		uint32_t memoryType;
		VkMemoryRequirements memoryRequirements;
		memoryType = CheckMemoryObjectTypeFromBuffer(logicalDevice, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, physicalDeviceMemoryProperties,
			memoryRequirements);

		if (memoryType >= physicalDeviceMemoryProperties.memoryTypeCount)
		{
			return false;
		}

		VkDeviceMemory memoryObject;
		if (!AllocateMemoryObject(logicalDevice, memoryRequirements.size, memoryType, memoryObject))
		{
			return false;
		}

		if (!BindMemoryObjectToBufer(logicalDevice, memoryObject, stagingBuffer, 0))
		{
			return false;
		}

		if (!MapUpdateAndUnmapHostVisibleMemory(logicalDevice, memoryObject, 0, dataSize, data, true, nullptr))
		{
			return false;
		}

		if (!BeginCommandBufferRecordingOperation(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr))
		{
			return false;
		}

		SetImageMemoryBarrier(commandBuffer, destinationImageGeneratingStages, VK_PIPELINE_STAGE_TRANSFER_BIT,
			{
				{
					destinationImage,							// VkImage            Image
					destinationImageCurrentAccess,				// VkAccessFlags      CurrentAccess
					VK_ACCESS_TRANSFER_WRITE_BIT,				// VkAccessFlags      NewAccess
					destinationImageCurrentLayout,				// VkImageLayout      CurrentLayout
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,		// VkImageLayout      NewLayout
					VK_QUEUE_FAMILY_IGNORED,					// uint32_t           CurrentQueueFamily
					VK_QUEUE_FAMILY_IGNORED,					// uint32_t           NewQueueFamily
					destinationImageAspect						// VkImageAspectFlags Aspect
				} 
			});

		CopyDataFromBufferToImage(commandBuffer, stagingBuffer, destinationImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			{
				{
					0,											// VkDeviceSize               bufferOffset
					0,											// uint32_t                   bufferRowLength
					0,											// uint32_t                   bufferImageHeight
					destinationImageSubresource,				// VkImageSubresourceLayers   imageSubresource
					destinationImageOffset,						// VkOffset3D                 imageOffset
					destinationImageSize,						// VkExtent3D                 imageExtent
				} 
			});

		SetImageMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, destinationImageConsumingStages,
			{
				{
					destinationImage,							// VkImage            Image
					VK_ACCESS_TRANSFER_WRITE_BIT,				// VkAccessFlags      CurrentAccess
					destinationImageNewAccess,					// VkAccessFlags      NewAccess
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,		// VkImageLayout      CurrentLayout
					destinationImageNewLayout,					// VkImageLayout      NewLayout
					VK_QUEUE_FAMILY_IGNORED,					// uint32_t           CurrentQueueFamily
					VK_QUEUE_FAMILY_IGNORED,					// uint32_t           NewQueueFamily
					destinationImageAspect						// VkImageAspectFlags Aspect
				} 
			});

		if (!EndCommandBufferRecordingOperation(commandBuffer))
		{
			return false;
		}

		VkFence fence;
		if (!CreateFence(logicalDevice, false, fence))
		{
			return false;
		}

		if (!SubmitCommandBuffersToQueue(queue, {}, { commandBuffer }, signalSemaphores, fence))
		{
			return false;
		}

		if (!WaitForFences(logicalDevice, {fence}, VK_FALSE, 500000000))
		{
			return false;
		}

		FreeMemoryObject(logicalDevice, memoryObject);
		DestroyBuffer(logicalDevice, stagingBuffer);
		DestroyFence(logicalDevice, fence);

		return true;
	}

	void DestroyImageView(VkDevice logicalDevice, VkImageView &imageView)
	{
		if (VK_NULL_HANDLE != imageView)
		{
			vkDestroyImageView(logicalDevice, imageView, nullptr);
			imageView = VK_NULL_HANDLE;
		}
	}

	void DestroyImage(VkDevice logicalDevice, VkImage  &image)
	{
		if (VK_NULL_HANDLE != image)
		{
			vkDestroyImage(logicalDevice, image, nullptr);
			image = VK_NULL_HANDLE;
		}
	}

	void DestroyBufferView(VkDevice logicalDevice, VkBufferView & bufferView)
	{
		if (VK_NULL_HANDLE != bufferView)
		{
			vkDestroyBufferView(logicalDevice, bufferView, nullptr);
			bufferView = VK_NULL_HANDLE;
		}
	}

	void DestroyBuffer(VkDevice logicalDevice, VkBuffer &buffer)
	{
		if (VK_NULL_HANDLE != buffer)
		{
			vkDestroyBuffer(logicalDevice, buffer, nullptr);
			buffer = VK_NULL_HANDLE;
		}
	}

	void FreeMemoryObject(VkDevice logicalDevice, VkDeviceMemory &memoryObject)
	{
		if (VK_NULL_HANDLE != memoryObject)
		{
			vkFreeMemory(logicalDevice, memoryObject, nullptr);
			memoryObject = VK_NULL_HANDLE;
		}
	}
}

