#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "../GPU/GpuDataStructure.h"


NW_NAMESPACE_BEGIN

class FrameBufferAttachment final
{
public:
	FrameBufferAttachment();
	~FrameBufferAttachment();

	bool Create(const VkDevice& _device, uint32 _width, uint32 _height, uint32_t _layerCount, VkFormat _format, VkImageUsageFlags _usage);
	void Destroy();

	NW_INLINE bool HasDepthComponents()
	{
		static const VkFormat formats[6]
		{
			VK_FORMAT_D16_UNORM,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D32_SFLOAT_S8_UINT
		};

		for (uint8 i = 0; i < 6; ++i)
		{
			if (formats[i] == m_format)
			{
				return true;
			}
		}

		return false;
	}

	NW_INLINE bool HasStencilComponents()
	{
		static const VkFormat formats[4]
		{
			VK_FORMAT_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
		};

		for (uint8 i = 0; i < 4; ++i)
		{
			if (formats[i] == m_format)
			{
				return true;
			}
		}

		return false;
	}

	NW_INLINE bool IsDepthOrStencil()
	{
		return(HasDepthComponents() || HasStencilComponents());
	}

	NW_INLINE operator const VkImageView& () const
	{
		return m_view;
	}

	NW_INLINE const VkImageView& GetImageView() const
	{
		return m_view;
	}
	NW_INLINE const VkImage& GetImage() const
	{
		return m_image;
	}

private:
	VkDevice m_device;
	VkImage m_image;
	VkImageView m_view;
	VkFormat m_format;
	GpuMemoryAllocation m_allocation;
};

NW_NAMESPACE_END