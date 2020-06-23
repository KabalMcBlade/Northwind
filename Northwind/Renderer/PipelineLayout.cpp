#include "PipelineLayout.h"

#include "DescriptorSetLayout.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"
#include "../GPU/GpuMemoryManager.h"


NW_NAMESPACE_BEGIN

PipelineLayout::PipelineLayoutAllocator* PipelineLayout::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kPipelineLayoutAllocatorSize);
	static PipelineLayoutAllocator memoryAllocator(memoryArea, "PipelineLayoutAllocator");

	return &memoryAllocator;
}

PipelineLayout::PipelineLayout() : m_device(VK_NULL_HANDLE), m_pipelineLayout(VK_NULL_HANDLE)
{
}


PipelineLayout::~PipelineLayout()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool PipelineLayout::Create(const VkDevice& _device, const DescriptorSetLayout& _descriptorSetLayout)
{
	m_device = _device;

	VkPipelineLayoutCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.setLayoutCount = 1;
	createInfo.pSetLayouts = _descriptorSetLayout;

	if (m_pushContant.size() > 0)
	{
		createInfo.pushConstantRangeCount = static_cast<uint32>(m_pushContant.size());
		createInfo.pPushConstantRanges = m_pushContant.data();
	}

	VkResult result = vkCreatePipelineLayout(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_pipelineLayout);

	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create PipelineLayout");
	return m_pipelineLayout != VK_NULL_HANDLE;

	return true;
}

void PipelineLayout::Destroy()
{
	if (m_pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(m_device, m_pipelineLayout, GpuMemoryManager::Instance().GetVK());
		m_pipelineLayout = VK_NULL_HANDLE;
	}
}

void PipelineLayout::PushConstant(VkShaderStageFlags _stage, uint32 _offset, uint32 _size)
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = _stage;
	pushConstantRange.offset = _offset;
	pushConstantRange.size = _size;

	m_pushContant.push_back(pushConstantRange);
}


NW_NAMESPACE_END