#include "FrameBuffer.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"
#include "../GPU/GpuMemoryManager.h"


NW_NAMESPACE_BEGIN

FrameBuffer::FrameBufferAllocator* FrameBuffer::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kFrameBufferAllocatorSize);
	static FrameBufferAllocator memoryAllocator(memoryArea, "FrameBufferAllocator");

	return &memoryAllocator;
}

//////////////////////////////////////////////////////////////////////////
FrameBuffer::FrameBuffer()
{
}

FrameBuffer::~FrameBuffer()
{
}

bool FrameBuffer::Create()
{
	return false;
}

void FrameBuffer::Destroy()
{
}


NW_NAMESPACE_END