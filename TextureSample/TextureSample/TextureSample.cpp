// TextureSample.cpp : Defines the entry point for the console application.
//

#include "VulkanSampleFramework.h"

using namespace VulkanSampleFramework;

class TextureSample : public VulkanSample
{
public:
	// Should wrap to one class called object
	Mesh m_Skybox;
	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;

	bool m_UpdateUniformBuffer;
	VkBuffer m_UniformBuffer;
	VkDeviceMemory m_UniformBufferMemory;

	// Should wrap to one class called image
	VkImage m_CubemapImage;
	VkDeviceMemory m_CubemapImageMemory;
	VkSampler m_CubemapSampler;
	VkImageView m_CubemapImageView;

	// Maybe using like render policy like Unreal?
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets;

	VkRenderPass m_RenderPass;
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_Pipeline;

	VkBuffer m_StagingBuffer;
	VkDeviceMemory m_StagingBufferMemory;

	static const VkFormat m_DepthFormat = VK_FORMAT_D16_UNORM;

	virtual bool Initialize(WindowParameters windowParameters) override
	{
		if (!InitializeVulkan(windowParameters))
		{
			return false;
		}

		// Vertex data
		if (!Load3DModelFromObjFile("Data/Models/cube.obj", false, false, false, false, m_Skybox))
		{
			return false;
		}

		if (!CreateBuffer(m_LogicalDevice, sizeof(m_Skybox.m_Data[0]) * m_Skybox.m_Data.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_VertexBuffer))
		{
			return false;
		}


		if (!AllocateAndBindMemoryObjectToBuffer(m_LogicalDevice, m_VertexBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_PhysicalDeviceMemoryProperties, m_VertexBufferMemory))
		{
			return false;
		}

		if (!UseStagingBufferToUpdateBufferWithDeviceLocalMemoryBound(m_LogicalDevice, sizeof(m_Skybox.m_Data[0]) * m_Skybox.m_Data.size(),
			&m_Skybox.m_Data[0], m_VertexBuffer, 0, 0, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			m_GraphicsQueue.m_Handle, m_FramesResources.front().m_CommandBuffer, {}, m_PhysicalDeviceMemoryProperties))
		{
			return false;
		}

		// Staging buffer
		if (!CreateBuffer(m_LogicalDevice, 2 * 16 * sizeof(float), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_StagingBuffer))
		{
			return false;
		}
		if (!AllocateAndBindMemoryObjectToBuffer(m_LogicalDevice, m_StagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_PhysicalDeviceMemoryProperties, m_StagingBufferMemory))
		{
			return false;
		}

		// Uniform buffer
		if (!CreateBuffer(m_LogicalDevice, 2 * 16 * sizeof(float), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, m_UniformBuffer))
		{
			return false;
		}
		if (!AllocateAndBindMemoryObjectToBuffer(m_LogicalDevice, m_UniformBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_PhysicalDeviceMemoryProperties, m_UniformBufferMemory))
		{
			return false;
		}

		if (!UpdateStagingBuffer(true))
		{
			return false;
		}

		// Cubemap
		if (!CreateSampler(m_LogicalDevice, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.0f, false, 1.0, false, VK_COMPARE_OP_ALWAYS, 0.0f, 1.0f,
			VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK, false, m_CubemapSampler))
		{
			return false;
		}

		if (!CreateSampledImage(m_PhysicalDevice, m_LogicalDevice, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, { 1024, 1024, 1 }, 1, 6,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, true, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, true, m_PhysicalDeviceMemoryProperties,
			m_CubemapImage, m_CubemapImageMemory, m_CubemapImageView))
		{
			return false;
		}

		std::vector<std::string> cubemapImages =
		{
			"Data/Textures/posx.jpg",
			"Data/Textures/negx.jpg",
			"Data/Textures/posy.jpg",
			"Data/Textures/negy.jpg",
			"Data/Textures/posz.jpg",
			"Data/Textures/negz.jpg"
		};

		for (size_t i = 0; i < cubemapImages.size(); ++i)
		{
			std::vector<unsigned char> cubemapImageData;
			int imageDataSize;
			if (!LoadTextureDataFromFile(cubemapImages[i].c_str(), 4, cubemapImageData, nullptr, nullptr, nullptr, &imageDataSize))
			{
				return false;
			}

			VkImageSubresourceLayers imageSubresource =
			{
				VK_IMAGE_ASPECT_COLOR_BIT,    // VkImageAspectFlags     aspectMask
				0,                            // uint32_t               mipLevel
				static_cast<uint32_t>(i),     // uint32_t               baseArrayLayer
				1                             // uint32_t               layerCount
			};
			UseStagingBufferToUpdateImageWithDeviceLocalMemoryBound(m_LogicalDevice, imageDataSize, &cubemapImageData[0],
				m_CubemapImage, imageSubresource, { 0, 0, 0 }, { 1024, 1024, 1 }, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				0, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				m_GraphicsQueue.m_Handle, m_FramesResources.front().m_CommandBuffer, {}, m_PhysicalDeviceMemoryProperties);
		}

		// Descriptor set with uniform buffer
		std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings =
		{
			{
				0,                                          // uint32_t             binding
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType     descriptorType
				1,                                          // uint32_t             descriptorCount
				VK_SHADER_STAGE_VERTEX_BIT,                 // VkShaderStageFlags   stageFlags
				nullptr                                     // const VkSampler    * pImmutableSamplers
			},
			{
				1,                                          // uint32_t             binding
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType     descriptorType
				1,                                          // uint32_t             descriptorCount
				VK_SHADER_STAGE_FRAGMENT_BIT,               // VkShaderStageFlags   stageFlags
				nullptr                                     // const VkSampler    * pImmutableSamplers
			}
		};

		if (!CreateDescriptorSetLayout(m_LogicalDevice, descriptorSetLayoutBindings, m_DescriptorSetLayout))
		{
			return false;
		}

		std::vector<VkDescriptorPoolSize> descriptorPoolSizes =
		{
			{
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,          // VkDescriptorType     type
				1                                           // uint32_t             descriptorCount
			},
			{
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType     type
				1                                           // uint32_t             descriptorCount
			}
		};

		if (!CreateDescriptorPool(m_LogicalDevice, false, 1, descriptorPoolSizes, m_DescriptorPool))
		{
			return false;
		}

		if (!AllocateDescriptorSets(m_LogicalDevice, m_DescriptorPool, { m_DescriptorSetLayout }, m_DescriptorSets)) 
		{
			return false;
		}

		BufferDescriptorInfo bufferDescriptorUpdate =
		{
			m_DescriptorSets[0],							// VkDescriptorSet                      TargetDescriptorSet
			0,												// uint32_t                             TargetDescriptorBinding
			0,												// uint32_t                             TargetArrayElement
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,				// VkDescriptorType                     TargetDescriptorType
			{												// std::vector<VkDescriptorBufferInfo>  BufferInfos
				{
					m_UniformBuffer,                        // VkBuffer                             buffer
					0,                                      // VkDeviceSize                         offset
					VK_WHOLE_SIZE                           // VkDeviceSize                         range
				}
			}
		};

		ImageDescriptorInfo imageDescriptorUpdate =
		{
			m_DescriptorSets[0],								// VkDescriptorSet                      TargetDescriptorSet
			1,													// uint32_t                             TargetDescriptorBinding
			0,													// uint32_t                             TargetArrayElement
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,			// VkDescriptorType                     TargetDescriptorType
			{													// std::vector<VkDescriptorImageInfo>   ImageInfos
				{
					m_CubemapSampler,							// VkSampler                            sampler
					m_CubemapImageView,							// VkImageView                          imageView
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL	// VkImageLayout                        imageLayout
				}
			}
		};
		
		// Update buffer and image to descriptor set
		UpdateDescriptorSets(m_LogicalDevice, { imageDescriptorUpdate }, { bufferDescriptorUpdate }, {}, {});

		// Render pass
		std::vector<VkAttachmentDescription> attachmentDescriptions = 
		{
			{
				0,														// VkAttachmentDescriptionFlags     flags
				m_Swapchain.m_Format,									// VkFormat                         format
				VK_SAMPLE_COUNT_1_BIT,									// VkSampleCountFlagBits            samples
				VK_ATTACHMENT_LOAD_OP_CLEAR,							// VkAttachmentLoadOp               loadOp
				VK_ATTACHMENT_STORE_OP_STORE,							// VkAttachmentStoreOp              storeOp
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,						// VkAttachmentLoadOp               stencilLoadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,						// VkAttachmentStoreOp              stencilStoreOp
				VK_IMAGE_LAYOUT_UNDEFINED,								// VkImageLayout                    initialLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR							// VkImageLayout                    finalLayout
			},
			{
				0,														// VkAttachmentDescriptionFlags     flags
				m_DepthFormat,											// VkFormat                         format
				VK_SAMPLE_COUNT_1_BIT,									// VkSampleCountFlagBits            samples
				VK_ATTACHMENT_LOAD_OP_CLEAR,							// VkAttachmentLoadOp               loadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,						// VkAttachmentStoreOp              storeOp
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,						// VkAttachmentLoadOp               stencilLoadOp
				VK_ATTACHMENT_STORE_OP_DONT_CARE,						// VkAttachmentStoreOp              stencilStoreOp
				VK_IMAGE_LAYOUT_UNDEFINED,								// VkImageLayout                    initialLayout
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL		// VkImageLayout                    finalLayout
			}
		};

		VkAttachmentReference depthAttachment =
		{
			1,													// uint32_t                             attachment
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL	// VkImageLayout                        layout;
		};

		std::vector<SubpassParameters> subpassParameters = 
		{
			{
				VK_PIPELINE_BIND_POINT_GRAPHICS,					// VkPipelineBindPoint                  PipelineType
				{},													// std::vector<VkAttachmentReference>   InputAttachments
				{													// std::vector<VkAttachmentReference>   ColorAttachments
					{
						0,                                          // uint32_t                             attachment
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,   // VkImageLayout                        layout
					}
				},
				{},													// std::vector<VkAttachmentReference>   ResolveAttachments
				&depthAttachment,									// VkAttachmentReference const        * DepthStencilAttachment
				{}													// std::vector<uint32_t>                PreserveAttachments
			}
		};

		std::vector<VkSubpassDependency> subpassDependencies =
		{
			{
				VK_SUBPASS_EXTERNAL,                            // uint32_t                   srcSubpass
				0,                                              // uint32_t                   dstSubpass
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,              // VkPipelineStageFlags       srcStageMask
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags       dstStageMask
				VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags              srcAccessMask
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags              dstAccessMask
				VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
			},
			{
				0,                                              // uint32_t                   srcSubpass
				VK_SUBPASS_EXTERNAL,                            // uint32_t                   dstSubpass
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags       srcStageMask
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,              // VkPipelineStageFlags       dstStageMask
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags              srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags              dstAccessMask
				VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
			}
		};


		if (!CreateRenderPass(m_LogicalDevice, attachmentDescriptions, subpassParameters, subpassDependencies, m_RenderPass))
		{
			return false;
		}

		// Graphics pipeline
		// Model
		std::vector<unsigned char> vertexShaderSpirv;
		if (!GetBinaryFileContents("Data/Shaders/Skybox.vert.spirv", vertexShaderSpirv))
		{
			return false;
		}

		VkShaderModule vertexShaderModule; //< Should be release after create graphic pipeline
		if (!CreateShaderModule(m_LogicalDevice, vertexShaderSpirv, vertexShaderModule))
		{
			return false;
		}

		std::vector<unsigned char> fragmentShaderSpirv;
		if (!GetBinaryFileContents("Data/Shaders/Skybox.frag.spirv", fragmentShaderSpirv)) 
		{
			return false;
		}

		VkShaderModule fragmentShaderModule;
		if (!CreateShaderModule(m_LogicalDevice, fragmentShaderSpirv, fragmentShaderModule))
		{
			return false;
		}

		std::vector<ShaderStageParameters> shaderStageParams =
		{
			{
				VK_SHADER_STAGE_VERTEX_BIT,			// VkShaderStageFlagBits        ShaderStage
				vertexShaderModule,					// VkShaderModule               ShaderModule
				"main",								// char const                 * EntryPointName;
				nullptr								// VkSpecializationInfo const * SpecializationInfo;
			},
			{
				VK_SHADER_STAGE_FRAGMENT_BIT,		// VkShaderStageFlagBits        ShaderStage
				fragmentShaderModule,				// VkShaderModule               ShaderModule
				"main",								// char const                 * EntryPointName
				nullptr								// VkSpecializationInfo const * SpecializationInfo
			}
		};

		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
		SpecifyPipelineShaderStages(shaderStageParams, shaderStageCreateInfos);

		// Vertex binding
		std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions =
		{
			{
				0,								// uint32_t                     binding
				3 * sizeof(float),				// uint32_t                     stride
				VK_VERTEX_INPUT_RATE_VERTEX		// VkVertexInputRate            inputRate
			}
		};

		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions = 
		{
			{
				0,                                                                        // uint32_t   location
				0,                                                                        // uint32_t   binding
				VK_FORMAT_R32G32B32_SFLOAT,                                               // VkFormat   format
				0                                                                         // uint32_t   offset
			}
		};

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
		SpecifyPipelineVertexInputState(vertexInputBindingDescriptions, vertexAttributeDescriptions, vertexInputStateCreateInfo);

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
		SpecifyPipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, inputAssemblyStateCreateInfo);

