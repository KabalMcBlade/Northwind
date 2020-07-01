#include "Pipeline.h"


#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"

#include "../GPU/GpuMemoryManager.h"

#include "../Shaders/Shader.h"

#include "PipelineLayout.h"
#include "RenderPass.h"
#include "VertexDescriptor.h"


NW_NAMESPACE_BEGIN

Pipeline::PipelineAllocator* Pipeline::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kPipelineAllocatorSize);
	static PipelineAllocator memoryAllocator(memoryArea, "PipelineAllocator");

	return &memoryAllocator;
}

Pipeline::Pipeline()
	: m_device(VK_NULL_HANDLE)
	, m_pipeline(VK_NULL_HANDLE)
	, m_bindPoint(VK_PIPELINE_BIND_POINT_MAX_ENUM)
	, m_layout(nullptr)
	, m_renderPass(nullptr)
{
}

Pipeline::~Pipeline()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool Pipeline::Create(const VkDevice& _device, VkPipelineBindPoint _bindPoint /*= VK_PIPELINE_BIND_POINT_GRAPHICS*/)
{
	nwAssertReturnValue(m_layout, false, "PipelineLayout is null!");
	nwAssertReturnValue(m_renderPass, false, "RenderPass is null!");

	m_device = _device;
	m_bindPoint = _bindPoint;

	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	createInfo.stageCount = static_cast<uint32>(m_shaderStages.size());
	createInfo.pStages = m_shaderStages.data();
	createInfo.layout = m_layout->GetLayout();
	createInfo.renderPass = m_renderPass->GetRenderPass();

	createInfo.pVertexInputState = &m_vertexInput;
	createInfo.pInputAssemblyState = &m_inputAssembly;
	createInfo.pRasterizationState = &m_rasterization;
	createInfo.pColorBlendState = &m_colorBlend;
	createInfo.pMultisampleState = &m_multiSample;
	createInfo.pViewportState = &m_viewPort;
	createInfo.pDepthStencilState = &m_depthStencil;
	createInfo.pDynamicState = &m_dynamic;

	VkResult result = vkCreateGraphicsPipelines(m_device, m_cache, 1, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_pipeline);

	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot generate Pipeline!");

	return true;
}

void Pipeline::Destroy()
{
	if (m_pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(m_device, m_pipeline, GpuMemoryManager::Instance().GetVK());
	}
}

void Pipeline::SetVertexInput(const VertexDescriptor& _vertexDescriptor)
{
	m_vertexInput = _vertexDescriptor;
}

void Pipeline::SetTopology(VkPrimitiveTopology _topology /*= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST*/, VkPipelineInputAssemblyStateCreateFlags _flags /*= 0*/, VkBool32 _primitiveRestartEnable /*= VK_FALSE*/)
{
	m_inputAssembly = {};
	m_inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_inputAssembly.topology = _topology;
	m_inputAssembly.flags = _flags;
	m_inputAssembly.primitiveRestartEnable = _primitiveRestartEnable;
}

void Pipeline::SetPolygonMode(VkPolygonMode _polygonMode /*= VK_POLYGON_MODE_FILL*/, VkCullModeFlags _cullMode /*= VK_CULL_MODE_NONE*/, VkFrontFace _frontFace /*= VK_FRONT_FACE_CLOCKWISE*/, VkPipelineRasterizationStateCreateFlags _flags /*= 0*/)
{
	m_rasterization = {};
	m_rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_rasterization.polygonMode = _polygonMode;
	m_rasterization.cullMode = _cullMode;
	m_rasterization.frontFace = _frontFace;
	m_rasterization.flags = _flags;
	m_rasterization.depthClampEnable = VK_FALSE;
	m_rasterization.lineWidth = 1.0f;
}

