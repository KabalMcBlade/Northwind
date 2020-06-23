#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class Device;
class RenderPass final
{
public:
	RenderPass();
	~RenderPass();

	bool Create(const Device& _device);
	void Destroy();

	NW_INLINE operator const VkRenderPass& () const
	{
		return m_renderPass;
	}

	NW_INLINE operator const VkRenderPass* () const
	{
		return &m_renderPass;
	}

	NW_INLINE const VkRenderPass& GetRenderPass() const
	{
		return m_renderPass;
	}

	NW_INLINE const VkFramebuffer& GetFramebuffer() const
	{
		return m_framebuffer;
	}

	VkRenderPassBeginInfo GetBeginInfo();

	void SetDimensions(int32 _width, int32 _height);
	void SetOffset(int32 _offsetX, int32 _offsetY);
	void SetFrameBuffer(VkFramebuffer _framebuffer);

	void PushColorDepthStencilClearValue(float _r, float _g, float _b, float _a, float _depth = 0.0f, uint32 _stencil = 0);
	void PushAttachmentDescription(VkAttachmentDescription _description);
	void PushSubpassDependency(VkSubpassDependency _dependency);
	void PushSubpassDescription(VkSubpassDescription _description);

private:
	using RenderPassAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static RenderPassAllocator* GetAllocator();

	VkDevice m_device;
	VkRenderPass m_renderPass;
	VkFramebuffer m_framebuffer;
	int32 m_width;
	int32 m_height;
	int32 m_offsetX;
	int32 m_offsetY;
	eos::Vector<VkAttachmentDescription, RenderPassAllocator, GetAllocator> m_attachmentDescriptors;
	eos::Vector<VkSubpassDependency, RenderPassAllocator, GetAllocator> m_subpassDependencies;
	eos::Vector<VkSubpassDescription, RenderPassAllocator, GetAllocator> m_subpassDescriptors;
	eos::Vector<VkClearValue, RenderPassAllocator, GetAllocator> m_clearValues;
};

NW_NAMESPACE_END