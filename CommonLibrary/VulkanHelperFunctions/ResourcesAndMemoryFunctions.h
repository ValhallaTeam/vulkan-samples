#pragma once
#include "../CommonFiles/Common.h"

namespace VulkanSampleFramework
{

	struct BufferTransition
	{
		VkBuffer        m_Buffer;
		VkAccessFlags   m_CurrentAccess;
		VkAccessFlags   m_NewAccess;
		uint32_t        m_CurrentQueueFamily;
		uint32_t        m_NewQueueFamily;
	};

	struct ImageTransition
	{
		VkImage             m_Image;
		VkAccessFlags       m_CurrentAccess;
		VkAccessFlags       m_NewAccess;
		VkImageLayout       m_CurrentLayout;
		VkImageLayout       m_NewLayout;
		uint32_t            m_CurrentQueueFamily;
		uint32_t            m_NewQueueFamily;
		VkImageAspectFlags  m_Aspect;
	};

	bool CreateBuffer(VkDevice logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer &buffer);

	//< allocate a memory object that suitable for buffer memory type, that should be managed by memory object manager
	uint32_t CheckMemoryObjectTypeFromBuffer(VkDevice logicalDevice, VkBuffer buffer, VkMemoryPropertyFlagBits memoryProperties,
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, VkMemoryRequirements &memoryRequirements);
	uint32_t CheckMemoryObjectTypeFromImage(VkDevice logicalDevice, VkImage image, VkMemoryPropertyFlagBits memoryProperties,
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, VkMemoryRequirements &memoryRequirements);
	bool AllocateMemoryObject(VkDevice logicalDevice, VkDeviceSize size, uint32_t memoryType, VkDeviceMemory &memoryObject);
	bool BindMemoryObjectToBufer(VkDevice logicalDevice, VkDeviceMemory memoryObject, VkBuffer buffer, uint32_t memoryOffset /*in bytes*/);
	void SetBufferMemoryBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages,
		std::vector<BufferTransition> bufferTransitions);
	bool AllocateAndBindMemoryObjectToBuffer(VkDevice logicalDevice, VkBuffer buffer, VkMemoryPropertyFlagBits memoryProperties,
		VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties, VkDeviceMemory &memoryObject);
	bool CreateBufferView(VkDevice logicalDevice, VkBuffer buffer, VkFormat format, VkDeviceSize memoryOffset, VkDeviceSize memoryRange, VkBufferView & bufferView);
	bool CreateImage(VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers,
		VkSampleCountFlagBits samples, VkImageUsageFlags usageScenarios, bool cubemap, VkImage &image);
	bool AllocateAndBindMemoryObjectToImage(VkDevice logicalDevice, VkImage image, VkMemoryPropertyFlagBits memoryProperties,
		VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties, VkDeviceMemory &memoryObject);
	bool BindMemoryObjectToImage(VkDevice logicalDevice, VkImage image, VkDeviceMemory & memoryObject, uint32_t memoryOffset);
	void SetImageMemoryBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages,
		std::vector<ImageTransition> imageTransitions);
	bool CreateImageView(VkDevice logicalDevice, VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlags aspect, VkImageView &imageView);
	
	// Now I just flush whole range of memory to update.
	bool MapUpdateAndUnmapHostVisibleMemory(VkDevice logicalDevice, VkDeviceMemory memoryObject, VkDeviceSize offset, VkDeviceSize dataSize, void *data,
		bool unmap, void **pointer);
	void CopyDataBetweenBuffers(VkCommandBuffer commandBuffer, VkBuffer sourceBuffer, VkBuffer destinationBuffer, std::vector<VkBufferCopy> regions);
	void CopyDataFromBufferToImage(VkCommandBuffer commandBuffer, VkBuffer sourceBuffer, VkImage destinationImage, VkImageLayout imageLayout,
		std::vector<VkBufferImageCopy> regions);
	void CopyDataFromImageToBuffer(VkCommandBuffer commandBuffer, VkImage sourceImage, VkImageLayout imageLayout, VkBuffer destinationBuffer,
		std::vector<VkBufferImageCopy> regions);
	bool UseStagingBufferToUpdateBufferWithDeviceLocalMemoryBound(VkDevice logicalDevice, VkDeviceSize dataSize, void *data, VkBuffer destinationBuffer,
		VkDeviceSize destinationOfffset, VkAccessFlags destinationBufferCurrentAccess, VkAccessFlags destinationBufferNewAccess,
		VkPipelineStageFlags destinationBufferGeneratingStages, VkPipelineStageFlags destinationBufferConsumingStages, VkQueue queue, VkCommandBuffer commandBuffer,
		std::vector<VkSemaphore> signalSemaphores, VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties);
	bool UseStagingBufferToUpdateImageWithDeviceLocalMemoryBound(VkDevice logicalDevice, VkDeviceSize dataSize, void *data, VkImage destinationImage,
		VkImageSubresourceLayers destinationImageSubresource, VkOffset3D destinationImageOffset, VkExtent3D destinationImageSize,
		VkImageLayout destinationImageCurrentLayout, VkImageLayout destinationImageNewLayout, VkAccessFlags destinationImageCurrentAccess,
		VkAccessFlags destinationImageNewAccess, VkImageAspectFlags destinationImageAspect, VkPipelineStageFlags destinationImageGeneratingStages,
		VkPipelineStageFlags destinationImageConsumingStages, VkQueue queue, VkCommandBuffer commandBuffer, std::vector<VkSemaphore> signalSemaphores,
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties);
	void DestroyImageView(VkDevice logicalDevice, VkImageView &imageView);
	void DestroyImage(VkDevice logicalDevice, VkImage  &image);
	void DestroyBufferView(VkDevice logicalDevice, VkBufferView & bufferView);
	void DestroyBuffer(VkDevice logicalDevice, VkBuffer &buffer);
	void FreeMemoryObject(VkDevice logicalDevice, VkDeviceMemory &memoryObject);
}