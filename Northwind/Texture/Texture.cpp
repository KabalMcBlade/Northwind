#include "Texture.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../GPU/GpuMemoryManager.h"

#include "../Core/MemoryWrapper.h"
#include "../Core/SettingsDefines.h"
#include "../Core/Assertions.h"

#include "../Renderer/Device.h"
#include "../Renderer/StagingBufferManager.h"

#include "../Renderer/CommandPool.h"
#include "../Renderer/CommandBuffer.h"


#define STB_IMAGE_IMPLEMENTATION
#include "../Dependencies/stb_image.h"

#define TINYKTX_IMPLEMENTATION
#include "../Dependencies/tiny_ktx.h"

#include "../Client/FileSystem.h"

#include "../Utilities/HashTools.h"


NW_NAMESPACE_BEGIN

constexpr uint32 kTextureNameHashSeed = 11235;

using TexturesAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;

namespace
{
	TexturesAllocator* GetAllocator()
	{
		static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kTexturesAllocatorSize);
		static TexturesAllocator memoryAllocator(memoryArea, "TexturesAllocator");

		return &memoryAllocator;
	}
}

//////////////////////////////////////////////////////////////////////////

void* Texture::KTX_Allocation(void* _pUserData, size _size)
{
	return static_cast<Texture*>(_pUserData)->KTX_Allocation(_size);
}

void Texture::KTX_Free(void* _pUserData, void* _pMemory)
{
	static_cast<Texture*>(_pUserData)->KTX_Free(_pMemory);
}

size Texture::KTX_Read(void *_pUserData, void* _pMemory, size _size)
{
	return static_cast<Texture*>(_pUserData)->KTX_Read(_pMemory, _size);
}

bool Texture::KTX_Seek(void *_pUserData, int64 _offset)
{
	return static_cast<Texture*>(_pUserData)->KTX_Seek(_offset);
}

int64 Texture::KTX_Tell(void *_pUserData)
{
	return static_cast<Texture*>(_pUserData)->KTX_Tell();
}

void Texture::KTX_Error(void* _pUserData, char const *_msg)
{
	static_cast<Texture*>(_pUserData)->KTX_Error(_msg);
}

void* Texture::KTX_Allocation(size _size)
{
	return eosNewAlignedRaw(_size, GetAllocator(), alignof(uint8));
}

void Texture::KTX_Free(void* _pMemory)
{
	eosDeleteRaw(_pMemory, GetAllocator());
}

size Texture::KTX_Read(void* _pMemory, size _size)
{
	char* binary = reinterpret_cast<char*>(_pMemory);
	return m_fileStream.read(binary, _size).gcount();
}

bool Texture::KTX_Seek(int64 _offset)
{
	return !!m_fileStream.seekg(_offset, std::ios_base::beg);
}

int64 Texture::KTX_Tell()
{
	return static_cast<int64>(m_fileStream.tellg());
}

void Texture::KTX_Error(char const *_msg)
{
	nwAssertVoid(false, "[TEXTURE LOAD ERROR]: %s", _msg);
}

//////////////////////////////////////////////////////////////////////////

