#include "MemoryWrapper.h"

#include "SettingsDefines.h"


NW_NAMESPACE_BEGIN

GenericAllocator* GetGenericCommonAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Generic::kCommonAllocatorSize);
	static GenericAllocator memoryAllocator(memoryArea, "GenericStringAllocator");

	return &memoryAllocator;
}

GenericAllocator* GetGenericStringAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Generic::kStringAllocatorSize);
	static GenericAllocator memoryAllocator(memoryArea, "GenericCommonAllocator");

	return &memoryAllocator;
}

NW_NAMESPACE_END