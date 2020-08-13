#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

struct StagingBuffer final
{
	StagingBuffer() :
		m_submitted(false),
		m_vkCommandBuffer(VK_NULL_HANDLE),
		m_vkBuffer(VK_NULL_HANDLE),
		m_vkFence(VK_NULL_HANDLE),
		m_vkOffset(0),
		m_data(nullptr) {}

	bool m_submitted;
	VkCommandBuffer m_vkCommandBuffer;
	VkBuffer m_vkBuffer;
	VkFence m_vkFence;
	VkDeviceSize m_vkOffset;
	uint8* m_data;
};

class NW_DLL StagingBufferManager final : public eos::NoCopyableMoveable
{
public:
	static StagingBufferManager& Instance();

	bool Create(VkDevice _device, VkQueue _graphicsQueue, int32 _graphicsFamilyIndex);
	void Destroy();

	uint8* Stage(size _size, size _alignment, VkCommandBuffer& _outCommandBuffer, VkBuffer& _outBuffer, size& _outBufferOffset);
	uint8* Stage(size _size, VkCommandBuffer& _outCommandBuffer, VkBuffer& _outBuffer, size& _outBufferOffset);

	// Call at the beginning of the frame! Before Render frame!
	// Call also when the window/surface is recreated.
	void Submit();

private:
	StagingBufferManager();
	~StagingBufferManager();

	friend class RenderManager;

	VkDevice m_device;
	VkQueue m_graphicsQueue;
	int32 m_graphicsFamilyIndex;
	size m_maxBufferSize;
	size m_defaultAlignment;
	uint8* m_mappedData;
	VkDeviceMemory m_memory;
	VkCommandPool m_commandPool;
	StagingBuffer m_buffer;
};

NW_NAMESPACE_END

