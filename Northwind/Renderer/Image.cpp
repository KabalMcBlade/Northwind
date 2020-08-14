#include "Image.h"

#include "../GPU/GpuMemoryManager.h"


NW_NAMESPACE_BEGIN

Image::Image() 
	: m_device(VK_NULL_HANDLE)
	, m_image(VK_NULL_HANDLE)
	, m_imageMemory(VK_NULL_HANDLE)
	, m_sampleCount(VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM)
	, m_format(VK_FORMAT_UNDEFINED)
	, m_type(VK_IMAGE_TYPE_MAX_ENUM)
	, m_tiling(VK_IMAGE_TILING_MAX_ENUM)
	, m_usage(VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM)
	, m_currentLayout(VK_IMAGE_LAYOUT_UNDEFINED)
	, m_mipLevelCount(0)
	, m_arrayLayerCount(0)
	, m_isCubeImage(false)
{
}


Image::~Image()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool Image::Create2D(const VkDevice& _device, uint32 _width, uint32 _height, VkFormat _format, VkImageUsageFlags _usage, EMemoryUsage _memoryUsage, EGpuMemoryType _memoryType,
	uint32 _mipLevels /*= 1*/, uint32 _arrayLayers /*= 1*/, VkSampleCountFlagBits _sampleCount /*= VK_SAMPLE_COUNT_1_BIT*/,
	VkImageLayout _initialLayout /*= VK_IMAGE_LAYOUT_PREINITIALIZED*/, VkImageTiling _tiling /*= VK_IMAGE_TILING_OPTIMAL*/)
{
	m_device = _device;

	VkImageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.extent.width = _width;
	createInfo.extent.height = _height;
	createInfo.extent.depth = 1;
	createInfo.mipLevels = _mipLevels;
	createInfo.arrayLayers = _arrayLayers;
	createInfo.format = _format;
	createInfo.tiling = _tiling;
	createInfo.initialLayout = _initialLayout;
	createInfo.usage = _usage;
	createInfo.samples = _sampleCount;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.flags = 0;

	VkResult result = vkCreateImage(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_image);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create Image");

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(m_device, m_image, &memoryRequirements);

	{
		GpuMemoryCreateInfo createInfo = {};
		createInfo.m_size = memoryRequirements.size;
		createInfo.m_align = memoryRequirements.alignment;
		createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
		createInfo.m_usage = _memoryUsage;
		createInfo.m_type = _memoryType;

		m_allocation = GpuMemoryManager::Instance().GetGPU()->Alloc(createInfo);
		nwAssertReturnValue(m_allocation.m_result == VK_SUCCESS, false, "Cannot Allocate memory!");

		VkResult result = vkBindImageMemory(m_device, m_image, m_allocation.m_memory, m_allocation.m_offset);
		nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot bind the image memory!");
	}

	m_isCubeImage = false;
	m_extent = { _width, _height, 1 };
	m_mipLevelCount = _mipLevels;
	m_arrayLayerCount = _arrayLayers;
	m_sampleCount = _sampleCount;
	m_format = _format;
	m_type = VK_IMAGE_TYPE_2D;
	m_tiling = _tiling;
	m_usage = _usage;
	m_currentLayout = _initialLayout;

	return true;
}