		// Vieew port
		ViewportInfo viewportInfos =
		{
			{							// std::vector<VkViewport>   Viewports
				{
					0.0f,               // float          x
					0.0f,               // float          y
					500.0f,             // float          width
					500.0f,             // float          height
					0.0f,               // float          minDepth
					1.0f                // float          maxDepth
				}
			},
			{							// std::vector<VkRect2D>     Scissors
				{
					{                   // VkOffset2D     offset
						0,              // int32_t        x
						0               // int32_t        y
					},
					{					// VkExtent2D     extent
						500,            // uint32_t       width
						500             // uint32_t       height
					}
				}
			}
		};
		VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
		SpecifyPipelineViewportAndScissorTestState(viewportInfos, viewportStateCreateInfo);

		// Rasterization state
		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
		SpecifyPipelineRasterizationState(false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f, rasterizationStateCreateInfo);

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
		SpecifyPipelineMultisampleState(VK_SAMPLE_COUNT_1_BIT, false, 0.0f, nullptr, false, false, multisampleStateCreateInfo);

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
		SpecifyPipelineDepthAndStencilState(true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0.0f, 1.0f, false, {}, {}, depthStencilStateCreateInfo);

		// Attachment blending state
		std::vector<VkPipelineColorBlendAttachmentState> attachmentBlendStates = 
		{
			{
				false,                          // VkBool32                 blendEnable
				VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcColorBlendFactor
				VK_BLEND_FACTOR_ONE,            // VkBlendFactor            dstColorBlendFactor
				VK_BLEND_OP_ADD,                // VkBlendOp                colorBlendOp
				VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcAlphaBlendFactor
				VK_BLEND_FACTOR_ONE,            // VkBlendFactor            dstAlphaBlendFactor
				VK_BLEND_OP_ADD,                // VkBlendOp                alphaBlendOp
				VK_COLOR_COMPONENT_R_BIT |      // VkColorComponentFlags    colorWriteMask
				VK_COLOR_COMPONENT_G_BIT |
				VK_COLOR_COMPONENT_B_BIT |
				VK_COLOR_COMPONENT_A_BIT
			}
		};

