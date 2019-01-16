#pragma once
#include "../CommonFiles/Common.h"
#include "../CommonFiles/Tools.h"

namespace VulkanSampleFramework
{
	struct ShaderStageParameters
	{
		VkShaderStageFlagBits m_ShaderStage;
		VkShaderModule m_ShaderModule;
		char const *m_EntryPointName;
		VkSpecializationInfo const *m_SpecializationInfo;
	};

	struct ViewportInfo
	{
		std::vector<VkViewport> m_Viewports;
		std::vector<VkRect2D> m_Scissors;
	};

	bool CreateShaderModule(VkDevice logicalDevice, std::vector<unsigned char> const &sourceCode, VkShaderModule &shaderModule);
	void SpecifyPipelineShaderStages(std::vector<ShaderStageParameters> const &shaderStageParams, 
		std::vector<VkPipelineShaderStageCreateInfo> & shaderStageCreateInfos);
	void SpecifyPipelineVertexInputState(std::vector<VkVertexInputBindingDescription> const &bindingDescriptions,
		std::vector<VkVertexInputAttributeDescription> const & attributeDescriptions, VkPipelineVertexInputStateCreateInfo &vertexInputStateCreateInfo);
	void SpecifyPipelineInputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnable,
		VkPipelineInputAssemblyStateCreateInfo & inputAssemblyStateCreateInfo);
	void SpecifyPipelineTessellationState(uint32_t patchControlPintsCount, VkPipelineTessellationStateCreateInfo &tessellationStateCreateInfo);
	void SpecifyPipelineViewportAndScissorTestState(ViewportInfo const &viewportInfos, VkPipelineViewportStateCreateInfo &viewportStateCreateInfo);
	void SpecifyPipelineRasterizationState(bool depthClampEnable, bool rasterizerDiscardEnable, VkPolygonMode polygonMode, VkCullModeFlags cullingMode,
		VkFrontFace frontFace, bool depthBiasEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, float lineWidth,
		VkPipelineRasterizationStateCreateInfo & rasterizationStateCreateInfo);
	void SpecifyPipelineMultisampleState(VkSampleCountFlagBits sampleCount, bool perSampleShadingEnable, float minSampleShading, VkSampleMask const *sampleMasks,
		bool alphaToCoverageEnable, bool alphaToOneEnable, VkPipelineMultisampleStateCreateInfo & multisampleStateCreateInfo);
	void SpecifyPipelineDepthAndStencilState(bool depthTestEnable, bool depthWriteEnable, VkCompareOp depthCompareOp, bool depthBoundsTestEnable,
		float minDepthBounds, float maxDepthBounds, bool stencilTestEnable, VkStencilOpState frontStencilTestParameters, VkStencilOpState backStencilTestParameters,
		VkPipelineDepthStencilStateCreateInfo & depthAndStencilStateCreateInfo);
	void SpecifyPipelineBlendState(bool logicOpEnable, VkLogicOp logicOp, std::vector<VkPipelineColorBlendAttachmentState> const & attachmentBlendStates,
		std::array<float, 4> const &blendConstants, VkPipelineColorBlendStateCreateInfo &blendStateCreateInfo);
	void SpecifyPipelineDynamicStates(std::vector<VkDynamicState> const &dynamicStates, VkPipelineDynamicStateCreateInfo  &dynamicStateCreatInfo);
	bool CreatePipelineLayout(VkDevice logicalDevice, std::vector<VkDescriptorSetLayout> const &descriptorSetLayouts,
		std::vector<VkPushConstantRange> const &pushConstantRanges, VkPipelineLayout &pipelineLayout);
	void SpecifyGraphicsPipelineCreationParameters(VkPipelineCreateFlags additionalOptions,
		std::vector<VkPipelineShaderStageCreateInfo> const &shaderStageCreateInfos, VkPipelineVertexInputStateCreateInfo const &vertexInputStateCreateInfo,
		VkPipelineInputAssemblyStateCreateInfo const &inputAssemblyStateCreateInfo, VkPipelineTessellationStateCreateInfo const *tessellationStateCreateInfo,
		VkPipelineViewportStateCreateInfo const *viewportStateCreateInfo, VkPipelineRasterizationStateCreateInfo const &rasterizationStateCreateInfo,
		VkPipelineMultisampleStateCreateInfo const *multisampleStateCreateInfo, VkPipelineDepthStencilStateCreateInfo const *depthAndStencilStateCreateInfo,
		VkPipelineColorBlendStateCreateInfo const * blendStateCreateInfo, VkPipelineDynamicStateCreateInfo const *dynamicStateCreatInfo,
		VkPipelineLayout pipelineLayout, VkRenderPass renderPass, uint32_t subpass, VkPipeline basePipelineHandle, int32_t basePipelineIndex,
		VkGraphicsPipelineCreateInfo &graphicsPipelineCreateInfo);
	bool CreatePipelineCacheObject(VkDevice logicalDevice, std::vector<unsigned char> const &cacheData, VkPipelineCache &pipelineCache);
	bool RetrieveDataFromPipelineCache(VkDevice logicalDevice, VkPipelineCache pipelineCache, std::vector<unsigned char> &pipelineCacheData);
	bool MergeMultiplePipelineCacheObjects(VkDevice logicalDevice, VkPipelineCache targetPipelineCache, std::vector<VkPipelineCache> const &sourcePipelineCaches);
	bool CreateGraphicsPipelines(VkDevice logicalDevice, std::vector<VkGraphicsPipelineCreateInfo> const &graphicsPipelineCreateInfos, VkPipelineCache pipelineCache,
		std::vector<VkPipeline>  &graphicsPipelines);
	void CreateComputePiplineInfo(VkPipelineCreateFlags additionalOptions, VkPipelineShaderStageCreateInfo const &computeShaderStage,
		VkPipelineLayout pipelineLayout, VkPipeline basePipelineHandle, int32_t basePipelineIndex, VkComputePipelineCreateInfo &computePipelineCreateInfo);
	bool CreateComputePipeline(VkDevice logicalDevice, std::vector<VkComputePipelineCreateInfo> const &computePipelineCreateInfos, VkPipelineCache pipelineCache, 
		VkPipeline &computePipeline);
	void BindPipelineObject(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineType, VkPipeline pipeline);
	bool CreateMultipleGraphicsPipelinesOnMultipleThreads(VkDevice logicalDevice, std::string const &pipelineCacheFilename,
		std::vector<std::vector<VkGraphicsPipelineCreateInfo>> const & graphicsPipelinesCreateInfos, std::vector<std::vector<VkPipeline>> & graphicsPipelines);
	void DestroyPipeline(VkDevice logicalDevice, VkPipeline & pipeline);
	void DestroyPipelineCache(VkDevice logicalDevice, VkPipelineCache &pipelineCache);
	void DestroyPipelineLayout(VkDevice logicalDevice, VkPipelineLayout & pipelineLayout);
	void DestroyShaderModule(VkDevice logicalDevice, VkShaderModule & shaderModule);

}