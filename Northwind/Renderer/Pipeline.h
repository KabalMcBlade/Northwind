#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class RenderPass;
class Shader;
class PipelineLayout;
class VertexDescriptor;
class Pipeline final
{
public:
	Pipeline();
	~Pipeline();

	NW_INLINE operator const VkPipeline& () const { return m_pipeline; }
	NW_INLINE operator const VkPipelineBindPoint& () const { return m_bindPoint; }

	bool Create(const VkDevice& _device, VkPipelineBindPoint _bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);
	void Destroy();

	// --
	// Need to call all of them explicitly!
	void SetVertexInput(const VertexDescriptor& _vertexDescriptor);

	void SetTopology(VkPrimitiveTopology _topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkPipelineInputAssemblyStateCreateFlags _flags = 0, VkBool32 _primitiveRestartEnable = VK_FALSE);

	void SetPolygonMode(VkPolygonMode _polygonMode = VK_POLYGON_MODE_FILL, VkCullModeFlags _cullMode = VK_CULL_MODE_NONE, VkFrontFace _frontFace = VK_FRONT_FACE_CLOCKWISE, VkPipelineRasterizationStateCreateFlags _flags = 0);

	// if stencil op front is VK_COMPARE_OP_MAX_ENUM, it simple sets the front equal to the back
	void SetDepthStencil(VkBool32 _depthTestEnabled = VK_TRUE, VkBool32 _depthWriteEnabled = VK_TRUE, VkCompareOp _depthOp = VK_COMPARE_OP_LESS_OR_EQUAL, VkCompareOp _stencilBackOp = VK_COMPARE_OP_ALWAYS, VkCompareOp _stencilFrontOp = VK_COMPARE_OP_MAX_ENUM);

	void SetViewport(uint32 _viewportCount = 1, uint32 _scissorCount = 1, VkPipelineViewportStateCreateFlags _flags = 0);

	void SetMultisample(VkSampleCountFlagBits _samples = VK_SAMPLE_COUNT_1_BIT, VkPipelineMultisampleStateCreateFlags _flags = 0);

	// dynamic state combo functions
	void PushDynamic(VkDynamicState _dynamicState);
	void SetDynamics(VkPipelineDynamicStateCreateFlags _flags = 0);

	// color blend attachments state combo functions
	// must be 1 per pass
	void PushColorBlendAttachment(VkColorComponentFlags _colorWriteMask = 0xF, VkBool32 _blendEnable = VK_FALSE, VkBlendFactor _colorSourceBlend = VK_BLEND_FACTOR_ONE, VkBlendFactor _colorDestBlend = VK_BLEND_FACTOR_ZERO, VkBlendOp _colorOp = VK_BLEND_OP_ADD, VkBlendFactor _alphaSourceBlend = VK_BLEND_FACTOR_ONE, VkBlendFactor _alphaDestBlend = VK_BLEND_FACTOR_ZERO, VkBlendOp _alphaOp = VK_BLEND_OP_ADD);
	void SetColorBlendAttachments();
	// --

	void SetLayout(const PipelineLayout& _layout);
	void SetRenderPass(const RenderPass& _renderPass);
	void SetCache(VkPipelineCache _cache);

	void PushShader(const Shader& _shader);

private:
	using PipelineAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static PipelineAllocator* GetAllocator();

	const PipelineLayout* m_layout;
	const RenderPass* m_renderPass;

	VkDevice m_device;

	VkPipeline m_pipeline;
	VkPipelineBindPoint m_bindPoint;
	VkPipelineCache m_cache;

	VkPipelineVertexInputStateCreateInfo m_vertexInput;
	VkPipelineInputAssemblyStateCreateInfo m_inputAssembly;
	VkPipelineRasterizationStateCreateInfo m_rasterization;
	VkPipelineDepthStencilStateCreateInfo m_depthStencil;
	VkPipelineViewportStateCreateInfo m_viewPort;
	VkPipelineMultisampleStateCreateInfo m_multiSample;
	VkPipelineDynamicStateCreateInfo m_dynamic;
	VkPipelineColorBlendStateCreateInfo m_colorBlend;

	eos::Vector<VkDynamicState, PipelineAllocator, GetAllocator> m_dynamicStates;
	eos::Vector<VkPipelineColorBlendAttachmentState, PipelineAllocator, GetAllocator> m_colorBlendAttachment;

	eos::Vector<VkPipelineShaderStageCreateInfo, PipelineAllocator, GetAllocator> m_shaderStages;
};

NW_NAMESPACE_END