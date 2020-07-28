#include "FrameBuffer.h"

#include "RenderPass.h"

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
	: m_device(VK_NULL_HANDLE)
	, m_framebuffer(VK_NULL_HANDLE)
	, m_width(0)
	, m_height(0)
	, m_layers(0)
{
}

FrameBuffer::~FrameBuffer()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool FrameBuffer::Create(const VkDevice& _device, const RenderPass& _renderPass, uint32 _width, uint32 _height, uint32 _layers/* = 1*/, VkFramebufferCreateFlags _flags/* = 0*/)
{
	m_width = _width;
	m_height = _height;
	m_layers = _layers;

	VkFramebufferCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.renderPass = _renderPass;
	info.attachmentCount = static_cast<uint32>(m_attachmentViews.size());
	info.pAttachments = m_attachmentViews.data();
	info.width = m_width;
	info.height = m_height;
	info.layers = m_layers;
	info.flags = _flags;

	VkResult result = vkCreateFramebuffer(m_device, &info, GpuMemoryManager::Instance().GetVK(), &m_framebuffer);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create FrameBuffer");

	return m_framebuffer != VK_NULL_HANDLE;
}

void FrameBuffer::Destroy()
{
	if (m_framebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(m_device, m_framebuffer, GpuMemoryManager::Instance().GetVK());
		m_framebuffer = VK_NULL_HANDLE;
	}

	m_attachmentViews.clear();
}

void FrameBuffer::PushAttachment(const FrameBufferAttachment& _attachment)
{
	m_attachmentViews.push_back(_attachment);
}

NW_NAMESPACE_END