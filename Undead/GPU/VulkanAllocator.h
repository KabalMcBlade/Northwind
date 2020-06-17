#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


UD_NAMESPACE_BEGIN

class UD_DLL VulkanAllocator final : public eos::NoCopyableMoveable
{
public:
	VulkanAllocator() 
	{
		m_callback.pUserData = (void*)this;
		m_callback.pfnAllocation = &VulkanAllocator::Allocation;
		m_callback.pfnReallocation = &VulkanAllocator::Reallocation;
		m_callback.pfnFree = &VulkanAllocator::Free;
		m_callback.pfnInternalAllocation = nullptr;
		m_callback.pfnInternalFree = nullptr;
	}

	~VulkanAllocator() {}

	UD_INLINE const VkAllocationCallbacks& GetCallbacks() const { return m_callback; }

private:
	static void* VKAPI_CALL Allocation(void* _pUserData, size _size, size _alignment, VkSystemAllocationScope _allocationScope);
	static void* VKAPI_CALL Reallocation(void* _pUserData, void* _pOriginal, size _size, size _alignment, VkSystemAllocationScope _allocationScope);
	static void VKAPI_CALL Free(void* _pUserData, void* _pMemory);

	void* Allocation(size _size, size _alignment, VkSystemAllocationScope _allocationScope);
	void* Reallocation(void* _pOriginal, size _size, size _alignment, VkSystemAllocationScope _allocationScope);
	void Free(void* _pMemory);

private:
	VkAllocationCallbacks	m_callback;
};

UD_NAMESPACE_END