void Texture::SetImageLayout(VkCommandBuffer _cmdbuffer, VkImage _image, VkImageLayout _oldImageLayout, VkImageLayout _newImageLayout, VkImageSubresourceRange _subresourceRange, VkPipelineStageFlags _srcStageMask /*= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT*/, VkPipelineStageFlags _dstStageMask /*= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT*/)
{
	// Create an image barrier object
	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.oldLayout = _oldImageLayout;
	imageMemoryBarrier.newLayout = _newImageLayout;
	imageMemoryBarrier.image = _image;
	imageMemoryBarrier.subresourceRange = _subresourceRange;

	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (_oldImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		// Image layout is undefined (or does not matter)
		// Only valid as initial layout
		// No flags required, listed only for completeness
		imageMemoryBarrier.srcAccessMask = 0;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		// Image is preinitialized
		// Only valid as initial layout for linear images, preserves memory contents
		// Make sure host writes have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image is a color attachment
		// Make sure any writes to the color buffer have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image is a depth/stencil attachment
		// Make sure any writes to the depth/stencil buffer have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image is a transfer source 
		// Make sure any reads from the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image is a transfer destination
		// Make sure any writes to the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image is read by a shader
		// Make sure any shader reads from the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Target layouts (new)
	// Destination access mask controls the dependency for the new image layout
	switch (_newImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image will be used as a transfer destination
		// Make sure any writes to the image have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image will be used as a transfer source
		// Make sure any reads from the image have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image will be used as a color attachment
		// Make sure any writes to the color buffer have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image layout will be used as a depth/stencil attachment
		// Make sure any writes to depth/stencil buffer have been finished
		imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image will be read in a shader (sampler, input attachment)
		// Make sure any writes to the image have been finished
		if (imageMemoryBarrier.srcAccessMask == 0)
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Put barrier inside setup command buffer
	vkCmdPipelineBarrier(
		_cmdbuffer,
		_srcStageMask,
		_dstStageMask,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);
}

void Texture::SetImageLayout(VkCommandBuffer _cmdbuffer, VkImage _image, VkImageAspectFlags _aspectMask, VkImageLayout _oldImageLayout, VkImageLayout _newImageLayout, VkPipelineStageFlags _srcStageMask /*= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT*/, VkPipelineStageFlags _dstStageMask /*= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT*/)
{
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = _aspectMask;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 1;
	SetImageLayout(_cmdbuffer, _image, _oldImageLayout, _newImageLayout, subresourceRange, _srcStageMask, _dstStageMask);
}

//////////////////////////////////////////////////////////////////////////

Texture::Texture() : m_hash(0), m_mipmaps(0)
{
}

Texture::~Texture()
{
	// I do not need to call Destroy here, because each member is a variable, and they will be destroyed as well as soon this is destroyed.
}

bool Texture::Load2D(const Device& _device, const nwString& _path,
	VkFormat _format, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	return Load(_device, _path, 0, _format, 0, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
}

bool Texture::LoadArray(const Device& _device, const nwString& _path,
	VkFormat _format, uint32 _sliceCount /*= 0*/, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	return Load(_device, _path, 1, _format, _sliceCount, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
}

bool Texture::LoadCube(const Device& _device, const nwString& _path,
	VkFormat _format, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	return Load(_device, _path,  2, _format, 0, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
}


// _type: 0 == 2D, 1 == array, 2 == cube
bool Texture::Load(const Device& _device, const nwString& _path, uint8 _type,
	VkFormat _format, uint32 _sliceCount /*= 0*/, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	bool result = false;

	nwString ext = FileSystem::GetFileExt(_path);

	if (ext == "ktx")
	{
		m_fileStream.open(_path.c_str(), std::ios::binary);

		TinyKtx_Callbacks callbacks{
		   &Texture::KTX_Error,
		   &Texture::KTX_Allocation,
		   &Texture::KTX_Free,
		   &Texture::KTX_Read,
		   &Texture::KTX_Seek,
		   &Texture::KTX_Tell
		};

		TinyKtx_ContextHandle ctx = TinyKtx_CreateContext(&callbacks, this);
		if (!TinyKtx_ReadHeader(ctx))
		{
			TinyKtx_DestroyContext(ctx);
			m_fileStream.close();
			nwAssertReturnValue(false, result, "KTX Texture Header cannot be read!");
		}


		m_width = TinyKtx_Width(ctx);
		m_height = TinyKtx_Height(ctx);
		//m_depth = TinyKtx_Depth(ctx);
		//m_slices = TinyKtx_ArraySlices(ctx);

		// cache in case nothing found in the file (shouldn't happens!)
		VkFormat format = _format;
		_format = TinyKtx_GetVulkanFormat(ctx);
		if (_format == VK_FORMAT_UNDEFINED)
		{
			_format = format;
			nwWarning("KTX Texture Format undefined, the user passed will be used instead.");
			//TinyKtx_DestroyContext(ctx);
			//m_fileStream.close();
			//nwAssertReturnValue(false, result, "KTX Texture Format undefined!");
		}

		// worth checking? Or just delegate to the user?
		// TinyKtx_Is1D
		// TinyKtx_Is2D
		// TinyKtx_Is3D
		// TinyKtx_IsCubemap
		// TinyKtx_IsArray

		bool needMipMapsGenerated = TinyKtx_NeedsGenerationOfMipmaps(ctx);

		m_mipmaps = 0;	// 0 by default, so in case no mipmap is generated
		if (!needMipMapsGenerated)
		{
			m_mipmaps = TinyKtx_NumberOfMipmaps(ctx);
		}


		uint64* mipmapsOffsets = static_cast<uint64*>(eosNewAlignedRaw(m_mipmaps * sizeof(uint64), GetAllocator(), alignof(uint8)));

		for (uint32 i = 0; i < m_mipmaps; ++i)
		{
			uint64 offset;
			uint32 size = TinyKtx_ImageSize(ctx, i, offset);

			mipmapsOffsets[i] = offset;
		}

		// I generated manually the mipmap, so I'm going just to read the image

		// data return by ImageRawData is owned by the context. Don't free it!
		//for (uint32 i = 0; i < mipmaps; ++i) 
		//{
		//	uint32 size = TinyKtx_ImageSize(ctx, i);
		//	const uint8 * buffer = reinterpret_cast<const uint8*>(TinyKtx_ImageRawData(ctx, i));
		//}
		uint64 unused;
		uint32 size = TinyKtx_ImageSize(ctx, 0, unused);
		const uint8* buffer = reinterpret_cast<const uint8*>(TinyKtx_ImageRawData(ctx, 0));

		// align to 16 to speed up afterwards the memcpy
		const uintPtr bufferPtr = (uintPtr)buffer;
		buffer = (uint8*)eos::CoreUtils::AlignTop(bufferPtr, 16);

		switch (_type)
		{
		case 1:
			if (_sliceCount == 0)
			{
				_sliceCount = TinyKtx_ArraySlices(ctx);
			}

			result = LoadArray(_device, buffer, size, needMipMapsGenerated, _path, _format, _sliceCount, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, mipmapsOffsets, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
			break;

		case 2:
			result = LoadCube(_device, buffer, size, needMipMapsGenerated, _path, _format, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, mipmapsOffsets, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
			break;
		
		case 0:
		default:
			result = Load2D(_device, buffer, size, needMipMapsGenerated, _path, _format, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, mipmapsOffsets, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
			break;
		}

		eosDeleteRaw(mipmapsOffsets, GetAllocator());

		TinyKtx_DestroyContext(ctx);

		m_fileStream.close();
	}
	else if (ext == "hdr")
	{
		int w;
		int h;
		int c;	// don't care I forced to 4 due Vulkan limitation (note: this will return the actual component on file all he times, simple do not care about)

		float* buffer = stbi_loadf(_path.c_str(), &w, &h, &c, STBI_rgb_alpha);

		m_width = static_cast<uint32>(w);
		m_height = static_cast<uint32>(h);
		//m_depth = 0;
		//m_slices = 0;

		// Format is forced to FLOAT here if is not already set
		if (_format != VK_FORMAT_R32G32B32A32_SFLOAT)
		{
			_format = VK_FORMAT_R32G32B32A32_SFLOAT;

			nwWarning("Format for HDR image MUST be VK_FORMAT_R32G32B32A32_SFLOAT, it will converted now.");
		}

		size size = m_width * m_height * STBI_rgb_alpha * sizeof(float);

		m_mipmaps = 0;	// 0 by default, so in case no mipmap is generated

		// align to 16 to speed up afterwards the memcpy
		const uintPtr bufferPtr = (uintPtr)buffer;
		buffer = (float*)eos::CoreUtils::AlignTop(bufferPtr, 16);


		switch (_type)
		{
		case 1:
			if (_sliceCount == 0)
			{
				_sliceCount = 1;
				nwWarning("A Texture Array was requested but with no slice. Will be set 1 slice.");
			}
			result = LoadArray(_device, buffer, size, true, _path, _format, _sliceCount, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, nullptr, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
			break;

		case 2:
			result = LoadCube(_device, buffer, size, true, _path, _format, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, nullptr, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
			break;

		case 0:
		default:
			result = Load2D(_device, buffer, size, true, _path, _format, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, nullptr, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
			break;
		}

		stbi_image_free(buffer);
	}
	else
	{
		int w;
		int h;
		int c;	// don't care I forced to 4 due Vulkan limitation (note: this will return the actual component on file all he times, simple do not care about)

		uint8* buffer = reinterpret_cast<uint8*>(stbi_load(_path.c_str(), &w, &h, &c, STBI_rgb_alpha));

		m_width = static_cast<uint32>(w);
		m_height = static_cast<uint32>(h);
		//m_depth = 0;
		//m_slices = 0;

		size size = m_width * m_height * STBI_rgb_alpha * sizeof(uint8);

		m_mipmaps = 0;	// 0 by default, so in case no mipmap is generated

		// align to 16 to speed up afterwards the memcpy
		const uintPtr bufferPtr = (uintPtr)buffer;
		buffer = (uint8*)eos::CoreUtils::AlignTop(bufferPtr, 16);

		switch (_type)
		{
		case 1:
			if (_sliceCount == 0)
			{
				_sliceCount = 1;
				nwWarning("A Texture Array was requested but with no slice. Will be set 1 slice.");
			}
			result = LoadArray(_device, buffer, size, true, _path, _format, _sliceCount, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, nullptr, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
			break;

		case 2:
			result = LoadCube(_device, buffer, size, true, _path, _format, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, nullptr, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
			break;

		case 0:
		default:
			result = Load2D(_device, buffer, size, true, _path, _format, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, nullptr, _imageUsageFlags, _imageLayout, _maxAnisotrpy);
			break;
		}


		stbi_image_free(buffer);
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////

bool Texture::Load2D(const Device& _device, const void* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
	VkFormat _format, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/,
	uint64* _mipmpapsOffsets /*= nullptr*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	VkCommandBuffer copyCmd;
	VkBuffer stagingBuffer;
	size bufferOffset;
	uint8* stagedMemory = StagingBufferManager::Instance().Stage(_size, 16, copyCmd, stagingBuffer, bufferOffset);

	eos::MemUtils::MemCpy(stagedMemory, _buffer, _size);

	eos::Vector<VkBufferImageCopy, TexturesAllocator, GetAllocator> bufferCopyRegions;


	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.layerCount = 1;

	uint32 maxLodLevel = m_mipmaps;
	// mipmap present already in texture
	if (!_needGenerateMipmaps)
	{
		// Setup buffer copy regions for each mip level
		for (uint32 i = 0; i < m_mipmaps; ++i)
		{
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = i;
			bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = m_width >> i;
			bufferCopyRegion.imageExtent.height = m_height >> i;
			bufferCopyRegion.imageExtent.depth = 1;
			bufferCopyRegion.bufferOffset = _mipmpapsOffsets[i];

			bufferCopyRegions.push_back(bufferCopyRegion);
		}
		subresourceRange.levelCount = m_mipmaps;
	}
	else
	{
		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyRegion.imageSubresource.mipLevel = 0;
		bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageExtent.width = m_width;
		bufferCopyRegion.imageExtent.height = m_height;
		bufferCopyRegion.imageExtent.depth = 1;

		bufferCopyRegions.push_back(bufferCopyRegion);

		if (CanGenerateMipmaps(_device, _format))
		{
			maxLodLevel = m_mipmaps = static_cast<uint32>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;
		}
		else
		{
			m_mipmaps = 1;
			maxLodLevel = 0;
		}

		subresourceRange.levelCount = 1;	// in any case here still 1, will be changed after during the generation
	}

	if (!(_imageUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
	{
		_imageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	m_image.Create2D(_device.GetDevice(), m_width, m_height, _format, _imageUsageFlags, EMemoryUsage_CPU_to_GPU, EGpuMemoryType_ImageOptimal, m_mipmaps, 1, VK_SAMPLE_COUNT_1_BIT, _imageLayout, VK_IMAGE_TILING_OPTIMAL);


	// Image barrier for optimal image (target)
	// Optimal image will be used as destination for the copy
	SetImageLayout(copyCmd, m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

	// Copy mip levels from staging buffer
	vkCmdCopyBufferToImage(copyCmd, stagingBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32>(bufferCopyRegions.size()), bufferCopyRegions.data());

	// Change texture image layout to shader read after all mip levels have been copied
	SetImageLayout(copyCmd, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _imageLayout, subresourceRange);

	if (_needGenerateMipmaps && CanGenerateMipmaps(_device, _format))
	{
		GenerateMipmaps(_device, subresourceRange);
	}

	// Create sampler (for now a sampler is associated to the very same texture, later can be a sampler manager, and share the sampler with textures and other way round)
	m_sampler.SetAddressModeU(_addressModeU);
	m_sampler.SetAddressModeV(_addressModeV);
	m_sampler.SetAddressModeW(_addressModeW);
	m_sampler.SetMinFilter(_minFilter);
	m_sampler.SetMagFilter(_magFilter);
	m_sampler.SetMipMap(VK_SAMPLER_MIPMAP_MODE_LINEAR, 0.0f);
	m_sampler.SetMaxAnisotropy(static_cast<float>(_maxAnisotrpy));
	m_sampler.SetLod(0.0f, static_cast<float>(maxLodLevel));
	m_sampler.SetBorderColor(VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE);
	m_sampler.SetDepthSampler(false);
	m_sampler.Create(_device.GetDevice());

	m_view.Create(_device.GetDevice(), m_image, VK_IMAGE_VIEW_TYPE_2D, _format, VK_IMAGE_ASPECT_COLOR_BIT, 0, m_mipmaps, 0, 1, { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A }, 0);

	// Update descriptor image info member that can be used for setting up descriptor sets
	UpdateDescriptor();
	
	m_hash = HashTools::MakeHash32(_name.c_str(), static_cast<uint32>(_name.length()), kTextureNameHashSeed);

	return true;
}

bool Texture::LoadArray(const Device& _device, const void* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
	VkFormat _format, uint32 _sliceCount /*= 1*/, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/,
	uint64* _mipmpapsOffsets /*= nullptr*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	nwAssertReturnValue(_sliceCount > 0, false, "At least 1 slice must be set for Texture Array!");

	VkCommandBuffer copyCmd;
	VkBuffer stagingBuffer;
	size bufferOffset;
	uint8* stagedMemory = StagingBufferManager::Instance().Stage(_size, 16, copyCmd, stagingBuffer, bufferOffset);

	eos::MemUtils::MemCpy(stagedMemory, _buffer, _size);

	eos::Vector<VkBufferImageCopy, TexturesAllocator, GetAllocator> bufferCopyRegions;


	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.layerCount = _sliceCount;

	uint32 maxLodLevel = m_mipmaps;
	// mipmap present already in texture
	if (!_needGenerateMipmaps)
	{
		for (uint32 slice = 0; slice < _sliceCount; ++slice)
		{
			// Setup buffer copy regions for each mip level
			for (uint32 i = 0; i < m_mipmaps; ++i)
			{
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = i;
				bufferCopyRegion.imageSubresource.baseArrayLayer = slice;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = m_width >> i;
				bufferCopyRegion.imageExtent.height = m_height >> i;
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = _mipmpapsOffsets[i];

				bufferCopyRegions.push_back(bufferCopyRegion);
			}
			subresourceRange.levelCount = m_mipmaps;
		}
	}
	else
	{
		for (uint32 slice = 0; slice < _sliceCount; ++slice)
		{
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = 0;
			bufferCopyRegion.imageSubresource.baseArrayLayer = slice;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = m_width;
			bufferCopyRegion.imageExtent.height = m_height;
			bufferCopyRegion.imageExtent.depth = 1;

			bufferCopyRegions.push_back(bufferCopyRegion);
		}

		if (CanGenerateMipmaps(_device, _format))
		{
			maxLodLevel = m_mipmaps = static_cast<uint32>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;
		}
		else
		{
			m_mipmaps = 1;
			maxLodLevel = 0;
		}

		subresourceRange.levelCount = 1;	// in any case here still 1, will be changed after during the generation
	}

	if (!(_imageUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
	{
		_imageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	m_image.Create2D(_device.GetDevice(), m_width, m_height, _format, _imageUsageFlags, EMemoryUsage_CPU_to_GPU, EGpuMemoryType_ImageOptimal, m_mipmaps, _sliceCount, VK_SAMPLE_COUNT_1_BIT, _imageLayout, VK_IMAGE_TILING_OPTIMAL);


	// Image barrier for optimal image (target)
	// Optimal image will be used as destination for the copy
	SetImageLayout(copyCmd, m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

	// Copy mip levels from staging buffer
	vkCmdCopyBufferToImage(copyCmd, stagingBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32>(bufferCopyRegions.size()), bufferCopyRegions.data());

	// Change texture image layout to shader read after all mip levels have been copied
	SetImageLayout(copyCmd, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _imageLayout, subresourceRange);

	if (_needGenerateMipmaps && CanGenerateMipmaps(_device, _format))
	{
		GenerateMipmaps(_device, subresourceRange);
	}

	// Create sampler (for now a sampler is associated to the very same texture, later can be a sampler manager, and share the sampler with textures and other way round)
	m_sampler.SetAddressModeU(_addressModeU);
	m_sampler.SetAddressModeV(_addressModeV);
	m_sampler.SetAddressModeW(_addressModeW);
	m_sampler.SetMinFilter(_minFilter);
	m_sampler.SetMagFilter(_magFilter);
	m_sampler.SetMipMap(VK_SAMPLER_MIPMAP_MODE_LINEAR, 0.0f);
	m_sampler.SetMaxAnisotropy(static_cast<float>(_maxAnisotrpy));
	m_sampler.SetLod(0.0f, static_cast<float>(maxLodLevel));
	m_sampler.SetBorderColor(VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE);
	m_sampler.SetDepthSampler(false);
	m_sampler.Create(_device.GetDevice());

	m_view.Create(_device.GetDevice(), m_image, VK_IMAGE_VIEW_TYPE_CUBE, _format, VK_IMAGE_ASPECT_COLOR_BIT, 0, m_mipmaps, 0, _sliceCount, { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A }, 0);

	// Update descriptor image info member that can be used for setting up descriptor sets
	UpdateDescriptor();

	m_hash = HashTools::MakeHash32(_name.c_str(), static_cast<uint32>(_name.length()), kTextureNameHashSeed);

	return true;
}

bool Texture::LoadCube(const Device& _device, const void* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
	VkFormat _format, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/,
	uint64* _mipmpapsOffsets /*= nullptr*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	VkCommandBuffer copyCmd;
	VkBuffer stagingBuffer;
	size bufferOffset;
	uint8* stagedMemory = StagingBufferManager::Instance().Stage(_size, 16, copyCmd, stagingBuffer, bufferOffset);

	eos::MemUtils::MemCpy(stagedMemory, _buffer, _size);

	eos::Vector<VkBufferImageCopy, TexturesAllocator, GetAllocator> bufferCopyRegions;


	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.layerCount = 6;

	uint32 maxLodLevel = m_mipmaps;
	// mipmap present already in texture
	if (!_needGenerateMipmaps)
	{
		for (uint32 face = 0; face < 6; ++face)
		{
			// Setup buffer copy regions for each mip level
			for (uint32 i = 0; i < m_mipmaps; ++i)
			{
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = i;
				bufferCopyRegion.imageSubresource.baseArrayLayer = face;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = m_width >> i;
				bufferCopyRegion.imageExtent.height = m_height >> i;
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = _mipmpapsOffsets[i];

				bufferCopyRegions.push_back(bufferCopyRegion);
			}
			subresourceRange.levelCount = m_mipmaps;
		}
	}
	else
	{
		for (uint32 face = 0; face < 6; ++face)
		{
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = 0;
			bufferCopyRegion.imageSubresource.baseArrayLayer = face;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = m_width;
			bufferCopyRegion.imageExtent.height = m_height;
			bufferCopyRegion.imageExtent.depth = 1;

			bufferCopyRegions.push_back(bufferCopyRegion);
		}

		if (CanGenerateMipmaps(_device, _format))
		{
			maxLodLevel = m_mipmaps = static_cast<uint32>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;
		}
		else
		{
			m_mipmaps = 1;
			maxLodLevel = 0;
		}

		subresourceRange.levelCount = 1;	// in any case here still 1, will be changed after during the generation
	}

	if (!(_imageUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
	{
		_imageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	m_image.CreateCube(_device.GetDevice(), m_width, m_height, _format, _imageUsageFlags, EMemoryUsage_CPU_to_GPU, EGpuMemoryType_ImageOptimal, m_mipmaps, VK_SAMPLE_COUNT_1_BIT, _imageLayout, VK_IMAGE_TILING_OPTIMAL);


	// Image barrier for optimal image (target)
	// Optimal image will be used as destination for the copy
	SetImageLayout(copyCmd, m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

	// Copy mip levels from staging buffer
	vkCmdCopyBufferToImage(copyCmd, stagingBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32>(bufferCopyRegions.size()), bufferCopyRegions.data());

	// Change texture image layout to shader read after all mip levels have been copied
	SetImageLayout(copyCmd, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _imageLayout, subresourceRange);

	if (_needGenerateMipmaps && CanGenerateMipmaps(_device, _format))
	{
		GenerateMipmaps(_device, subresourceRange, 6);
	}

	// Create sampler (for now a sampler is associated to the very same texture, later can be a sampler manager, and share the sampler with textures and other way round)
	m_sampler.SetAddressModeU(_addressModeU);
	m_sampler.SetAddressModeV(_addressModeV);
	m_sampler.SetAddressModeW(_addressModeW);
	m_sampler.SetMinFilter(_minFilter);
	m_sampler.SetMagFilter(_magFilter);
	m_sampler.SetMipMap(VK_SAMPLER_MIPMAP_MODE_LINEAR, 0.0f);
	m_sampler.SetMaxAnisotropy(static_cast<float>(_maxAnisotrpy));
	m_sampler.SetLod(0.0f, static_cast<float>(maxLodLevel));
	m_sampler.SetBorderColor(VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE);
	m_sampler.SetDepthSampler(false);
	m_sampler.Create(_device.GetDevice());

	m_view.Create(_device.GetDevice(), m_image, VK_IMAGE_VIEW_TYPE_CUBE, _format, VK_IMAGE_ASPECT_COLOR_BIT, 0, m_mipmaps, 0, 6, { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A }, 0);

	// Update descriptor image info member that can be used for setting up descriptor sets
	UpdateDescriptor();

	m_hash = HashTools::MakeHash32(_name.c_str(), static_cast<uint32>(_name.length()), kTextureNameHashSeed);

	return true;
}

bool Texture::CanGenerateMipmaps(const Device& _device, VkFormat _format)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(_device.GetPhysicalDevice(), _format, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		nwAssertReturnValue(false, false, "Mipmaps generation requested, but the hardware does not support VkImageBlit, so it will be skipped");
	}

	return true;
}

void Texture::GenerateMipmaps(const Device& _device, VkImageSubresourceRange _subresourceRange, uint32 _faceCount /*= 1*/)
{
	CommandPool pool;
	pool.Create(_device.GetDevice(), _device.GetQueueFamily().GetGraphicsFamily(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	CommandBuffer blitCmd;
	blitCmd.Create(_device.GetDevice(), pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	blitCmd.Begin();


	for (uint32 f = 0; f < _faceCount; ++f)
	{
		// Copy down mipmaps from n-1 to n
		for (uint32 i = 1; i < m_mipmaps; ++i)
		{
			VkImageBlit imageBlit{};

			// Source
			imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlit.srcSubresource.layerCount = 1;
			imageBlit.srcSubresource.mipLevel = i - 1;
			imageBlit.srcSubresource.baseArrayLayer = f;
			imageBlit.srcOffsets[1].x = static_cast<uint32>(m_width >> (i - 1));
			imageBlit.srcOffsets[1].y = static_cast<uint32>(m_height >> (i - 1));
			imageBlit.srcOffsets[1].z = 1;

			// Destination
			imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlit.dstSubresource.layerCount = 1;
			imageBlit.dstSubresource.mipLevel = i;
			imageBlit.dstSubresource.baseArrayLayer = f;
			imageBlit.dstOffsets[1].x = static_cast<uint32>(m_width >> i);
			imageBlit.dstOffsets[1].y = static_cast<uint32>(m_height >> i);
			imageBlit.dstOffsets[1].z = 1;

			VkImageSubresourceRange mipSubRange = {};
			mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			mipSubRange.baseMipLevel = i;
			mipSubRange.levelCount = 1;
			mipSubRange.layerCount = 1;

			// Prepare current mip level as image blit destination
			VkImageMemoryBarrier imageMemoryBarrierDest{};
			imageMemoryBarrierDest.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrierDest.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrierDest.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrierDest.srcAccessMask = 0;
			imageMemoryBarrierDest.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrierDest.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemoryBarrierDest.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageMemoryBarrierDest.image = m_image;
			imageMemoryBarrierDest.subresourceRange = mipSubRange;

			vkCmdPipelineBarrier(blitCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierDest);

			// Blit from previous level
			vkCmdBlitImage(blitCmd, m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_LINEAR);

			// Prepare current mip level as image blit source for next level
			VkImageMemoryBarrier imageMemoryBarrierSrc{};
			imageMemoryBarrierSrc.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrierSrc.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrierSrc.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrierSrc.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrierSrc.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			imageMemoryBarrierSrc.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageMemoryBarrierSrc.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			imageMemoryBarrierSrc.image = m_image;
			imageMemoryBarrierSrc.subresourceRange = mipSubRange;

			vkCmdPipelineBarrier(blitCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierSrc);
		}
	}

	// After the loop, all mip layers are in TRANSFER_SRC layout, so transition all to SHADER_READ
	_subresourceRange.levelCount = m_mipmaps;

	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageMemoryBarrier.image = m_image;
	imageMemoryBarrier.subresourceRange = _subresourceRange;

	vkCmdPipelineBarrier(blitCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	blitCmd.End();
}

// Call ONLY if you want destroy the Texture manually and recreated without destroy the texture (destructor) itself
void Texture::Destroy()
{
	m_view.Destroy();
	m_image.Destroy();
	m_sampler.Destroy();
}

void Texture::UpdateDescriptor()
{
	m_descriptor.sampler = m_sampler;
	m_descriptor.imageView = m_view;
	m_descriptor.imageLayout = m_image.GetLayout();
}

NW_NAMESPACE_END