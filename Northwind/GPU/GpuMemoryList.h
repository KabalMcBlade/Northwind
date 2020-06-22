#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "GpuDataStructure.h"


NW_NAMESPACE_BEGIN

NW_MEMORY_ALIGNED class GpuMemoryList final
{
public:
	GpuMemoryList(const VkDevice _device, uint32 _memorytypeIndex, VkDeviceSize _size, EMemoryUsage _usage);
	~GpuMemoryList();

	bool Alloc(size _size, size _align, VkDeviceSize _granularity, EGpuMemoryType _gpuType, GpuMemoryAllocation& _gpuMemory);
	void Free(GpuMemoryAllocation& _gpuMemory);

	NW_INLINE bool IsHostVisible() const { return m_usage != EMemoryUsage_GPU; }
	NW_INLINE uint32 GetMemoryTypeIndex() const { return m_memoryTypeIndex; }
	NW_INLINE VkDeviceSize GetAllocatedDeviceSize() const { return m_allocated; }

private:
	// for granularity check link: https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html
	// and search for: Buffer-Image Granularity
	bool HasGranularityConflict(EGpuMemoryType _memoryType0, VkDeviceSize _offset0, VkDeviceSize _size0, EGpuMemoryType _memoryType1, VkDeviceSize _offset1, VkDeviceSize _pageSize);


	NW_MEMORY_ALIGNED struct GpuMemoryNode final
	{
		uint32              m_index;
		VkDeviceSize        m_size;
		VkDeviceSize        m_offset;
		EGpuMemoryType      m_gpuMemoryType;
	};

	using MemoryNode = typename NW_MEMORY_ALIGNED eos::DoublyLinkedList<GpuMemoryNode>::Node;

	VkDevice m_device;
	eos::DoublyLinkedList<GpuMemoryNode> m_nodeList;
	VkDeviceMemory m_deviceMemory;
	VkDeviceSize m_totalSize;
	VkDeviceSize m_allocated;
	EMemoryUsage m_usage;
	uint32 m_memoryTypeIndex;
	uint32 m_nextNodeIndex;
	uint8* m_memoryBuffer;
};

NW_NAMESPACE_END