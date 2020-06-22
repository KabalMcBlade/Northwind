#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "QueueFamily.h"


NW_NAMESPACE_BEGIN

class DescriptorPool final
{
public:
	DescriptorPool();
	~DescriptorPool();

	bool Create(const VkDevice& _device);
	void Destroy();

	NW_INLINE operator const VkDescriptorPool& () const
	{
		return m_descriptorPool;
	}

	void Push(VkDescriptorType _type, uint32 _descriptorCount);
	bool Generate(VkDescriptorPoolCreateFlags _flag = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, uint32 _maxDescriptorSet = 16384);

private:
	using DescriptorPoolAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static DescriptorPoolAllocator* GetAllocator();

	VkDevice m_device;
	VkDescriptorPool m_descriptorPool;
	eos::Vector<VkDescriptorPoolSize, DescriptorPoolAllocator, DescriptorPool::GetAllocator> m_poolSizes;
};

NW_NAMESPACE_END