#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "FrameBufferAttachment.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class RenderPass;
class Sampler;
class FrameBuffer final
{
public:
	FrameBuffer();
	~FrameBuffer();

	bool Create();
	void Destroy();

	NW_INLINE operator const VkFramebuffer& () const
	{
		return m_framebuffer;
	}

private:
	using FrameBufferAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static FrameBufferAllocator* GetAllocator();

	VkFramebuffer m_framebuffer;

	eos::Vector<FramebufferAttachment, FrameBufferAllocator, GetAllocator> m_attachments;
};

NW_NAMESPACE_END