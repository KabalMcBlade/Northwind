#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Assertions.h"

#include "GpuMemoryManager.h"


NW_NAMESPACE_BEGIN

NW_INLINE VkDeviceMemory vkGpuAllocateMemory(const VkDevice& _device, const VkMemoryAllocateInfo& _allocateInfo)
{
	VkDeviceMemory deviceMemoryAllocated = VK_NULL_HANDLE;

	VkResult result = vkAllocateMemory(_device, &_allocateInfo, GpuMemoryManager::Instance().GetVK(), &deviceMemoryAllocated);
	nwAssertReturnValue(result == VK_SUCCESS, VK_NULL_HANDLE, "Cannot allocate memory!");

	return deviceMemoryAllocated;
}

NW_INLINE void* vkGpuMapMemory(const VkDevice& _device, const VkDeviceMemory& _deviceMemoryAllocated, VkDeviceSize _offset, VkDeviceSize _size, VkMemoryMapFlags _flags)
{
	void* data = nullptr;

	VkResult result = vkMapMemory(_device, _deviceMemoryAllocated, _offset, _size, _flags, &data);
	nwAssertReturnValue(result == VK_SUCCESS, nullptr, "Cannot map memory!");

	return data;
}

NW_INLINE void vkGpuUnmapMemory(const VkDevice& _device, const VkDeviceMemory& _deviceMemoryAllocated)
{
	vkUnmapMemory(_device, _deviceMemoryAllocated);
}


NW_INLINE void vkGpuFreeMemory(const VkDevice& _device, VkDeviceMemory& _deviceMemoryAllocated)
{
	vkFreeMemory(_device, _deviceMemoryAllocated, GpuMemoryManager::Instance().GetVK());
	_deviceMemoryAllocated = VK_NULL_HANDLE;
}

NW_NAMESPACE_END