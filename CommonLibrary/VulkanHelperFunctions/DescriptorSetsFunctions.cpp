#include "DescriptorSetsFunctions.h"
#include "ResourcesAndMemoryFunctions.h"

namespace VulkanSampleFramework
{
	bool CreateSampler(VkDevice logicalDevice, VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode uAddressMode,
		VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode, float lodBias, bool anisotropyEnable, float maxAnisotropy, bool compareEnable,
		VkCompareOp compareOperator, float minLod, float maxLod, VkBorderColor borderColor, bool unnormalizedXoords, VkSampler &sampler)
	{
		VkSamplerCreateInfo samplerCreateInfo =
		{
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,		// VkStructureType          sType
			nullptr,									// const void             * pNext
			0,											// VkSamplerCreateFlags     flags
			magFilter,									// VkFilter                 magFilter
			minFilter,									// VkFilter                 minFilter
			mipmapMode,									// VkSamplerMipmapMode      mipmapMode
			uAddressMode,								// VkSamplerAddressMode     addressModeU
			vAddressMode,								// VkSamplerAddressMode     addressModeV
			wAddressMode,								// VkSamplerAddressMode     addressModeW
			lodBias,									// float                    mipLodBias
			anisotropyEnable,							// VkBool32                 anisotropyEnable
			maxAnisotropy,								// float                    maxAnisotropy
			compareEnable,								// VkBool32                 compareEnable
			compareOperator,							// VkCompareOp              compareOp
			minLod,										// float                    minLod
			maxLod,										// float                    maxLod
			borderColor,								// VkBorderColor            borderColor
			unnormalizedXoords							// VkBool32                 unnormalizedCoordinates
		};

		VkResult result = vkCreateSampler(logicalDevice, &samplerCreateInfo, nullptr, &sampler);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create sampler." << std::endl;
			return false;
		}
		return true;
	}

	bool CreateSampledImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers,
		VkImageUsageFlags usage, bool cubemap, VkImageViewType viewType, VkImageAspectFlags aspect, bool linearFiltering, VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties,
		VkImage &sampledImage, VkDeviceMemory &memoryObject, VkImageView &sampledImageView)
	{
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
		{
			std::cout << "Provided format is not supported for a sampled image." << std::endl;
			return false;
		}
		if (linearFiltering && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) 
		{
			std::cout << "Provided format is not supported for a linear image filtering." << std::endl;
			return false;
		}

		if (!CreateImage(logicalDevice, type, format, size, numMipmaps, numLayers, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_SAMPLED_BIT, cubemap, sampledImage)) {
			return false;
		}

		if (!AllocateAndBindMemoryObjectToImage(logicalDevice, sampledImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, physicalDeviceMemoryProperties, memoryObject))
		{
			return false;
		}

		if (!CreateImageView(logicalDevice, sampledImage, viewType, format, aspect, sampledImageView))
		{
			return false;
		}
		return true;
	}

	bool CreateStorageImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImageType type, VkFormat format, VkExtent3D size, uint32_t numMipmaps, uint32_t numLayers,
		VkImageUsageFlags usage,VkImageViewType viewType, VkImageAspectFlags aspect, bool atomicOperations, VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties, 
		VkImage &storageImage, VkDeviceMemory &memoryObject, VkImageView &storageImageView)
	{
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT))
		{
			std::cout << "Provided format is not supported for a storage image." << std::endl;
			return false;
		}
		if (atomicOperations &&	!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT)) 
		{
			std::cout << "Provided format is not supported for atomic operations on storage images." << std::endl;
			return false;
		}

		if (!CreateImage(logicalDevice, type, format, size, numMipmaps, numLayers, VK_SAMPLE_COUNT_1_BIT, usage | VK_IMAGE_USAGE_STORAGE_BIT, false, storageImage))
		{
			return false;
		}

		if (!AllocateAndBindMemoryObjectToImage(logicalDevice, storageImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, physicalDeviceMemoryProperties, memoryObject))
		{
			return false;
		}

		if (!CreateImageView(logicalDevice, storageImage, viewType, format, aspect, storageImageView)) 
		{
			return false;
		}
		return true;
	}

	bool CreateDescriptorSetLayout(VkDevice logicalDevice, std::vector<VkDescriptorSetLayoutBinding> const &bindings, VkDescriptorSetLayout &descriptorSetLayout)
	{
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = 
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,	// VkStructureType                      sType
			nullptr,												// const void                         * pNext
			0,														// VkDescriptorSetLayoutCreateFlags     flags
			static_cast<uint32_t>(bindings.size()),					// uint32_t                             bindingCount
			bindings.data()											// const VkDescriptorSetLayoutBinding * pBindings
		};

		VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create a layout for descriptor sets." << std::endl;
			return false;
		}
		return true;
	}

	bool CreateDescriptorPool(VkDevice logicalDevice, bool freeIndividualSets, uint32_t maxSetsCount, std::vector<VkDescriptorPoolSize> const &descriptorTypes,
		VkDescriptorPool &descriptorPool)
	{
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo =
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,													// VkStructureType                sType
			nullptr,																						// const void                   * pNext
			freeIndividualSets ? VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0u,					// VkDescriptorPoolCreateFlags    flags
			maxSetsCount,																					// uint32_t                       maxSets
			static_cast<uint32_t>(descriptorTypes.size()),													// uint32_t                       poolSizeCount
			descriptorTypes.data()																			// const VkDescriptorPoolSize   * pPoolSizes
		};

		VkResult result = vkCreateDescriptorPool(logicalDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create a descriptor pool." << std::endl;
			return false;
		}
		return true;
	}

	bool AllocateDescriptorSets(VkDevice logicalDevice, VkDescriptorPool descriptorPool, std::vector<VkDescriptorSetLayout> const &descriptorSetLayouts,
		std::vector<VkDescriptorSet> &descriptorSets)
	{
		if (descriptorSetLayouts.size() > 0)
		{
			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo =
			{
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,				// VkStructureType                  sType
				nullptr,													// const void                     * pNext
				descriptorPool,												// VkDescriptorPool                 descriptorPool
				static_cast<uint32_t>(descriptorSetLayouts.size()),			// uint32_t                         descriptorSetCount
				descriptorSetLayouts.data()									// const VkDescriptorSetLayout    * pSetLayouts
			};

			descriptorSets.resize(descriptorSetLayouts.size());

			VkResult result = vkAllocateDescriptorSets(logicalDevice, &descriptorSetAllocateInfo, descriptorSets.data());
			if (VK_SUCCESS != result)
			{
				std::cout << "Could not allocate descriptor sets." << std::endl;
				return false;
			}
			return true;
		}
		return false;
	}

	void UpdateDescriptorSets(VkDevice logicalDevice, std::vector<ImageDescriptorInfo> const &imageDescriptorInfos,
		std::vector<BufferDescriptorInfo> const &bufferDescriptorInfos, std::vector<TexelBufferDescriptorInfo> const &texelBufferDescriptorInfos,
		std::vector<CopyDescriptorInfo> const &copyDescriptorInfos)
	{
		std::vector<VkWriteDescriptorSet> writeDescriptors;
		std::vector<VkCopyDescriptorSet> copyDescriptors;

		// image descriptors
		for (auto & imageDescriptor : imageDescriptorInfos)
		{
			writeDescriptors.push_back(
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
					nullptr,                                                                // const void                     * pNext
					imageDescriptor.m_TargetDescriptorSet,                                  // VkDescriptorSet                  dstSet
					imageDescriptor.m_TargetDescriptorBinding,                              // uint32_t                         dstBinding
					imageDescriptor.m_TargetArrayElement,                                   // uint32_t                         dstArrayElement
					static_cast<uint32_t>(imageDescriptor.m_ImageInfos.size()),             // uint32_t                         descriptorCount
					imageDescriptor.m_TargetDescriptorType,                                 // VkDescriptorType                 descriptorType
					imageDescriptor.m_ImageInfos.data(),                                    // const VkDescriptorImageInfo    * pImageInfo
					nullptr,                                                                // const VkDescriptorBufferInfo   * pBufferInfo
					nullptr                                                                 // const VkBufferView             * pTexelBufferView
				});
		}

		// buffer descriptors
		for (auto &bufferDescriptor : bufferDescriptorInfos)
		{
			writeDescriptors.push_back(
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,									// VkStructureType                  sType
					nullptr,                                                                // const void                     * pNext
					bufferDescriptor.m_TargetDescriptorSet,                                 // VkDescriptorSet                  dstSet
					bufferDescriptor.m_TargetDescriptorBinding,                             // uint32_t                         dstBinding
					bufferDescriptor.m_TargetArrayElement,                                  // uint32_t                         dstArrayElement
					static_cast<uint32_t>(bufferDescriptor.m_BufferInfos.size()),           // uint32_t                         descriptorCount
					bufferDescriptor.m_TargetDescriptorType,                                // VkDescriptorType                 descriptorType
					nullptr,                                                                // const VkDescriptorImageInfo    * pImageInfo
					bufferDescriptor.m_BufferInfos.data(),                                  // const VkDescriptorBufferInfo   * pBufferInfo
					nullptr                                                                 // const VkBufferView             * pTexelBufferView
				});
		}

		// texel buffer descriptors
		for (auto & texelBufferDescriptor : texelBufferDescriptorInfos)
		{
			writeDescriptors.push_back(
				{
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,                                 // VkStructureType                  sType
					nullptr,                                                                // const void                     * pNext
					texelBufferDescriptor.m_TargetDescriptorSet,                            // VkDescriptorSet                  dstSet
					texelBufferDescriptor.m_TargetDescriptorBinding,                        // uint32_t                         dstBinding
					texelBufferDescriptor.m_TargetArrayElement,                             // uint32_t                         dstArrayElement
					static_cast<uint32_t>(texelBufferDescriptor.m_TexelBufferViews.size()), // uint32_t                         descriptorCount
					texelBufferDescriptor.m_TargetDescriptorType,                           // VkDescriptorType                 descriptorType
					nullptr,                                                                // const VkDescriptorImageInfo    * pImageInfo
					nullptr,                                                                // const VkDescriptorBufferInfo   * pBufferInfo
					texelBufferDescriptor.m_TexelBufferViews.data()                         // const VkBufferView             * pTexelBufferView
				});
		}

		// copy descriptors
		for (auto & copyDescriptor : copyDescriptorInfos)
		{
			copyDescriptors.push_back(
				{
					VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,										// VkStructureType    sType
					nullptr,																	// const void       * pNext
					copyDescriptor.m_SourceDescriptorSet,										// VkDescriptorSet    srcSet
					copyDescriptor.m_SourceDescriptorBinding,									// uint32_t           srcBinding
					copyDescriptor.m_SourceArrayElement,										// uint32_t           srcArrayElement
					copyDescriptor.m_TargetDescriptorSet,										// VkDescriptorSet    dstSet
					copyDescriptor.m_TargetDescriptorBinding,									// uint32_t           dstBinding
					copyDescriptor.m_TargetArrayElement,										// uint32_t           dstArrayElement
					copyDescriptor.m_DescriptorCount											// uint32_t           descriptorCount
				});
		}

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(writeDescriptors.size()), writeDescriptors.data(), static_cast<uint32_t>(copyDescriptors.size()), copyDescriptors.data());
	}

	void BindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineType, VkPipelineLayout pipelineLayout, uint32_t indexForFirstSet,
		std::vector<VkDescriptorSet> const &descriptorSets, std::vector<uint32_t> const &dynamicOffsets)
	{
		vkCmdBindDescriptorSets(commandBuffer, pipelineType, pipelineLayout, indexForFirstSet, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(),
			static_cast<uint32_t>(dynamicOffsets.size()), dynamicOffsets.data());
	}

	bool FreeDescriptorSets(VkDevice logicalDvice, VkDescriptorPool descriptorPool, std::vector<VkDescriptorSet> & descriptorSets)
	{
		if (descriptorSets.size() > 0)
		{
			VkResult result = vkFreeDescriptorSets(logicalDvice, descriptorPool, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data());
			descriptorSets.clear();
			if (VK_SUCCESS != result)
			{
				std::cout << "Error occurred during freeing descriptor sets." << std::endl;
				return false;
			}
		}
		return true;
	}

	bool ResetDescriptorPool(VkDevice logicalDevice, VkDescriptorPool descriptorPool)
	{
		VkResult result = vkResetDescriptorPool(logicalDevice, descriptorPool, 0);
		if (VK_SUCCESS != result)
		{
			std::cout << "Error occurred during descriptor pool reset." << std::endl;
			return false;
		}
		return true;
	}
	void DestroyDescriptorPool(VkDevice logicalDevice, VkDescriptorPool & descriptorPool)
	{
		if (VK_NULL_HANDLE != descriptorPool)
		{
			vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
			descriptorPool = VK_NULL_HANDLE;
		}
	}

	void DestroyDescriptorSetLayout(VkDevice logicalDevice, VkDescriptorSetLayout & descriptorSetLayout)
	{
		if (VK_NULL_HANDLE != descriptorSetLayout)
		{
			vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);
			descriptorSetLayout = VK_NULL_HANDLE;
		}
	}

	void DestroySampler(VkDevice logicalDevice, VkSampler &sampler)
	{
		if (VK_NULL_HANDLE != sampler)
		{
			vkDestroySampler(logicalDevice, sampler, nullptr);
			sampler = VK_NULL_HANDLE;
		}
	}



}