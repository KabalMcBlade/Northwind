#include "ImageView.h"

#include "../Core/Assertions.h"

#include "../GPU/GpuMemoryManager.h"

#include "Image.h"


NW_NAMESPACE_BEGIN


ImageView::ImageView()
	: m_device(VK_NULL_HANDLE)
	, m_imageView(VK_NULL_HANDLE)
	, m_format(VK_FORMAT_UNDEFINED)
	, m_type(VK_IMAGE_VIEW_TYPE_2D)
	, m_aspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
	, m_baseMipLevel(0)
	, m_levelCount(0)
	, m_baseArrayLayer(0)
	, m_layerCount(0)
{
}

ImageView::~ImageView()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool ImageView::Create(const VkDevice& _device, const Image& _image, VkImageViewType _viewType, VkImageAspectFlags _aspectFlags,
	uint32 _baseMipLevel /*= 0*/, uint32 _levelCount /*= 1*/, uint32 _baseArrayLayer /*= 0*/, uint32 _layerCount /*= 1*/,
	VkComponentMapping _componentMapping /*= {}*/, VkImageViewCreateFlags _flags /*= 0*/)
{
	return Create(_device, _image, _viewType, _image.GetFormat(), _aspectFlags, _baseMipLevel, _levelCount, _baseArrayLayer, _layerCount, _componentMapping, _flags);
}

bool ImageView::Create(const VkDevice& _device, const Image& _image, VkImageViewType _viewType, VkFormat format, VkImageAspectFlags _aspectFlags,
	uint32 _baseMipLevel /*= 0*/, uint32 _levelCount /*= 1*/, uint32 _baseArrayLayer /*= 0*/, uint32 _layerCount /*= 1*/,
	VkComponentMapping _componentMapping /*= {}*/, VkImageViewCreateFlags _flags /*= 0*/)
{
	m_device = _device;

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = _image;
	viewInfo.viewType = _viewType;
	viewInfo.format = format;
	viewInfo.components = _componentMapping;
	viewInfo.subresourceRange.aspectMask = _aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = _baseMipLevel;
	viewInfo.subresourceRange.levelCount = _levelCount;
	viewInfo.subresourceRange.baseArrayLayer = _baseArrayLayer;
	viewInfo.subresourceRange.layerCount = _layerCount;
	viewInfo.flags = _flags;

	VkResult result = vkCreateImageView(m_device, &viewInfo, GpuMemoryManager::Instance().GetVK(), &m_imageView);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create ImageView");

	m_type = _viewType;
	m_format = format;
	m_componentMapping = _componentMapping;
	m_aspectMask = _aspectFlags;
	m_baseMipLevel = _baseMipLevel;
	m_levelCount = _levelCount;
	m_baseArrayLayer = _baseArrayLayer;
	m_layerCount = _layerCount;

	return true;
}

void ImageView::Destroy()
{
	if (m_imageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(m_device, m_imageView, GpuMemoryManager::Instance().GetVK());
		m_imageView = VK_NULL_HANDLE;
	}
}

NW_NAMESPACE_END