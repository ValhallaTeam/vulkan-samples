#pragma once
#include "../CommonFiles/Common.h"

namespace VulkanSampleFramework
{
	struct SubpassParameters {
		VkPipelineBindPoint m_PipelineType;
		std::vector<VkAttachmentReference> m_InputAttachments;
		std::vector<VkAttachmentReference> m_ColorAttachments;
		std::vector<VkAttachmentReference> m_ResolveAttachments;
		VkAttachmentReference const *m_DepthStencilAttachment;
		std::vector<uint32_t> m_PreserveAttachments;
	};


	void SpecifySubpassDescriptions(std::vector<SubpassParameters> const &subpassParameters, std::vector<VkSubpassDescription> &subpassDescriptions);
	bool CreateRenderPass(VkDevice logicalDevice, std::vector<VkAttachmentDescription> const & attachmentsDescriptions,
		std::vector<SubpassParameters> const &subpassParameters, std::vector<VkSubpassDependency> const &subpassDependencies, VkRenderPass &renderPass);
	bool CreateFramebuffer(VkDevice logicalDevice, VkRenderPass renderPass, std::vector<VkImageView> const &attachments, uint32_t width, uint32_t height,
		uint32_t layers, VkFramebuffer &framebuffer);
	void BeginRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea,
		std::vector<VkClearValue> const &clearValues, VkSubpassContents subpassContents);
	void ProgressToTheNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents subpassContents);
	void EndRenderPass(VkCommandBuffer commandBuffer);
	void DestroyFramebuffer(VkDevice logicalDevice, VkFramebuffer &framebuffer);
	void DestroyRenderPass(VkDevice logicalDevice, VkRenderPass &renderPass);
}
