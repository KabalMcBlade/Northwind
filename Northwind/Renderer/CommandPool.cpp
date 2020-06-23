#include "CommandPool.h"

#include "../Core/Assertions.h"
#include "../GPU/GpuMemoryManager.h"


NW_NAMESPACE_BEGIN

CommandPool::CommandPool() : m_device(VK_NULL_HANDLE), m_commandPool(VK_NULL_HANDLE)
{

}

CommandPool::~CommandPool()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool CommandPool::Create(const VkDevice& _device)
{
	m_device = _device;
	return true;
}

void CommandPool::Destroy()
{
	if (m_commandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(m_device, m_commandPool, GpuMemoryManager::Instance().GetVK());
		m_commandPool = VK_NULL_HANDLE;
	}
}

bool CommandPool::Generate(uint32 _queueFamilyIndex, VkCommandPoolCreateFlags _flags)
{
	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = _queueFamilyIndex;
	createInfo.flags = _flags;

	VkResult result = vkCreateCommandPool(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_commandPool);

	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create CommandPool");
	return m_commandPool != VK_NULL_HANDLE;
}

NW_NAMESPACE_END
