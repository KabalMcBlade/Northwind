#include "DescriptorSet.h"

#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"


NW_NAMESPACE_BEGIN

DescriptorSet::DescriptorSetAllocator* DescriptorSet::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kDescriptorSetAllocatorSize);
	static DescriptorSetAllocator memoryAllocator(memoryArea, "DescriptorSetAllocator");

	return &memoryAllocator;
}

DescriptorSet::DescriptorSet() : m_device(VK_NULL_HANDLE), m_pool(nullptr), m_descriptorSet(VK_NULL_HANDLE)
{
}

DescriptorSet::~DescriptorSet()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool DescriptorSet::Create(const VkDevice& _device, const DescriptorPool& _descriptorPool)
{
	m_device = _device;
	m_pool = &_descriptorPool;
	return true;
}

void DescriptorSet::Destroy()
{
	// Nothing here
}

bool DescriptorSet::Generate()
{
	VkDescriptorSetAllocateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	createInfo.descriptorPool = *m_pool;
	createInfo.pSetLayouts = m_layouts.data();
	createInfo.descriptorSetCount = static_cast<uint32>(m_layouts.size());

	VkResult result = vkAllocateDescriptorSets(m_device, &createInfo, &m_descriptorSet);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create DescriptorSet");

	for (auto& descriptor : m_descriptors)
	{
		descriptor.dstSet = m_descriptorSet;
	}

	vkUpdateDescriptorSets(m_device, static_cast<uint32>(m_descriptors.size()), m_descriptors.data(), 0, nullptr);

	return true;
}

void DescriptorSet::PushLayout(const DescriptorSetLayout& _layout)
{
	m_layouts.push_back(_layout);
}

void DescriptorSet::PushDescriptor(VkWriteDescriptorSet _descriptor)
{
	m_descriptors.push_back(_descriptor);
}

void DescriptorSet::PushDescriptor(uint32 _binding, VkDescriptorType _type, const VkDescriptorBufferInfo* _bufferInfo, uint32 _descriptorCount /*= 1*/)
{
	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.descriptorType = _type;
	writeDescriptorSet.dstBinding = _binding;
	writeDescriptorSet.pBufferInfo = _bufferInfo;
	writeDescriptorSet.descriptorCount = _descriptorCount;
	m_descriptors.push_back(writeDescriptorSet);
}

void DescriptorSet::PushDescriptor(uint32 _binding, VkDescriptorType _type, const VkDescriptorImageInfo* _imageInfo, uint32 _descriptorCount /*= 1*/)
{
	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.descriptorType = _type;
	writeDescriptorSet.dstBinding = _binding;
	writeDescriptorSet.pImageInfo = _imageInfo;
	writeDescriptorSet.descriptorCount = _descriptorCount;
	m_descriptors.push_back(writeDescriptorSet);
}

NW_NAMESPACE_END