#pragma once
#include "../CommonFiles/Common.h"

namespace VulkanSampleFramework
{
	struct ImageDescriptorInfo {
		VkDescriptorSet m_TargetDescriptorSet;
		uint32_t m_TargetDescriptorBinding;
		uint32_t m_TargetArrayElement;
		VkDescriptorType m_TargetDescriptorType;
		std::vector<VkDescriptorImageInfo> m_ImageInfos;
	};

	struct BufferDescriptorInfo
	{
		VkDescriptorSet m_TargetDescriptorSet;
		uint32_t m_TargetDescriptorBinding;
		uint32_t m_TargetArrayElement;
		VkDescriptorType m_TargetDescriptorType;
		std::vector<VkDescriptorBufferInfo> m_BufferInfos;
	};

	struct TexelBufferDescriptorInfo
	{
		VkDescriptorSet m_TargetDescriptorSet;
		uint32_t m_TargetDescriptorBinding;
		uint32_t m_TargetArrayElement;
		VkDescriptorType m_TargetDescriptorType;
		std::vector<VkBufferView> m_TexelBufferViews;
	};

	struct CopyDescriptorInfo
	{
		VkDescriptorSet m_TargetDescriptorSet;
		uint32_t m_TargetDescriptorBinding;
		uint32_t m_TargetArrayElement;
		VkDescriptorSet m_SourceDescriptorSet;
		uint32_t m_SourceDescriptorBinding;
		uint32_t m_SourceArrayElement;
		uint32_t m_DescriptorCount;
	};

	bool CreateSampler(VkDevice logicalDevice, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode,
		VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, bool anisotropyEnable, float maxAnisotropy, bool compareEnable,
		VkCompareOp compareOperator, float minLod, float maxLod, VkBorderColor borderColor, bool unnormalizedXoords, VkSampler &sampler);
	bool CreateSampledImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers,
		VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, bool linearFiltering, VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties,
		VkImage &sampledImage, VkDeviceMemory &memoryObject, VkImageView &sampledImageView);
	bool CreateStorageImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers,
		VkImageUsageFlags usage, VkImageViewType viewType, VkImageAspectFlags aspect, bool atomicOperations, VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties, 
		VkImage &storageImage, VkDeviceMemory &memoryObject, VkImageView &storageImageView);
	bool CreateDescriptorSetLayout(VkDevice logicalDevice, std::vector<VkDescriptorSetLayoutBinding> const &bindings, VkDescriptorSetLayout &descriptorSetLayout);
	bool CreateDescriptorPool(VkDevice logicalDevice, bool freeIndividualSets, uint32_t maxSetsCount, std::vector<VkDescriptorPoolSize> const &descriptorTypes,
		VkDescriptorPool &descriptorPool);
	bool AllocateDescriptorSets(VkDevice logicalDevice, VkDescriptorPool descriptorPool, std::vector<VkDescriptorSetLayout> const &descriptorSetLayouts,
		std::vector<VkDescriptorSet> &descriptorSets);
	void UpdateDescriptorSets(VkDevice logicalDevice, std::vector<ImageDescriptorInfo> const &imageDescriptorInfos,
		std::vector<BufferDescriptorInfo> const &bufferDescriptorInfos, std::vector<TexelBufferDescriptorInfo> const &texelBufferDescriptorInfos,
		std::vector<CopyDescriptorInfo> const &copyDescriptorInfos);
	void BindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineType, VkPipelineLayout pipelineLayout, uint32_t indexForFirstSet,
		std::vector<VkDescriptorSet> const &descriptorSets, std::vector<uint32_t> const &dynamicOffsets);
	bool FreeDescriptorSets(VkDevice logicalDvice, VkDescriptorPool descriptorPool, std::vector<VkDescriptorSet> & descriptorSets);
	bool ResetDescriptorPool(VkDevice logicalDevice, VkDescriptorPool descriptorPool);
	void DestroyDescriptorPool(VkDevice logicalDevice, VkDescriptorPool & descriptorPool);
	void DestroyDescriptorSetLayout(VkDevice logicalDevice, VkDescriptorSetLayout & descriptorSetLayout);
	void DestroySampler(VkDevice logicalDevice, VkSampler &sampler);
}
