#include "DescriptorPool.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"
#include "../GPU/GpuMemoryManager.h"


NW_NAMESPACE_BEGIN

DescriptorPool::DescriptorPoolAllocator* DescriptorPool::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kDescriptorPoolAllocatorSize);
	static DescriptorPoolAllocator memoryAllocator(memoryArea, "DescriptorPoolAllocator");

	return &memoryAllocator;
}

//////////////////////////////////////////////////////////////////////////

DescriptorPool::DescriptorPool() : m_device(VK_NULL_HANDLE), m_descriptorPool(VK_NULL_HANDLE)
{

}

DescriptorPool::~DescriptorPool()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool DescriptorPool::Create(const VkDevice& _device)
{
	m_device = _device;
	return true;
}

void DescriptorPool::Destroy()
{
	if (m_descriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(m_device, m_descriptorPool, GpuMemoryManager::Instance().GetVK());
		m_descriptorPool = VK_NULL_HANDLE;
	}
}

void DescriptorPool::Push(VkDescriptorType _type, uint32 _descriptorCount)
{
	VkDescriptorPoolSize descriptorPoolSize{};
	descriptorPoolSize.type = _type;
	descriptorPoolSize.descriptorCount = _descriptorCount;
	m_poolSizes.push_back(descriptorPoolSize);
}

bool DescriptorPool::Generate(VkDescriptorPoolCreateFlags _flag /*= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT*/, uint32 _maxDescriptorSet /*= 16384*/)
{
	VkDescriptorPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = static_cast<uint32>(m_poolSizes.size());
	createInfo.pPoolSizes = m_poolSizes.data();
	createInfo.maxSets = _maxDescriptorSet;
	createInfo.flags = _flag;

	VkResult result = vkCreateDescriptorPool(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_descriptorPool);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create DescriptorPool");

	return m_descriptorPool != VK_NULL_HANDLE;
}

NW_NAMESPACE_END