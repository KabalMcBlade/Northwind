#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>


#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN


class Image;
class ImageView final
{
public:
	ImageView();
	~ImageView();

	NW_INLINE operator const VkImageView& () const
	{
		return m_imageView;
	}

	bool Create(const VkDevice& _device, const Image& _image, VkImageViewType _viewType, VkImageAspectFlags _aspectFlags,
		uint32 _baseMipLevel = 0, uint32 _levelCount = 1, uint32 _baseArrayLayer = 0, uint32 _layerCount = 1,
		VkComponentMapping _componentMapping = {}, VkImageViewCreateFlags _flags = 0);

	bool Create(const VkDevice& _device, const Image& _image, VkImageViewType _viewType, VkFormat format, VkImageAspectFlags _aspectFlags,
		uint32 _baseMipLevel = 0, uint32 _levelCount = 1, uint32 _baseArrayLayer = 0, uint32 _layerCount = 1,
		VkComponentMapping _componentMapping = {}, VkImageViewCreateFlags _flags = 0);

	void Destroy();

private:
	VkDevice m_device;
	VkImageView m_imageView;
	VkImageViewType m_type;
	VkFormat m_format;
	VkComponentMapping m_componentMapping;
	VkImageAspectFlags m_aspectMask;
	uint32 m_baseMipLevel;
	uint32 m_levelCount;
	uint32 m_baseArrayLayer;
	uint32 m_layerCount;
};

NW_NAMESPACE_END