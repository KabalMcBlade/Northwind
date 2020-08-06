#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/Assertions.h"
#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class Image final
{
public:
	Image();
	~Image();

	bool Create2D(const VkDevice& _device, uint32 _width, uint32 _height, VkFormat _format, VkImageUsageFlags _usage, EMemoryUsage _memoryUsage, EGpuMemoryType _memoryType,
		uint32 _mipLevels = 1, uint32 _arrayLayers = 1, VkSampleCountFlagBits _sampleCount = VK_SAMPLE_COUNT_1_BIT,
		VkImageLayout _initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED, VkImageTiling _tiling = VK_IMAGE_TILING_OPTIMAL);

	bool CreateCube(const VkDevice& _device, uint32 _width, uint32 _height, VkFormat _format, VkImageUsageFlags _usage, EMemoryUsage _memoryUsage, EGpuMemoryType _memoryType,
		uint32 _mipLevels = 1, VkImageLayout _initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED, VkImageTiling _tiling = VK_IMAGE_TILING_OPTIMAL);

	void Destroy();

	NW_INLINE operator VkImage() const { return m_image; }

	NW_INLINE bool IsCubeImage() const { return m_isCubeImage; }

	NW_INLINE  VkExtent3D GetExtent(uint32 _mipLevels = 0) const
	{
		nwAssert(_mipLevels < m_mipLevelCount, "Mip level count passed is greater or equal to the current mip level count: it has to be lesser. Value passed %d, Current level %d", _mipLevels, m_mipLevelCount);
		nwAssert(m_extent.depth == 1 || _mipLevels == 0, "Depth extent is different from 1 OR the _mipLevels is not 0. Value passed %d, Extent Depth %d", _mipLevels, m_extent.depth);

		return { (m_extent.width >> _mipLevels), (m_extent.height >> _mipLevels), m_extent.depth };
	}

	NW_INLINE uint32 GetLevelCount() const { return m_mipLevelCount; }
	NW_INLINE uint32 GetLayerCount() const { return m_arrayLayerCount; }
	NW_INLINE VkSampleCountFlagBits GetSampleCount() const { return m_sampleCount; }
	NW_INLINE VkFormat GetFormat() const { return m_format; }
	NW_INLINE VkImageType GetType() const { return m_type; }
	NW_INLINE VkImageTiling GetTiling() const { return m_tiling; }
	NW_INLINE VkImageUsageFlags GetUsage() const { return m_usage; }
	NW_INLINE VkImageLayout GetLayout() const { return m_currentLayout; }

private:
	VkDevice m_device;
	VkImage m_image;
	VkDeviceMemory m_imageMemory;
	VkExtent3D m_extent;
	VkSampleCountFlagBits m_sampleCount;
	VkFormat m_format;
	VkImageType m_type;
	VkImageTiling m_tiling;
	VkImageUsageFlags m_usage;
	VkImageLayout m_currentLayout;
	GpuMemoryAllocation m_allocation;
	uint32 m_mipLevelCount;
	uint32 m_arrayLayerCount;
	bool m_isCubeImage;
};

NW_NAMESPACE_END