#include "RenderPassAndFramebufferFunctions.h"

namespace VulkanSampleFramework
{
	void SpecifySubpassDescriptions(std::vector<SubpassParameters> const &subpassParameters, std::vector<VkSubpassDescription> &subpassDescriptions)
	{
		subpassDescriptions.clear();

		for (auto & subpassDescription : subpassParameters) {
			subpassDescriptions.push_back
			(
				{
					0,																			// VkSubpassDescriptionFlags        flags
					subpassDescription.m_PipelineType,											// VkPipelineBindPoint              pipelineBindPoint
					static_cast<uint32_t>(subpassDescription.m_InputAttachments.size()),		// uint32_t                         inputAttachmentCount
					subpassDescription.m_InputAttachments.data(),								// const VkAttachmentReference    * pInputAttachments
					static_cast<uint32_t>(subpassDescription.m_ColorAttachments.size()),		// uint32_t                         colorAttachmentCount
					subpassDescription.m_ColorAttachments.data(),								// const VkAttachmentReference    * pColorAttachments
					subpassDescription.m_ResolveAttachments.data(),								// const VkAttachmentReference    * pResolveAttachments
					subpassDescription.m_DepthStencilAttachment,								// const VkAttachmentReference    * pDepthStencilAttachment
					static_cast<uint32_t>(subpassDescription.m_PreserveAttachments.size()),		// uint32_t                         preserveAttachmentCount
					subpassDescription.m_PreserveAttachments.data()								// const uint32_t                 * pPreserveAttachments
				}
			);
		}
	}

	bool CreateRenderPass(VkDevice logicalDevice, std::vector<VkAttachmentDescription> const &attachmentsDescriptions,
		std::vector<SubpassParameters> const &subpassParameters, std::vector<VkSubpassDependency> const &subpassDependencies, VkRenderPass &renderPass)
	{
		std::vector<VkSubpassDescription> subpassDescriptions;
		SpecifySubpassDescriptions(subpassParameters, subpassDescriptions);

		VkRenderPassCreateInfo renderPassCreateInfo =
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,					// VkStructureType                    sType
			nullptr,													// const void                       * pNext
			0,															// VkRenderPassCreateFlags            flags
			static_cast<uint32_t>(attachmentsDescriptions.size()),		// uint32_t                           attachmentCount
			attachmentsDescriptions.data(),								// const VkAttachmentDescription    * pAttachments
			static_cast<uint32_t>(subpassDescriptions.size()),			// uint32_t                           subpassCount
			subpassDescriptions.data(),									// const VkSubpassDescription       * pSubpasses
			static_cast<uint32_t>(subpassDependencies.size()),			// uint32_t                           dependencyCount
			subpassDependencies.data()									// const VkSubpassDependency        * pDependencies
		};

		VkResult result = vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, nullptr, &renderPass);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create a render pass." << std::endl;
			return false;
		}
		return true;
	}

	bool CreateFramebuffer(VkDevice logicalDevice, VkRenderPass renderPass, std::vector<VkImageView> const &attachments, uint32_t width, uint32_t height,
		uint32_t layers, VkFramebuffer &framebuffer)
	{
		VkFramebufferCreateInfo framebufferCreateInfo =
		{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,		// VkStructureType              sType
			nullptr,										// const void                 * pNext
			0,												// VkFramebufferCreateFlags     flags
			renderPass,										// VkRenderPass                 renderPass
			static_cast<uint32_t>(attachments.size()),		// uint32_t                     attachmentCount
			attachments.data(),								// const VkImageView          * pAttachments
			width,											// uint32_t                     width
			height,											// uint32_t                     height
			layers											// uint32_t                     layers
		};

		VkResult result = vkCreateFramebuffer(logicalDevice, &framebufferCreateInfo, nullptr, &framebuffer);
		if (VK_SUCCESS != result)
		{
			std::cout << "Could not create a framebuffer." << std::endl;
			return false;
		}
		return true;
	}

	void BeginRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea,
		std::vector<VkClearValue> const &clearValues, VkSubpassContents subpassContents)
	{
		VkRenderPassBeginInfo renderPassBeginInfo =
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,		// VkStructureType        sType
			nullptr,										// const void           * pNext
			renderPass,										// VkRenderPass           renderPass
			framebuffer,									// VkFramebuffer          framebuffer
			renderArea,										// VkRect2D               renderArea
			static_cast<uint32_t>(clearValues.size()),		// uint32_t               clearValueCount
			clearValues.data()								// const VkClearValue   * pClearValues
		};

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, subpassContents);
	}

	void ProgressToTheNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents subpassContents)
	{
		vkCmdNextSubpass(commandBuffer, subpassContents);
	}

	void EndRenderPass(VkCommandBuffer commandBuffer)
	{
		vkCmdEndRenderPass(commandBuffer);
	}

	void DestroyFramebuffer(VkDevice logicalDevice, VkFramebuffer &framebuffer)
	{
		if (VK_NULL_HANDLE != framebuffer)
		{
			vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
			framebuffer = VK_NULL_HANDLE;
		}
	}

	void DestroyRenderPass(VkDevice logicalDevice, VkRenderPass &renderPass)
	{
		if (VK_NULL_HANDLE != renderPass)
		{
			vkDestroyRenderPass(logicalDevice, renderPass, nullptr);
			renderPass = VK_NULL_HANDLE;
		}
	}

}