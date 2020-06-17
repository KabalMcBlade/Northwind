#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/Assertions.h"

#include "GpuMemoryManager.h"


UD_NAMESPACE_BEGIN

UD_INLINE VkDeviceMemory vkGpuAllocateMemory(const VkDevice& _device, const VkMemoryAllocateInfo& _allocateInfo)
{
	VkDeviceMemory deviceMemoryAllocated = VK_NULL_HANDLE;

	VkResult result = vkAllocateMemory(_device, &_allocateInfo, GpuMemoryManager::Instance().GetVK(), &deviceMemoryAllocated);
	udAssertReturnValue(result == VK_SUCCESS, VK_NULL_HANDLE, "Cannot allocate memory!");

	return deviceMemoryAllocated;
}

UD_INLINE void* vkGpuMapMemory(const VkDevice& _device, const VkDeviceMemory& _deviceMemoryAllocated, VkDeviceSize _offset, VkDeviceSize _size, VkMemoryMapFlags _flags)
{
	void* data = nullptr;

	VkResult result = vkMapMemory(_device, _deviceMemoryAllocated, _offset, _size, _flags, &data);
	udAssertReturnValue(result == VK_SUCCESS, nullptr, "Cannot map memory!");

	return data;
}

UD_INLINE void vkGpuUnmapMemory(const VkDevice& _device, const VkDeviceMemory& _deviceMemoryAllocated)
{
	vkUnmapMemory(_device, _deviceMemoryAllocated);
}


UD_INLINE void vkGpuFreeMemory(const VkDevice& _device, VkDeviceMemory& _deviceMemoryAllocated)
{
	vkFreeMemory(_device, _deviceMemoryAllocated, GpuMemoryManager::Instance().GetVK());
	_deviceMemoryAllocated = VK_NULL_HANDLE;
}

UD_NAMESPACE_END