		VkPipelineColorBlendStateCreateInfo blendStateCreateInfo;
		SpecifyPipelineBlendState(false, VK_LOGIC_OP_COPY, attachmentBlendStates, { 1.0f, 1.0f, 1.0f, 1.0f }, blendStateCreateInfo);

		std::vector<VkDynamicState> dynamicSates = 
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
		SpecifyPipelineDynamicStates(dynamicSates, dynamicStateCreateInfo);

		if (!CreatePipelineLayout(m_LogicalDevice, { m_DescriptorSetLayout }, {}, m_PipelineLayout))
		{
			return false;
		}

		// graphics pipe create info
		VkGraphicsPipelineCreateInfo pipelineCreateInfo;
		SpecifyGraphicsPipelineCreationParameters(0, shaderStageCreateInfos, vertexInputStateCreateInfo, inputAssemblyStateCreateInfo,
			nullptr, &viewportStateCreateInfo, rasterizationStateCreateInfo, &multisampleStateCreateInfo, &depthStencilStateCreateInfo, &blendStateCreateInfo,
			&dynamicStateCreateInfo, m_PipelineLayout, m_RenderPass, 0, VK_NULL_HANDLE, -1, pipelineCreateInfo);

		std::vector<VkPipeline> pipeline;
		if (!CreateGraphicsPipelines(m_LogicalDevice, { pipelineCreateInfo }, VK_NULL_HANDLE, pipeline))
		{
			return false;
		}
		m_Pipeline = pipeline[0];

