#include "FrameBufferAttachment.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"
#include "../GPU/GpuMemoryManager.h"

NW_NAMESPACE_BEGIN

FrameBufferAttachment::FrameBufferAttachment() 
	: m_device(VK_NULL_HANDLE)
	, m_image(VK_NULL_HANDLE)
	, m_view(VK_NULL_HANDLE)
	, m_allocation(GpuMemoryAllocation())
	, m_format()
{
}

FrameBufferAttachment::~FrameBufferAttachment()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool FrameBufferAttachment::Create(const VkDevice& _device, uint32 _width, uint32 _height, uint32_t _layerCount, VkFormat _format, VkImageUsageFlags _usage)
{
	VkImageAspectFlags aspectMask = 0;
	//VkImageLayout imageLayout;

	m_device = _device;
	m_format = _format;

	if (_usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
	{
		aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		//imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	if (_usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
	{
		aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		//imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	nwAssertReturnValue(aspectMask > 0, false, "AspectMask not supported!");

	VkImageCreateInfo image = {};
	image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image.pNext = nullptr;
	image.imageType = VK_IMAGE_TYPE_2D;
	image.format = m_format;
	image.extent.width = _width;
	image.extent.height = _height;
	image.extent.depth = 1;
	image.mipLevels = 1;
	image.arrayLayers = 1;
	image.samples = VK_SAMPLE_COUNT_1_BIT;
	image.tiling = VK_IMAGE_TILING_OPTIMAL;
	image.usage = _usage | VK_IMAGE_USAGE_SAMPLED_BIT;

	VkResult result = vkCreateImage(m_device, &image, GpuMemoryManager::Instance().GetVK(), &m_image);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create Image for FrameBufferAttachment");

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(m_device, m_image, &memoryRequirements);

	{
		GpuMemoryCreateInfo createInfo = {};
		createInfo.m_size = memoryRequirements.size;
		createInfo.m_align = memoryRequirements.alignment;
		createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
		createInfo.m_usage = EMemoryUsage_GPU;
		createInfo.m_type = EGpuMemoryType_ImageOptimal;		// linear?

		m_allocation = GpuMemoryManager::Instance().GetGPU()->Alloc(createInfo);
		nwAssertReturnValue(m_allocation.m_result == VK_SUCCESS, false, "Cannot Allocate memory!");

		result = vkBindImageMemory(m_device, m_image, m_allocation.m_memory, m_allocation.m_offset);
		nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot bind an Image for FrameBufferAttachment");
	}

	VkImageViewCreateInfo imageView = {};
	imageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageView.pNext = nullptr;
	imageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageView.format = m_format;
	imageView.subresourceRange = {};
	imageView.subresourceRange.aspectMask = aspectMask;
	imageView.subresourceRange.baseMipLevel = 0;
	imageView.subresourceRange.levelCount = 1;
	imageView.subresourceRange.baseArrayLayer = 0;
	imageView.subresourceRange.layerCount = _layerCount;	// 1
	imageView.image = m_image;
	result = vkCreateImageView(m_device, &imageView, GpuMemoryManager::Instance().GetVK(), &m_view);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create ImageView for FrameBufferAttachment");

	return m_view != VK_NULL_HANDLE;
}

void FrameBufferAttachment::Destroy()
{
	if (m_image != VK_NULL_HANDLE)
	{
		vkDestroyImage(m_device, m_image, GpuMemoryManager::Instance().GetVK());
		m_image = VK_NULL_HANDLE;
	}

	if (m_view != VK_NULL_HANDLE)
	{
		vkDestroyImageView(m_device, m_view, GpuMemoryManager::Instance().GetVK());
		m_view = VK_NULL_HANDLE;
	}

	if (m_allocation.m_memory != VK_NULL_HANDLE)
	{
		GpuMemoryManager::Instance().GetGPU()->Free(m_allocation);
	}
}

NW_NAMESPACE_END