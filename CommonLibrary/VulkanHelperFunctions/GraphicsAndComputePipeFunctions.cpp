#include "GraphicsAndComputePipeFunctions.h"

namespace VulkanSampleFramework
{
	bool CreateShaderModule(VkDevice logicalDevice, std::vector<unsigned char> const &sourceCode, VkShaderModule &shaderModule)
	{
		VkShaderModuleCreateInfo shaderModuleCreateInfo =
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,				// VkStructureType              sType
			nullptr,													// const void                 * pNext
			0,															// VkShaderModuleCreateFlags    flags
			sourceCode.size(),											// size_t                       codeSize
			reinterpret_cast<uint32_t const *>(sourceCode.data())		// const uint32_t             * pCode
		};

		VkResult result = vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create a shader module." << std::endl;
			return false;
		}
		return true;
	}

	void SpecifyPipelineShaderStages(std::vector<ShaderStageParameters> const &shaderStageParams, std::vector<VkPipelineShaderStageCreateInfo> & shaderStageCreateInfos)
	{
		shaderStageCreateInfos.clear();
		for (auto & shaderStage : shaderStageParams)
		{
			shaderStageCreateInfos.push_back
			(
				{
					VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,	// VkStructureType                    sType
					nullptr,												// const void                       * pNext
					0,														// VkPipelineShaderStageCreateFlags   flags
					shaderStage.m_ShaderStage,								// VkShaderStageFlagBits              stage
					shaderStage.m_ShaderModule,								// VkShaderModule                     module
					shaderStage.m_EntryPointName,							// const char                       * pName
					shaderStage.m_SpecializationInfo						// const VkSpecializationInfo       * pSpecializationInfo
				}
			);
		}
	}

	void SpecifyPipelineVertexInputState(std::vector<VkVertexInputBindingDescription> const &bindingDescriptions,
		std::vector<VkVertexInputAttributeDescription> const & attributeDescriptions, VkPipelineVertexInputStateCreateInfo &vertexInputStateCreateInfo)
	{
		vertexInputStateCreateInfo = 
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,		// VkStructureType                           sType
			nullptr,														// const void                              * pNext
			0,																// VkPipelineVertexInputStateCreateFlags     flags
			static_cast<uint32_t>(bindingDescriptions.size()),				// uint32_t                                  vertexBindingDescriptionCount
			bindingDescriptions.data(),										// const VkVertexInputBindingDescription   * pVertexBindingDescriptions
			static_cast<uint32_t>(attributeDescriptions.size()),			// uint32_t                                  vertexAttributeDescriptionCount
			attributeDescriptions.data()									// const VkVertexInputAttributeDescription * pVertexAttributeDescriptions
		};
	}

	void SpecifyPipelineInputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnable,
		VkPipelineInputAssemblyStateCreateInfo & inputAssemblyStateCreateInfo)
	{
		inputAssemblyStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,	// VkStructureType                           sType
			nullptr,														// const void                              * pNext
			0,																// VkPipelineInputAssemblyStateCreateFlags   flags
			topology,														// VkPrimitiveTopology                       topology
			primitiveRestartEnable											// VkBool32                                  primitiveRestartEnable
		};
	}

	void SpecifyPipelineTessellationState(uint32_t patchControlPintsCount, VkPipelineTessellationStateCreateInfo &tessellationStateCreateInfo)
	{
		tessellationStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,		// VkStructureType                            sType
			nullptr,														// const void                               * pNext
			0,																// VkPipelineTessellationStateCreateFlags     flags
			patchControlPintsCount											// uint32_t                                   patchControlPoints
		};
	}

	void SpecifyPipelineViewportAndScissorTestState(ViewportInfo const &viewportInfos, VkPipelineViewportStateCreateInfo &viewportStateCreateInfo)
	{
		uint32_t viewportCount = static_cast<uint32_t>(viewportInfos.m_Viewports.size());
		uint32_t scissorCount = static_cast<uint32_t>(viewportInfos.m_Scissors.size());
		
		assert(viewportCount == scissorCount);

		viewportStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,		// VkStructureType                      sType
			nullptr,													// const void                         * pNext
			0,															// VkPipelineViewportStateCreateFlags   flags
			viewportCount,												// uint32_t                             viewportCount
			viewportInfos.m_Viewports.data(),							// const VkViewport                   * pViewports
			scissorCount,												// uint32_t                             scissorCount
			viewportInfos.m_Scissors.data()								// const VkRect2D                     * pScissors
		};
	}

	void SpecifyPipelineRasterizationState(bool depthClampEnable, bool rasterizerDiscardEnable, VkPolygonMode polygonMode, VkCullModeFlags cullingMode,
		VkFrontFace frontFace, bool depthBiasEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, float lineWidth,
		VkPipelineRasterizationStateCreateInfo & rasterizationStateCreateInfo)
	{
		rasterizationStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,		// VkStructureType                            sType
			nullptr,														// const void                               * pNext
			0,																// VkPipelineRasterizationStateCreateFlags    flags
			depthClampEnable,												// VkBool32                                   depthClampEnable
			rasterizerDiscardEnable,										// VkBool32                                   rasterizerDiscardEnable
			polygonMode,													// VkPolygonMode                              polygonMode
			cullingMode,													// VkCullModeFlags                            cullMode
			frontFace,														// VkFrontFace                                frontFace
			depthBiasEnable,												// VkBool32                                   depthBiasEnable
			depthBiasConstantFactor,										// float                                      depthBiasConstantFactor
			depthBiasClamp,													// float                                      depthBiasClamp
			depthBiasSlopeFactor,											// float                                      depthBiasSlopeFactor
			lineWidth														// float                                      lineWidth
		};
	}

	void SpecifyPipelineMultisampleState(VkSampleCountFlagBits sampleCount, bool perSampleShadingEnable, float minSampleShading, VkSampleMask const *sampleMasks,
		bool alphaToCoverageEnable, bool alphaToOneEnable, VkPipelineMultisampleStateCreateInfo & multisampleStateCreateInfo)
	{
		multisampleStateCreateInfo = 
		{
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,	// VkStructureType                          sType
			nullptr,													// const void                             * pNext
			0,															// VkPipelineMultisampleStateCreateFlags    flags
			sampleCount,												// VkSampleCountFlagBits                    rasterizationSamples
			perSampleShadingEnable,										// VkBool32                                 sampleShadingEnable
			minSampleShading,											// float                                    minSampleShading
			sampleMasks,												// const VkSampleMask                     * pSampleMask
			alphaToCoverageEnable,										// VkBool32                                 alphaToCoverageEnable
			alphaToOneEnable											// VkBool32                                 alphaToOneEnable
		};
	}

	void SpecifyPipelineDepthAndStencilState(bool depthTestEnable, bool depthWriteEnable, VkCompareOp depthCompareOp, bool depthBoundsTestEnable,
		float minDepthBounds, float maxDepthBounds, bool stencilTestEnable, VkStencilOpState frontStencilTestParameters, VkStencilOpState backStencilTestParameters,
		VkPipelineDepthStencilStateCreateInfo & depthAndStencilStateCreateInfo)
	{
		depthAndStencilStateCreateInfo = 
		{
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,		// VkStructureType                            sType
			nullptr,														// const void                               * pNext
			0,																// VkPipelineDepthStencilStateCreateFlags     flags
			depthTestEnable,												// VkBool32                                   depthTestEnable
			depthWriteEnable,												// VkBool32                                   depthWriteEnable
			depthCompareOp,													// VkCompareOp                                depthCompareOp
			depthBoundsTestEnable,											// VkBool32                                   depthBoundsTestEnable
			stencilTestEnable,												// VkBool32                                   stencilTestEnable
			frontStencilTestParameters,										// VkStencilOpState                           front
			backStencilTestParameters,										// VkStencilOpState                           back
			minDepthBounds,													// float                                      minDepthBounds
			maxDepthBounds													// float                                      maxDepthBounds
		};
	}

	void SpecifyPipelineBlendState(bool logicOpEnable, VkLogicOp logicOp, std::vector<VkPipelineColorBlendAttachmentState> const & attachmentBlendStates,
		std::array<float, 4> const &blendConstants, VkPipelineColorBlendStateCreateInfo &blendStateCreateInfo)
	{
		blendStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,		// VkStructureType                              sType
			nullptr,														// const void                                 * pNext
			0,																// VkPipelineColorBlendStateCreateFlags         flags
			logicOpEnable,													// VkBool32                                     logicOpEnable
			logicOp,														// VkLogicOp                                    logicOp
			static_cast<uint32_t>(attachmentBlendStates.size()),			// uint32_t                                     attachmentCount
			attachmentBlendStates.data(),									// const VkPipelineColorBlendAttachmentState  * pAttachments
			{																// float                                        blendConstants[4]
				blendConstants[0],
				blendConstants[1],
				blendConstants[2],
				blendConstants[3]
			}
		};
	}

	void SpecifyPipelineDynamicStates(std::vector<VkDynamicState> const &dynamicStates, VkPipelineDynamicStateCreateInfo  &dynamicStateCreatInfo)
	{
		dynamicStateCreatInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,     // VkStructureType                      sType
			nullptr,                                                  // const void                         * pNext
			0,                                                        // VkPipelineDynamicStateCreateFlags    flags
			static_cast<uint32_t>(dynamicStates.size()),             // uint32_t                             dynamicStateCount
			dynamicStates.data()                                     // const VkDynamicState               * pDynamicStates
		};
	}

	bool CreatePipelineLayout(VkDevice logicalDevice, std::vector<VkDescriptorSetLayout> const &descriptorSetLayouts,
		std::vector<VkPushConstantRange> const &pushConstantRanges, VkPipelineLayout &pipelineLayout)
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = 
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,				// VkStructureType                  sType
			nullptr,													// const void                     * pNext
			0,															// VkPipelineLayoutCreateFlags      flags
			static_cast<uint32_t>(descriptorSetLayouts.size()),			// uint32_t                         setLayoutCount
			descriptorSetLayouts.data(),								// const VkDescriptorSetLayout    * pSetLayouts
			static_cast<uint32_t>(pushConstantRanges.size()),			// uint32_t                         pushConstantRangeCount
			pushConstantRanges.data()									// const VkPushConstantRange      * pPushConstantRanges
		};

		VkResult result = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);

		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create pipeline layout." << std::endl;
			return false;
		}
		return true;
	}

	void SpecifyGraphicsPipelineCreationParameters(VkPipelineCreateFlags additionalOptions,
		std::vector<VkPipelineShaderStageCreateInfo> const &shaderStageCreateInfos, VkPipelineVertexInputStateCreateInfo const &vertexInputStateCreateInfo,
		VkPipelineInputAssemblyStateCreateInfo const &inputAssemblyStateCreateInfo, VkPipelineTessellationStateCreateInfo const *tessellationStateCreateInfo,
		VkPipelineViewportStateCreateInfo const *viewportStateCreateInfo, VkPipelineRasterizationStateCreateInfo const &rasterizationStateCreateInfo,
		VkPipelineMultisampleStateCreateInfo const *multisampleStateCreateInfo, VkPipelineDepthStencilStateCreateInfo const *depthAndStencilStateCreateInfo,
		VkPipelineColorBlendStateCreateInfo const * blendStateCreateInfo, VkPipelineDynamicStateCreateInfo const *dynamicStateCreatInfo,
		VkPipelineLayout pipelineLayout, VkRenderPass renderPass, uint32_t subpass, VkPipeline  basePipelineHandle, int32_t basePipelineIndex,
		VkGraphicsPipelineCreateInfo &graphicsPipelineCreateInfo)
	{
		graphicsPipelineCreateInfo =
		{
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,				// VkStructureType                                sType
			nullptr,														// const void                                   * pNext
			additionalOptions,												// VkPipelineCreateFlags                          flags
			static_cast<uint32_t>(shaderStageCreateInfos.size()),			// uint32_t                                       stageCount
			shaderStageCreateInfos.data(),									// const VkPipelineShaderStageCreateInfo        * pStages
			&vertexInputStateCreateInfo,									// const VkPipelineVertexInputStateCreateInfo   * pVertexInputState
			&inputAssemblyStateCreateInfo,									// const VkPipelineInputAssemblyStateCreateInfo * pInputAssemblyState
			tessellationStateCreateInfo,									// const VkPipelineTessellationStateCreateInfo  * pTessellationState
			viewportStateCreateInfo,										// const VkPipelineViewportStateCreateInfo      * pViewportState
			&rasterizationStateCreateInfo,									// const VkPipelineRasterizationStateCreateInfo * pRasterizationState
			multisampleStateCreateInfo,										// const VkPipelineMultisampleStateCreateInfo   * pMultisampleState
			depthAndStencilStateCreateInfo,									// const VkPipelineDepthStencilStateCreateInfo  * pDepthStencilState
			blendStateCreateInfo,											// const VkPipelineColorBlendStateCreateInfo    * pColorBlendState
			dynamicStateCreatInfo,											// const VkPipelineDynamicStateCreateInfo       * pDynamicState
			pipelineLayout,													// VkPipelineLayout                               layout
			renderPass,														// VkRenderPass                                   renderPass
			subpass,															// uint32_t                                       subpass
			basePipelineHandle,												// VkPipeline                                     basePipelineHandle
			basePipelineIndex												// int32_t                                        basePipelineIndex
		};
	}

	bool CreatePipelineCacheObject(VkDevice logicalDevice, std::vector<unsigned char> const &cacheData, VkPipelineCache &pipelineCache)
	{
		VkPipelineCacheCreateInfo pipeline_cache_create_info =
		{
			VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,		// VkStructureType                sType
			nullptr,											// const void                   * pNext
			0,													// VkPipelineCacheCreateFlags     flags
			static_cast<uint32_t>(cacheData.size()),			// size_t                         initialDataSize
			cacheData.data()									// const void                   * pInitialData
		};

		VkResult result = vkCreatePipelineCache(logicalDevice, &pipeline_cache_create_info, nullptr, &pipelineCache);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create pipeline cache." << std::endl;
			return false;
		}
		return true;
	}

	bool RetrieveDataFromPipelineCache(VkDevice logicalDevice, VkPipelineCache pipelineCache, std::vector<unsigned char> &pipelineCacheData)
	{
		size_t dataSize = 0;
		VkResult result = VK_SUCCESS;

		result = vkGetPipelineCacheData(logicalDevice, pipelineCache, &dataSize, nullptr);
		if ((VK_SUCCESS != result) || (0 == dataSize))
		{
			std::cout << "Could not get the size of the pipeline cache." << std::endl;
			return false;
		}

		pipelineCacheData.resize(dataSize);
		result = vkGetPipelineCacheData(logicalDevice, pipelineCache, &dataSize, pipelineCacheData.data());
		if ((VK_SUCCESS != result) || (0 == dataSize))
		{
			std::cout << "Could not acquire pipeline cache data." << std::endl;
			return false;
		}

		return true;
	}

	bool MergeMultiplePipelineCacheObjects(VkDevice logicalDevice, VkPipelineCache targetPipelineCache, std::vector<VkPipelineCache> const &sourcePipelineCaches)
	{
		if (sourcePipelineCaches.size() > 0)
		{
			VkResult result = vkMergePipelineCaches(logicalDevice, targetPipelineCache, static_cast<uint32_t>(sourcePipelineCaches.size()), sourcePipelineCaches.data());
			if (VK_SUCCESS != result)
			{
				std::cout << "Could not merge pipeline cache objects." << std::endl;
				return false;
			}
			return true;
		}
		return false;
	}

	bool CreateGraphicsPipelines(VkDevice logicalDevice, std::vector<VkGraphicsPipelineCreateInfo> const &graphicsPipelineCreateInfos, VkPipelineCache pipelineCache,
		std::vector<VkPipeline>  &graphicsPipelines)
	{
		if (graphicsPipelineCreateInfos.size() > 0)
		{
			graphicsPipelines.resize(graphicsPipelineCreateInfos.size());
			VkResult result = vkCreateGraphicsPipelines(logicalDevice, pipelineCache, static_cast<uint32_t>(graphicsPipelineCreateInfos.size()),
				graphicsPipelineCreateInfos.data(), nullptr, graphicsPipelines.data());
			if (VK_SUCCESS != result)
			{
				std::cout << "Could not create a graphics pipeline." << std::endl;
				return false;
			}
			return true;
		}
		return false;
	}

	void CreateComputePiplineInfo(VkPipelineCreateFlags additionalOptions, VkPipelineShaderStageCreateInfo const &computeShaderStage,
		VkPipelineLayout pipelineLayout, VkPipeline basePipelineHandle, int32_t basePipelineIndex, VkComputePipelineCreateInfo &computePipelineCreateInfo)
	{
		computePipelineCreateInfo =
		{
			VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,		// VkStructureType                    sType
			nullptr,											// const void                       * pNext
			additionalOptions,									// VkPipelineCreateFlags              flags
			computeShaderStage,									// VkPipelineShaderStageCreateInfo    stage
			pipelineLayout,										// VkPipelineLayout                   layout
			basePipelineHandle,									// VkPipeline                         basePipelineHandle
			basePipelineIndex									// int32_t                            basePipelineIndex
		};
	}

	bool CreateComputePipeline(VkDevice logicalDevice, std::vector<VkComputePipelineCreateInfo> const &computePipelineCreateInfos, VkPipelineCache pipelineCache,
		VkPipeline &computePipeline)
	{

		VkResult result = vkCreateComputePipelines(logicalDevice, pipelineCache, static_cast<uint32_t>(computePipelineCreateInfos.size()),
			computePipelineCreateInfos.data(), nullptr, &computePipeline);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create compute pipeline." << std::endl;
			return false;
		}
		return true;
	}

	void BindPipelineObject(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineType, VkPipeline pipeline)
	{
		vkCmdBindPipeline(commandBuffer, pipelineType, pipeline);
	}

	bool CreateMultipleGraphicsPipelinesOnMultipleThreads(VkDevice logicalDevice, std::string const &pipelineCacheFilename,
		std::vector<std::vector<VkGraphicsPipelineCreateInfo>> const & graphicsPipelinesCreateInfos, std::vector<std::vector<VkPipeline>> & graphicsPipelines)
	{
		// Load cache from file (if available)
		std::vector<unsigned char> cacheData;
		GetBinaryFileContents(pipelineCacheFilename, cacheData);

		// Create cache for each thread, initialize its contents with data loaded from file
		std::vector<VkPipelineCache> pipelineCaches(graphicsPipelinesCreateInfos.size());
		for (size_t i = 0; i < graphicsPipelinesCreateInfos.size(); ++i)
		{
			pipelineCaches[i] = VkPipelineCache();
			if (!CreatePipelineCacheObject(logicalDevice, cacheData, pipelineCaches[i]))
			{
				return false;
			}
		}

		// Create multiple threads, where each thread creates multiple pipelines using its own cache
		std::vector<std::thread> threads(graphicsPipelinesCreateInfos.size());
		for (size_t i = 0; i < graphicsPipelinesCreateInfos.size(); ++i)
		{
			graphicsPipelines[i].resize(graphicsPipelinesCreateInfos[i].size());
			threads[i] = std::thread(CreateGraphicsPipelines, logicalDevice, std::ref(graphicsPipelinesCreateInfos[i]), pipelineCaches[i], std::ref(graphicsPipelines[i]));
		}

		// Wait for all threads to finish
		for (size_t i = 0; i < graphicsPipelinesCreateInfos.size(); ++i)
		{
			threads[i].join();
		}

		// Merge all the caches into one, retrieve its contents and store them in the file
		VkPipelineCache targetCache = pipelineCaches.back();
		pipelineCaches.pop_back();

		if (!MergeMultiplePipelineCacheObjects(logicalDevice, targetCache, pipelineCaches))
		{
			return false;
		}

		if (!RetrieveDataFromPipelineCache(logicalDevice, targetCache, cacheData))
		{
			return false;
		}

		if (!SaveBinaryFileContents(pipelineCacheFilename, cacheData))
		{
			return false;
		}

		// Destroy all temp pipeline cach object
		DestroyPipelineCache(logicalDevice, targetCache);
		for (size_t i = 0; i < pipelineCaches.size(); ++i)
		{
			DestroyPipelineCache(logicalDevice, pipelineCaches[i]);
		}
		pipelineCaches.clear();

		return true;
	}

	void DestroyPipeline(VkDevice logicalDevice, VkPipeline & pipeline)
	{
		if (VK_NULL_HANDLE != pipeline)
		{
			vkDestroyPipeline(logicalDevice, pipeline, nullptr);
			pipeline = VK_NULL_HANDLE;
		}
	}

	void DestroyPipelineCache(VkDevice logicalDevice, VkPipelineCache &pipelineCache)
	{
		if (VK_NULL_HANDLE != pipelineCache)
		{
			vkDestroyPipelineCache(logicalDevice, pipelineCache, nullptr);
			pipelineCache = VK_NULL_HANDLE;
		}
	}

	void DestroyPipelineLayout(VkDevice logicalDevice, VkPipelineLayout & pipelineLayout)
	{
		if (VK_NULL_HANDLE != pipelineLayout)
		{
			vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
			pipelineLayout = VK_NULL_HANDLE;
		}
	}

	void DestroyShaderModule(VkDevice logicalDevice, VkShaderModule & shaderModule)
	{
		if (VK_NULL_HANDLE != shaderModule)
		{
			vkDestroyShaderModule(logicalDevice, shaderModule, nullptr);
			shaderModule = VK_NULL_HANDLE;
		}
	}
}