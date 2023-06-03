#include "Buffers.h"

#include "GraphicsPipeline.h"




void GraphicsPipeline::createRenderPass()
{
	mDebugPrint("Creating render pass...");

	// Attachment setup
	VkAttachmentDescription colorAttachment{
		.format = *m_pSwapchain->getSwapchainImageFormat(),
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	// Attachment reference
	VkAttachmentReference colorAttachmentRef{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	// Subpass
	VkSubpassDescription subpass{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentRef
	};

	// Subpass dependency
	VkSubpassDependency dependency{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	};

	// Render pass
	VkRenderPassCreateInfo renderPassInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &colorAttachment,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};

	if (vkCreateRenderPass(*m_pLogicalDevice, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}


}

void GraphicsPipeline::createDescriptorSetLayout()
{
	mDebugPrint("Creating uniform buffer descriptor set layout...");
	VkDescriptorSetLayoutBinding uboLayoutBinding{
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.pImmutableSamplers = nullptr
	};

	mDebugPrint("Creating texture sampler descriptor set layout...");
	VkDescriptorSetLayoutBinding textureSamplerLayoutBinding{
		.binding = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.pImmutableSamplers = nullptr
	};

	/*
	mDebugPrint("Creating heightmap sampler descriptor set layout...");
	VkDescriptorSetLayoutBinding heightSamplerLayoutBinding{
		.binding = 2,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.pImmutableSamplers = nullptr
	};
	*/

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, textureSamplerLayoutBinding /*, heightSamplerLayoutBinding */};

	VkDescriptorSetLayoutCreateInfo layoutInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};

	if (vkCreateDescriptorSetLayout(*m_pLogicalDevice, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layouts!");
	}
}

void GraphicsPipeline::createGraphicsPipeline()
{
	mDebugPrint("Creating graphics pipeline layout...");

	auto vertShaderCode = m_pUtilities->readFile("shaders/vert.spv");
	auto fragShaderCode = m_pUtilities->readFile("shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	mDebugPrint("Creating shader stages...");
	// Vertex shader stage
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vertShaderModule,
		.pName = "main"
	};

	// Fragment shader stage
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = fragShaderModule,
		.pName = "main"
	};

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	auto bindingDescription = VertexBuffer::sVertex::getBindingDescription();
	auto attributeDescriptions = VertexBuffer::sVertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &bindingDescription,
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
		.pVertexAttributeDescriptions = attributeDescriptions.data()
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	/*
	* MOVED TO CommandBuffer::createCommandBuffers()
	mDebugPrint("Creating viewport...");
	VkViewport viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)m_pSwapchain->getSwapchainExtent()->width,
		.height = (float)m_pSwapchain->getSwapchainExtent()->height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	
	mDebugPrint("Creating scissor...");
	VkRect2D scissor{
		.offset = { 0, 0 },
		.extent = *m_pSwapchain->getSwapchainExtent()
	};
	*/

	mDebugPrint("Creating viewport state...");
	VkPipelineViewportStateCreateInfo viewportState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = nullptr, // Dynamic
		.scissorCount = 1,
		.pScissors = nullptr // Dynamic
	};

	mDebugPrint("Creating rasterizer...");
	//Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		//rasterizer.depthClampEnable = VK_FALSE;
		.rasterizerDiscardEnable = VK_FALSE,
		//rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f, // Optional
		.depthBiasClamp = 0.0f, // Optional
		.depthBiasSlopeFactor = 0.0f, // Optional
		.lineWidth = m_pGraphicsSettings->wireframeThickness
	};
	m_pGraphicsSettings->rasterizerDepthClamp == true ? rasterizer.depthClampEnable = VK_TRUE : rasterizer.depthClampEnable = VK_FALSE;
	m_pGraphicsSettings->wireframe == true ? rasterizer.polygonMode = VK_POLYGON_MODE_LINE : rasterizer.polygonMode = VK_POLYGON_MODE_FILL;

	mDebugPrint("Creating multisampler...");
	VkPipelineMultisampleStateCreateInfo multisampling{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		//multisampling.sampleShadingEnable = VK_FALSE;
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.minSampleShading = 1.0f, // Optional
		.pSampleMask = nullptr, // Optional
		.alphaToCoverageEnable = VK_FALSE, // Optional
		.alphaToOneEnable = VK_FALSE, // Optional
	};
	m_pGraphicsSettings->multisampling == true ? multisampling.sampleShadingEnable = VK_TRUE : multisampling.sampleShadingEnable = VK_FALSE;

	mDebugPrint("Creating colour blender...");
	VkPipelineColorBlendAttachmentState colorBlendAttachment{
		.blendEnable = VK_TRUE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
	/*
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
	*/

	VkPipelineColorBlendStateCreateInfo colorBlending{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY, // Optional
		.attachmentCount = 1,
		.pAttachments = &colorBlendAttachment
	};
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional


	std::vector<VkDynamicState> dynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data()
	};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 1, // Optional
		.pSetLayouts = &m_descriptorSetLayout,
		.pushConstantRangeCount = 0, // Optional 
		.pPushConstantRanges = nullptr // Optional
	};

	if (vkCreatePipelineLayout(*m_pLogicalDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}



	// Pipeline
	mDebugPrint("Creating pipeline...");
	VkGraphicsPipelineCreateInfo pipelineInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = 2,
		.pStages = shaderStages,
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &inputAssembly,
		//pTessellationState = nullptr, // Optional
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,
		.pDepthStencilState = nullptr, // Optional
		.pColorBlendState = &colorBlending,
		.pDynamicState = &dynamicState,
		// Fixed Functions
		.layout = m_pipelineLayout,
		// Render Pass
		.renderPass = m_renderPass,
		.subpass = 0,
		// Derivatives
		.basePipelineHandle = VK_NULL_HANDLE, // Optional
		.basePipelineIndex = -1 // Optional
	};


	if (vkCreateGraphicsPipelines(*m_pLogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}


	vkDestroyShaderModule(*m_pLogicalDevice, vertShaderModule, nullptr);
	vkDestroyShaderModule(*m_pLogicalDevice, fragShaderModule, nullptr);
}

void GraphicsPipeline::cleanup()
{
	vkDestroyDescriptorSetLayout(*m_pLogicalDevice, m_descriptorSetLayout, nullptr);
	vkDestroyPipeline(*m_pLogicalDevice, m_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(*m_pLogicalDevice, m_pipelineLayout, nullptr);
	vkDestroyRenderPass(*m_pLogicalDevice, m_renderPass, nullptr);
}



VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = code.size(),
		.pCode = reinterpret_cast<const uint32_t*>(code.data())
	};

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(*m_pLogicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}