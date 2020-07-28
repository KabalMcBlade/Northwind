#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "FrameBufferAttachment.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class RenderPass;
class FrameBuffer final
{
public:
	FrameBuffer();
	~FrameBuffer();

	bool Create(const VkDevice& _device, const RenderPass& _renderPass, uint32 _width, uint32 _height, uint32 _layers = 1, VkFramebufferCreateFlags _flags = 0);
	void Destroy();

	NW_INLINE operator const VkFramebuffer& () const
	{
		return m_framebuffer;
	}

	void PushAttachment(const FrameBufferAttachment& _attachment);

	NW_INLINE uint32 GetAttachmentCount() const { return static_cast<uint32>(m_attachmentViews.size()); }
	NW_INLINE uint32 GetWidth() const { return m_width; }
	NW_INLINE uint32 GetHeight() const { return m_height; }
	NW_INLINE uint32 GetLayerCount() const { return m_layers; }

private:
	using FrameBufferAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static FrameBufferAllocator* GetAllocator();

	VkFramebuffer m_framebuffer;
	VkDevice m_device;
	uint32 m_width;
	uint32 m_height;
	uint32 m_layers;

	eos::Vector<VkImageView, FrameBufferAllocator, GetAllocator> m_attachmentViews;
};

NW_NAMESPACE_END