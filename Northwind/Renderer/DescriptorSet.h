#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class DescriptorPool;
class DescriptorSetLayout;
class DescriptorSet final
{
public:
	DescriptorSet();
	~DescriptorSet();

	bool Create(const VkDevice& _device, const DescriptorPool& _descriptorPool);
	void Destroy();

	NW_INLINE operator const VkDescriptorSet& () const
	{
		return m_descriptorSet;
	}

	NW_INLINE operator const VkDescriptorSet* () const
	{
		return &m_descriptorSet;
	}

	void PushLayout(const DescriptorSetLayout& _layout);
	void PushDescriptor(VkWriteDescriptorSet _descriptor);
	void PushDescriptor(uint32 _binding, VkDescriptorType _type, const VkDescriptorBufferInfo* _bufferInfo, uint32 _descriptorCount = 1);
	void PushDescriptor(uint32 _binding, VkDescriptorType _type, const VkDescriptorImageInfo* _imageInfo, uint32 _descriptorCount = 1);

private:
	using DescriptorSetAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static DescriptorSetAllocator* GetAllocator();

	VkDevice m_device;

	VkDescriptorSet m_descriptorSet;

	eos::Vector<VkDescriptorSetLayout, DescriptorSetAllocator, GetAllocator> m_layouts;
	eos::Vector<VkWriteDescriptorSet, DescriptorSetAllocator, GetAllocator> m_descriptors;
};

NW_NAMESPACE_END