void Pipeline::SetDepthStencil(VkBool32 _depthTestEnabled /*= VK_TRUE*/, VkBool32 _depthWriteEnabled /*= VK_TRUE*/, VkCompareOp _depthOp /*= VK_COMPARE_OP_LESS_OR_EQUAL*/, VkCompareOp _stencilBackOp /*= VK_COMPARE_OP_ALWAYS*/, VkCompareOp _stencilFrontOp /*= VK_COMPARE_OP_MAX_ENUM*/)
{
	m_depthStencil = {};
	m_depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_depthStencil.depthTestEnable = _depthTestEnabled;
	m_depthStencil.depthWriteEnable = _depthWriteEnabled;
	m_depthStencil.depthCompareOp = _depthOp;
	m_depthStencil.back.compareOp = _stencilBackOp;
	if (_stencilFrontOp == VK_COMPARE_OP_MAX_ENUM)
	{
		m_depthStencil.front = m_depthStencil.back;
	}
	else
	{
		m_depthStencil.front.compareOp = _stencilFrontOp;
	}
}

void Pipeline::SetViewport(uint32 _viewportCount /*= 1*/, uint32 _scissorCount /*= 1*/, VkPipelineViewportStateCreateFlags _flags /*= 0*/)
{
	m_viewPort = {};
	m_viewPort.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_viewPort.viewportCount = _viewportCount;
	m_viewPort.scissorCount = _scissorCount;
	m_viewPort.flags = _flags;
}

void Pipeline::SetMultisample(VkSampleCountFlagBits _samples /*= VK_SAMPLE_COUNT_1_BIT*/, VkPipelineMultisampleStateCreateFlags _flags /*= 0*/)
{
	m_multiSample = {};
	m_multiSample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_multiSample.rasterizationSamples = _samples;
	m_multiSample.flags = _flags;
}

void Pipeline::PushDynamic(VkDynamicState _dynamicState)
{
	m_dynamicStates.push_back(_dynamicState);
}

void Pipeline::SetDynamics(VkPipelineDynamicStateCreateFlags _flags /*= 0*/)
{
	m_dynamic = {};
	m_dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	m_dynamic.pDynamicStates = m_dynamicStates.data();
	m_dynamic.dynamicStateCount = static_cast<uint32>(m_dynamicStates.size());
	m_dynamic.flags = _flags;
}

void Pipeline::PushColorBlendAttachment(VkColorComponentFlags _colorWriteMask /*= 0xF*/, VkBool32 _blendEnable /*= VK_FALSE*/, VkBlendFactor _colorSourceBlend /*= VK_BLEND_FACTOR_ONE*/, VkBlendFactor _colorDestBlend /*= VK_BLEND_FACTOR_ZERO*/, VkBlendOp _colorOp /*= VK_BLEND_OP_ADD*/, VkBlendFactor _alphaSourceBlend /*= VK_BLEND_FACTOR_ONE*/, VkBlendFactor _alphaDestBlend /*= VK_BLEND_FACTOR_ZERO*/, VkBlendOp _alphaOp /*= VK_BLEND_OP_ADD*/)
{
	VkPipelineColorBlendAttachmentState colorBlend{};

	colorBlend.colorWriteMask = _colorWriteMask;
	colorBlend.blendEnable = _blendEnable;

	colorBlend.srcColorBlendFactor = _colorSourceBlend;
	colorBlend.dstColorBlendFactor = _colorSourceBlend;
	colorBlend.colorBlendOp = _colorOp;
	colorBlend.srcAlphaBlendFactor = _alphaSourceBlend;
	colorBlend.dstAlphaBlendFactor = _alphaDestBlend;
	colorBlend.alphaBlendOp = _alphaOp;

	m_colorBlendAttachment.push_back(colorBlend);
}

void Pipeline::SetColorBlendAttachments()
{
	m_colorBlend = {};
	m_colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_colorBlend.pAttachments = m_colorBlendAttachment.data();
	m_colorBlend.attachmentCount = static_cast<uint32>(m_colorBlendAttachment.size());
}


void Pipeline::SetLayout(const PipelineLayout& _layout)
{
	m_layout = &_layout;
}

void Pipeline::SetRenderPass(const RenderPass& _renderPass)
{
	m_renderPass = &_renderPass;
}


void Pipeline::SetCache(VkPipelineCache _cache)
{
	m_cache = _cache;
}

void Pipeline::PushShader(const Shader& _shader)
{
	VkPipelineShaderStageCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage = _shader;
	createInfo.pName = "main";
	createInfo.module = _shader;

	m_shaderStages.push_back(createInfo);
}


NW_NAMESPACE_END