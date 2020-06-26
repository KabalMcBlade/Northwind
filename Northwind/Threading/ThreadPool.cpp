#include "ThreadPool.h"

#include "../Core/SettingsDefines.h"

NW_NAMESPACE_BEGIN

ThreadPool::ThreadAllocator* ThreadPool::GetAllocator()
{
	static eos::HeapAreaR threadPoolMemoryArea(SettingsDefines::Engine::kThreadPoolAllocatorSize);
	static ThreadAllocator threadPoolAllocator(threadPoolMemoryArea, "ThreadPoolMemory");

	return &threadPoolAllocator;
}

NW_NAMESPACE_END