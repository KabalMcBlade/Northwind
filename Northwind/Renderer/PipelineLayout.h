#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class DescriptorSetLayout;
class PipelineLayout final
{
public:
	PipelineLayout();
	~PipelineLayout();

	bool Create(const VkDevice& _device, const DescriptorSetLayout& _descriptorSetLayout);
	void Destroy();

	NW_INLINE operator const VkPipelineLayout& () const
	{
		return m_pipelineLayout;
	}

	NW_INLINE const VkPipelineLayout& GetLayout() const
	{
		return m_pipelineLayout;
	}

	NW_INLINE const size GetPushConstantSize() const
	{
		return m_pushContant.size();
	}

	NW_INLINE const VkPushConstantRange GetPushConstant(uint32 _index) const
	{
		return m_pushContant[_index];
	}

	void PushConstant(VkShaderStageFlags _stage, uint32 _offset, uint32 _size);

private:
	using PipelineLayoutAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static PipelineLayoutAllocator* GetAllocator();

	VkDevice m_device;
	VkPipelineLayout m_pipelineLayout;
	eos::Vector<VkPushConstantRange, PipelineLayoutAllocator, GetAllocator> m_pushContant;
};

NW_NAMESPACE_END
