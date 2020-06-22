#include "GpuMemoryManager.h"

// #include "../Core/MemoryWrapper.h"
// #include "../Core/SettingsDefines.h"
// #include "../Core/Assertions.h"


NW_NAMESPACE_BEGIN

GpuMemoryManager& GpuMemoryManager::Instance()
{
	static GpuMemoryManager instance;
	return instance;
}

void GpuMemoryManager::Init(const VkPhysicalDevice& _physicalDevice, const VkDevice& _device, size _granularity)
{
	m_gpuAllocator.Init(_physicalDevice, _device, _granularity);
}

void GpuMemoryManager::Shutdown()
{
	m_gpuAllocator.Shutdown();
}

NW_NAMESPACE_END