bool Image::Create3D(const VkDevice& _device, uint32 _width, uint32 _height, uint32 _depth, VkFormat _format, VkImageUsageFlags _usage, EMemoryUsage _memoryUsage, EGpuMemoryType _memoryType,
	uint32 _mipLevels /*= 1*/, uint32 _arrayLayers /*= 1*/, VkSampleCountFlagBits _sampleCount /*= VK_SAMPLE_COUNT_1_BIT*/,
	VkImageLayout _initialLayout /*= VK_IMAGE_LAYOUT_PREINITIALIZED*/, VkImageTiling _tiling /*= VK_IMAGE_TILING_OPTIMAL*/)
{
	m_device = _device;

	VkImageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_3D;
	createInfo.extent.width = _width;
	createInfo.extent.height = _height;
	createInfo.extent.depth = _depth;
	createInfo.mipLevels = _mipLevels;
	createInfo.arrayLayers = _arrayLayers;
	createInfo.format = _format;
	createInfo.tiling = _tiling;
	createInfo.initialLayout = _initialLayout;
	createInfo.usage = _usage;
	createInfo.samples = _sampleCount;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.flags = 0;

	VkResult result = vkCreateImage(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_image);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create Image");

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(m_device, m_image, &memoryRequirements);

	{
		GpuMemoryCreateInfo createInfo = {};
		createInfo.m_size = memoryRequirements.size;
		createInfo.m_align = memoryRequirements.alignment;
		createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
		createInfo.m_usage = _memoryUsage;
		createInfo.m_type = _memoryType;

		m_allocation = GpuMemoryManager::Instance().GetGPU()->Alloc(createInfo);
		nwAssertReturnValue(m_allocation.m_result == VK_SUCCESS, false, "Cannot Allocate memory!");

		VkResult result = vkBindImageMemory(m_device, m_image, m_allocation.m_memory, m_allocation.m_offset);
		nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot bind the image memory!");
	}

	m_isCubeImage = false;
	m_extent = { _width, _height, 1 };
	m_mipLevelCount = _mipLevels;
	m_arrayLayerCount = _arrayLayers;
	m_sampleCount = _sampleCount;
	m_format = _format;
	m_type = VK_IMAGE_TYPE_3D;
	m_tiling = _tiling;
	m_usage = _usage;
	m_currentLayout = _initialLayout;

	return true;
}


bool Image::CreateCube(const VkDevice& _device, uint32 _width, uint32 _height, VkFormat _format, VkImageUsageFlags _usage, EMemoryUsage _memoryUsage, EGpuMemoryType _memoryType,
	uint32 _mipLevels /*= 1*/, VkSampleCountFlagBits _sampleCount /*= VK_SAMPLE_COUNT_1_BIT*/,
	VkImageLayout _initialLayout /*= VK_IMAGE_LAYOUT_PREINITIALIZED*/, VkImageTiling _tiling /*= VK_IMAGE_TILING_OPTIMAL*/)
{
	m_device = _device;

	VkImageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.extent.width = _width;
	createInfo.extent.height = _height;
	createInfo.extent.depth = 1;
	createInfo.mipLevels = _mipLevels;
	createInfo.arrayLayers = 6;
	createInfo.format = _format;
	createInfo.tiling = _tiling;
	createInfo.initialLayout = _initialLayout;
	createInfo.usage = _usage;
	createInfo.samples = _sampleCount;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	VkResult result = vkCreateImage(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_image);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create Image");

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(m_device, m_image, &memoryRequirements);

	{
		GpuMemoryCreateInfo createInfo = {};
		createInfo.m_size = memoryRequirements.size;
		createInfo.m_align = memoryRequirements.alignment;
		createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
		createInfo.m_usage = _memoryUsage;
		createInfo.m_type = _memoryType;

		m_allocation = GpuMemoryManager::Instance().GetGPU()->Alloc(createInfo);
		nwAssertReturnValue(m_allocation.m_result == VK_SUCCESS, false, "Cannot Allocate memory!");

		VkResult result = vkBindImageMemory(m_device, m_image, m_allocation.m_memory, m_allocation.m_offset);
		nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot bind the image memory!");
	}

	m_isCubeImage = true;
	m_extent = { _width, _height, 1 };
	m_mipLevelCount = _mipLevels;
	m_arrayLayerCount = 6;
	m_sampleCount = VK_SAMPLE_COUNT_1_BIT;
	m_format = _format;
	m_type = VK_IMAGE_TYPE_2D;
	m_tiling = _tiling;
	m_usage = _usage;
	m_currentLayout = _initialLayout;

	return true;
}

void Image::Destroy()
{
	if (m_image != VK_NULL_HANDLE)
	{
		vkDestroyImage(m_device, m_image, GpuMemoryManager::Instance().GetVK());
		GpuMemoryManager::Instance().GetGPU()->Free(m_allocation);

		m_image = VK_NULL_HANDLE;
		m_allocation = GpuMemoryAllocation();
	}
}

NW_NAMESPACE_END