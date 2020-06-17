#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "VulkanAllocator.h"
#include "GpuMemoryAllocator.h"


UD_NAMESPACE_BEGIN

class UD_DLL GpuMemoryManager final : public eos::NoCopyableMoveable
{
public:
// 	enum EMemoryType
// 	{
// 		EMemoryType_Vulkan,
// 		EMemoryType_GPU
// 	};

	static GpuMemoryManager& Instance();

	void Init(const VkPhysicalDevice& _physicalDevice, const VkDevice& _device, size _granularity);
	void Shutdown();

	UD_INLINE const VkAllocationCallbacks* GetVK() const { return &m_vulkanAllocator.GetCallbacks(); }

	UD_INLINE const GpuMemoryAllocator* GetGPU() const { return &m_gpuAllocator; }
	UD_INLINE GpuMemoryAllocator* GetGPU() { return &m_gpuAllocator; }

private:
	GpuMemoryManager() {}
	~GpuMemoryManager() {}

private:
	VulkanAllocator m_vulkanAllocator;
	GpuMemoryAllocator m_gpuAllocator;
};

UD_NAMESPACE_END