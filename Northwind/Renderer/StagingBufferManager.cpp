#include "StagingBufferManager.h"

#include "../Core/SettingsDefines.h"
#include "../core/Assertions.h"
#include "../GPU/GpuMemoryManager.h"
// 

NW_NAMESPACE_BEGIN

StagingBufferManager& StagingBufferManager::Instance()
{
	static StagingBufferManager instance;
	return instance;
}

StagingBufferManager::StagingBufferManager() 
	: m_device(VK_NULL_HANDLE)
	, m_graphicsQueue(VK_NULL_HANDLE)
	, m_graphicsFamilyIndex(0)
	, m_maxBufferSize(0)
	, m_mappedData(nullptr)
	, m_commandPool(nullptr)
{
}

StagingBufferManager::~StagingBufferManager()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool StagingBufferManager::Create(VkDevice _device, VkQueue _graphicsQueue, int32 _graphicsFamilyIndex)
{
	m_memory = VK_NULL_HANDLE;
	m_commandPool = VK_NULL_HANDLE;
	m_mappedData = nullptr;

	m_maxBufferSize = SettingsDefines::Engine::kStagingBufferSize;
	m_device = _device;
	m_graphicsQueue = _graphicsQueue;
	m_graphicsFamilyIndex = _graphicsFamilyIndex;

	VkResult result;

	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = m_maxBufferSize;
		createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		m_buffer.m_vkOffset = 0;
		result = vkCreateBuffer(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_buffer.m_vkBuffer);
		nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create buffer for staging!");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(m_device, m_buffer.m_vkBuffer, &memoryRequirements);

	VkDeviceSize mask = memoryRequirements.alignment - 1;
	VkDeviceSize size = (memoryRequirements.size + mask) & ~mask;

	GpuMemoryCreateInfo createInfo = {};
	createInfo.m_size = memoryRequirements.size;
	createInfo.m_align = memoryRequirements.alignment;
	createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
	createInfo.m_usage = EMemoryUsage_CPU_to_GPU;
	createInfo.m_type = EGpuMemoryType_Buffer;

	{
		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = size;
		allocateInfo.memoryTypeIndex = GpuMemoryAllocator::FindMemoryType(createInfo);

		VkResult result = vkAllocateMemory(m_device, &allocateInfo, GpuMemoryManager::Instance().GetVK(), &m_memory);
		nwAssertReturnValue(result == VK_SUCCESS && m_memory != VK_NULL_HANDLE, false, "Cannot allocate memory!");
	}

	result = vkBindBufferMemory(m_device, m_buffer.m_vkBuffer, m_memory, 0);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot bind buffer for staging!");

	void* ptrMap = nullptr;
	result = vkMapMemory(m_device, m_memory, 0, size, 0, &ptrMap);
	nwAssertReturnValue(result == VK_SUCCESS && ptrMap != nullptr, false, "Cannot allocate memory!");

	m_mappedData = static_cast<uint8*>(ptrMap);
	nwAssertReturnValue(m_mappedData != nullptr, false, "Cannot map data!");

	{
		VkCommandPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = m_graphicsFamilyIndex;

		result = vkCreateCommandPool(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_commandPool);
		nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create command pool for staging!");
	}

	{
		VkCommandBufferAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = m_commandPool;
		allocateInfo.commandBufferCount = 1;

		VkFenceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		result = vkAllocateCommandBuffers(m_device, &allocateInfo, &m_buffer.m_vkCommandBuffer);
		nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot allocate command buffer for staging!");

		result = vkCreateFence(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_buffer.m_vkFence);
		nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create fence for staging!");

		result = vkBeginCommandBuffer(m_buffer.m_vkCommandBuffer, &commandBufferBeginInfo);
		nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot begin command buffer for staging!");

		m_buffer.m_data = (uint8*)m_mappedData;
	}

	return true;
}

void StagingBufferManager::Destroy()
{
	if (m_mappedData != nullptr)
	{
		vkUnmapMemory(m_device, m_memory);
		vkFreeMemory(m_device, m_memory, GpuMemoryManager::Instance().GetVK());

		m_mappedData = nullptr;

		vkDestroyFence(m_device, m_buffer.m_vkFence, GpuMemoryManager::Instance().GetVK());
		vkDestroyBuffer(m_device, m_buffer.m_vkBuffer, GpuMemoryManager::Instance().GetVK());

		vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_buffer.m_vkCommandBuffer);
		vkDestroyCommandPool(m_device, m_commandPool, GpuMemoryManager::Instance().GetVK());

		m_maxBufferSize = 0;
	}
}

uint8* StagingBufferManager::Stage(size _size, size _alignment, VkCommandBuffer& _outCommandBuffer, VkBuffer& _outBuffer, size& _outBufferOffset)
{
	nwAssertReturnValue(_size < m_maxBufferSize, nullptr, "Size is outbound of total memory!");

	StagingBuffer& stagingBuffer = m_buffer;

	VkDeviceSize mask = _alignment - 1;
	VkDeviceSize size = (stagingBuffer.m_vkOffset + mask) & ~mask;

	stagingBuffer.m_vkOffset = size;

	if ((stagingBuffer.m_vkOffset + _size) >= (m_maxBufferSize) && !stagingBuffer.m_submitted)
	{
		Submit();
	}

	stagingBuffer = m_buffer;
	if (stagingBuffer.m_submitted)
	{
		VkResult result = vkWaitForFences(m_device, 1, &stagingBuffer.m_vkFence, VK_TRUE, UINT64_MAX);
		nwAssertReturnValue(result == VK_SUCCESS, nullptr, "Cannot wait fences for staging!");

		result = vkResetFences(m_device, 1, &stagingBuffer.m_vkFence);
		nwAssertReturnValue(result == VK_SUCCESS, nullptr, "Cannot reset fences for staging!");

		stagingBuffer.m_vkOffset = 0;
		stagingBuffer.m_submitted = false;

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		result = vkBeginCommandBuffer(stagingBuffer.m_vkCommandBuffer, &commandBufferBeginInfo);
		nwAssertReturnValue(result == VK_SUCCESS, nullptr, "Cannot begin command buffer for staging!");
	}

	_outCommandBuffer = stagingBuffer.m_vkCommandBuffer;
	_outBuffer = stagingBuffer.m_vkBuffer;
	_outBufferOffset = stagingBuffer.m_vkOffset;

	uint8* data = stagingBuffer.m_data + stagingBuffer.m_vkOffset;
	stagingBuffer.m_vkOffset += _size;

	return data;
}

void StagingBufferManager::Submit()
{
	StagingBuffer& stagingBuffer = m_buffer;

	if (stagingBuffer.m_submitted || stagingBuffer.m_vkOffset == 0)
	{
		return;
	}

	VkMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
	vkCmdPipelineBarrier(stagingBuffer.m_vkCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 1, &barrier, 0, nullptr, 0, nullptr);

	vkEndCommandBuffer(stagingBuffer.m_vkCommandBuffer);

	VkMappedMemoryRange memoryRange = {};
	memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	memoryRange.memory = m_memory;
	memoryRange.size = VK_WHOLE_SIZE;
	vkFlushMappedMemoryRanges(m_device, 1, &memoryRange);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &stagingBuffer.m_vkCommandBuffer;

	vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, stagingBuffer.m_vkFence);

	stagingBuffer.m_submitted = true;
}

NW_NAMESPACE_END