		// Destroy shader module after create pipeline object
		DestroyShaderModule(m_LogicalDevice, vertexShaderModule);
		DestroyShaderModule(m_LogicalDevice, fragmentShaderModule);
		return true;
	}


	virtual bool Draw() override
	{
		// Lambda
		auto prepareFrame = [&](VkCommandBuffer commandBuffer, uint32_t swapchainImageIndex, VkFramebuffer framebuffer)
		{
			if (!BeginCommandBufferRecordingOperation(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr))
			{
				return false;
			}

			if (m_UpdateUniformBuffer)
			{
				m_UpdateUniformBuffer = false;

				BufferTransition preTransferTransition =
				{
					m_UniformBuffer,					// VkBuffer         Buffer
					VK_ACCESS_UNIFORM_READ_BIT,			// VkAccessFlags    CurrentAccess
					VK_ACCESS_TRANSFER_WRITE_BIT,		// VkAccessFlags    NewAccess
					VK_QUEUE_FAMILY_IGNORED,			// uint32_t         CurrentQueueFamily
					VK_QUEUE_FAMILY_IGNORED				// uint32_t         NewQueueFamily
				};
				SetBufferMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, { preTransferTransition });

				std::vector<VkBufferCopy> regions =
				{
					{
						0,							// VkDeviceSize     srcOffset
						0,							// VkDeviceSize     dstOffset
						2 * 16 * sizeof(float)		// VkDeviceSize     size
					}
				};
				CopyDataBetweenBuffers(commandBuffer, m_StagingBuffer, m_UniformBuffer, regions);

				BufferTransition postTransferTransition =
				{
					m_UniformBuffer,					// VkBuffer         Buffer
					VK_ACCESS_TRANSFER_WRITE_BIT,		// VkAccessFlags    CurrentAccess
					VK_ACCESS_UNIFORM_READ_BIT,			// VkAccessFlags    NewAccess
					VK_QUEUE_FAMILY_IGNORED,			// uint32_t         CurrentQueueFamily
					VK_QUEUE_FAMILY_IGNORED				// uint32_t         NewQueueFamily
				};
				SetBufferMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, { postTransferTransition });
			}

			if (m_PresentQueue.m_FamilyIndex != m_GraphicsQueue.m_FamilyIndex)
			{
				ImageTransition imageTransitionBeforeDrawing =
				{
					m_Swapchain.m_Images[swapchainImageIndex],		// VkImage              Image
					VK_ACCESS_MEMORY_READ_BIT,						// VkAccessFlags        CurrentAccess
					VK_ACCESS_MEMORY_READ_BIT,						// VkAccessFlags        NewAccess
					VK_IMAGE_LAYOUT_UNDEFINED,						// VkImageLayout        CurrentLayout
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,		// VkImageLayout        NewLayout
					m_PresentQueue.m_FamilyIndex,					// uint32_t             CurrentQueueFamily
					m_GraphicsQueue.m_FamilyIndex,					// uint32_t             NewQueueFamily
					VK_IMAGE_ASPECT_COLOR_BIT						// VkImageAspectFlags   Aspect
				};
				SetImageMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, { imageTransitionBeforeDrawing });
			}

			// Drawing
			BeginRenderPass(commandBuffer, m_RenderPass, framebuffer, { { 0, 0 }, m_Swapchain.m_Size }, { { 0.1f, 0.2f, 0.3f, 1.0f },{ 1.0f, 0 } }, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport viewport =
			{
				0.0f,												// float    x
				0.0f,												// float    y
				static_cast<float>(m_Swapchain.m_Size.width),		// float    width
				static_cast<float>(m_Swapchain.m_Size.height),		// float    height
				0.0f,												// float    minDepth
				1.0f,												// float    maxDepth
			};
			SetViewportStateDynamically(commandBuffer, 0, { viewport });

			VkRect2D scissor =
			{
				{													// VkOffset2D     offset
					0,												// int32_t        x
					0												// int32_t        y
				},
				{													// VkExtent2D     extent
					m_Swapchain.m_Size.width,                       // uint32_t       width
					m_Swapchain.m_Size.height                       // uint32_t       height
				}
			};
			SetScissorStateDynamically(commandBuffer, 0, { scissor });

			BindVertexBuffers(commandBuffer, 0, { { m_VertexBuffer, 0 } });

			BindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, m_DescriptorSets, {});

			BindPipelineObject(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

			for (size_t i = 0; i < m_Skybox.m_Parts.size(); ++i)
			{
				DrawGeometry(commandBuffer, m_Skybox.m_Parts[i].m_VertexCount, 1, m_Skybox.m_Parts[i].m_VertexOffset, 0);
			}

			EndRenderPass(commandBuffer);

			if (m_PresentQueue.m_FamilyIndex != m_GraphicsQueue.m_FamilyIndex)
			{
				ImageTransition imageRransitionBeforePresent =
				{
					m_Swapchain.m_Images[swapchainImageIndex],		// VkImage              Image
					VK_ACCESS_MEMORY_READ_BIT,						// VkAccessFlags        CurrentAccess
					VK_ACCESS_MEMORY_READ_BIT,						// VkAccessFlags        NewAccess
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,				// VkImageLayout        CurrentLayout
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,				// VkImageLayout        NewLayout
					m_GraphicsQueue.m_FamilyIndex,					// uint32_t             CurrentQueueFamily
					m_PresentQueue.m_FamilyIndex,					// uint32_t             NewQueueFamily
					VK_IMAGE_ASPECT_COLOR_BIT						// VkImageAspectFlags   Aspect
				};
				SetImageMemoryBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, { imageRransitionBeforePresent });
			}

			if (!EndCommandBufferRecordingOperation(commandBuffer))
			{
				return false;
			}

			return true;
		};

		return IncreasePerformanceThroughIncreasingTheNumberOfSeparatelyRenderedFrames(m_LogicalDevice, m_GraphicsQueue.m_Handle, m_PresentQueue.m_Handle, m_Swapchain.m_Handle, m_Swapchain.m_Size,
			m_Swapchain.m_ImageViews, m_RenderPass, {}, prepareFrame, m_FramesResources);
		return true;
	}

	void OnMouseEvent()
	{
		UpdateStagingBuffer(false);
	}

	bool UpdateStagingBuffer(bool force)
	{
		m_UpdateUniformBuffer = true;
		static float horizontalAngle = 0.0f;
		static float verticalAngle = 0.0f;
		if (m_MouseState.Buttons[0].IsPressed || force)
		{
			horizontalAngle += 0.5f * m_MouseState.Position.Delta.X;
			verticalAngle -= 0.5f * m_MouseState.Position.Delta.Y;
			if (verticalAngle > 90.0f)
			{
				verticalAngle = 90.0f;
			}
			if (verticalAngle < -90.0f)
			{
				verticalAngle = -90.0f;
			}

			Matrix4x4 modelViewMatrix = PrepareRotationMatrix(verticalAngle, { 1.0f, 0.0f, 0.0f }) * PrepareRotationMatrix(horizontalAngle, { 0.0f, -1.0f, 0.0f });

			if (!MapUpdateAndUnmapHostVisibleMemory(m_LogicalDevice, m_StagingBufferMemory, 0, sizeof(modelViewMatrix[0]) * modelViewMatrix.size(), &modelViewMatrix[0], true, nullptr))
			{
				return false;
			}

			Matrix4x4 perspectiveMatrix = PreparePerspectiveProjectionMatrix(static_cast<float>(m_Swapchain.m_Size.width) / static_cast<float>(m_Swapchain.m_Size.height),
				50.0f, 0.5f, 10.0f);

			if (!MapUpdateAndUnmapHostVisibleMemory(m_LogicalDevice, m_StagingBufferMemory, sizeof(modelViewMatrix[0]) * modelViewMatrix.size(),
				sizeof(perspectiveMatrix[0]) * perspectiveMatrix.size(), &perspectiveMatrix[0], true, nullptr))
			{
				return false;
			}
		}
		return true;
	}

	virtual bool Resize() override
	{
		if (!CreateSwapchain())
		{
			return false;
		}

		if (IsReady())
		{
			if (!UpdateStagingBuffer(true))
			{
				return false;
			}
		}
		return true;
	}

	virtual void Deinitialize()
	{
		if (m_LogicalDevice)
		{
			WaitForAllSubmittedCommandsToBeFinished(m_LogicalDevice);

			Mesh m_Skybox;

			FreeMemoryObject(m_LogicalDevice, m_StagingBufferMemory);
			DestroyBuffer(m_LogicalDevice, m_StagingBuffer);

			DestroyPipeline(m_LogicalDevice, m_Pipeline);
			DestroyPipelineLayout(m_LogicalDevice, m_PipelineLayout);
			DestroyRenderPass(m_LogicalDevice, m_RenderPass);
			
			DestroyDescriptorPool(m_LogicalDevice, m_DescriptorPool);				//< will release all descriptor set that allocate from this pool
			DestroyDescriptorSetLayout(m_LogicalDevice, m_DescriptorSetLayout);
			
			DestroyImageView(m_LogicalDevice, m_CubemapImageView);
			DestroySampler(m_LogicalDevice, m_CubemapSampler);
			FreeMemoryObject(m_LogicalDevice, m_CubemapImageMemory);
			DestroyImage(m_LogicalDevice, m_CubemapImage);

			FreeMemoryObject(m_LogicalDevice, m_UniformBufferMemory);
			DestroyBuffer(m_LogicalDevice, m_UniformBuffer);

			FreeMemoryObject(m_LogicalDevice, m_VertexBufferMemory);
			DestroyBuffer(m_LogicalDevice, m_VertexBuffer);

			VulkanSample::Deinitialize();
		}
	}

};

VULKAN_SAMPLE_FRAMEWORK("TextureSample", 50, 25, 1280, 800, TextureSample)

