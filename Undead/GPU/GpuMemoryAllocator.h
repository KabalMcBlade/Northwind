#pragma once


#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "GpuDataStructure.h"


UD_NAMESPACE_BEGIN

UD_MEMORY_ALIGNED class GpuMemoryAllocator final
{
public:
	static uint32 FindMemoryType(const GpuMemoryCreateInfo& _memoryCreateInfo);

public:
	GpuMemoryAllocator();
	~GpuMemoryAllocator();

	GpuMemoryAllocation Alloc(const GpuMemoryCreateInfo& _createInfo);
	void Free(GpuMemoryAllocation& _gpuMemory);

	// accessor raw to the GPU memory allocator, if needs for something
	template<typename T>
	UD_INLINE T* New()
	{
		return eosNew(T, GetAllocator());
	}

	UD_INLINE void Delete(void* _ptr)
	{
		eosDelete(_ptr, GetAllocator());
	}

private:
	static uint32 CountBitsSet(uint32 _value);

private:
	friend class GpuMemoryManager;

	void Init(const VkPhysicalDevice& _physicalDevice, const VkDevice& _device, size _granularity);
	void Shutdown();

	using GpuAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static GpuAllocator* GetAllocator();

	static VkPhysicalDeviceMemoryProperties m_memProperties;

	eos::Vector<GpuMemoryList*, GpuAllocator, GetAllocator> m_gpuMemory[VK_MAX_MEMORY_TYPES];
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;
	size m_granularity;
};

UD_NAMESPACE_END