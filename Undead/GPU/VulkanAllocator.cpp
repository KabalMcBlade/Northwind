#include "VulkanAllocator.h"

#include "../Core/MemoryWrapper.h"
#include "../Core/SettingsDefines.h"
#include "../Core/Assertions.h"

UD_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////

using VulkanMemoryAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;

namespace
{
	VulkanMemoryAllocator* GetAllocator()
	{
		static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kVulkanAllocatorSize);
		static VulkanMemoryAllocator memoryAllocator(memoryArea, "VulkanMemoryAllocator");

		return &memoryAllocator;
	}
}
//////////////////////////////////////////////////////////////////////////

void* VulkanAllocator::Allocation(size _size, size _alignment, VkSystemAllocationScope _allocationScope)
{
	return eosNewAlignedRaw(_size, GetAllocator(), _alignment);
}

void* VKAPI_CALL VulkanAllocator::Allocation(void* _pUserData, size _size, size _alignment, VkSystemAllocationScope _allocationScope)
{
	return static_cast<VulkanAllocator*>(_pUserData)->Allocation(_size, _alignment, _allocationScope);
}

void* VulkanAllocator::Reallocation(void* _pOriginal, size _size, size _alignment, VkSystemAllocationScope _allocationScope)
{
	return eosReallocAlignedRaw(_pOriginal, _size, GetAllocator(), _alignment);
}

void* VKAPI_CALL VulkanAllocator::Reallocation(void* _pUserData, void* _pOriginal, size _size, size _alignment, VkSystemAllocationScope _allocationScope)
{
	return static_cast<VulkanAllocator*>(_pUserData)->Reallocation(_pOriginal, _size, _alignment, _allocationScope);
}

void VulkanAllocator::Free(void* _pMemory)
{
	if (_pMemory == nullptr) // https://www.khronos.org/registry/vulkan/specs/1.0/man/html/PFN_vkFreeFunction.html (may be null and it would be safe anyway!)
	{
		return;
	}

	eosDeleteRaw(_pMemory, GetAllocator());
}

void VKAPI_CALL VulkanAllocator::Free(void* _pUserData, void* _pMemory)
{
	return static_cast<VulkanAllocator*>(_pUserData)->Free(_pMemory);
}

UD_NAMESPACE_END