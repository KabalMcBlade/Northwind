#include "Texture.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemoryWrapper.h"
#include "../Core/SettingsDefines.h"
#include "../Core/Assertions.h"

#include "../Dependencies/stb_image.h"

#define TINYKTX_IMPLEMENTATION
#include "../Dependencies/tiny_ktx.h"

#define TINYDDS_IMPLEMENTATION
#include "../Dependencies/tiny_dds.h"

#include "../Client/FileSystem.h"


NW_NAMESPACE_BEGIN


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

void* Texture::KTX_DDS_Allocation(void* _pUserData, size _size)
{
	return static_cast<Texture*>(_pUserData)->KTX_DDS_Allocation(_size);
}

void Texture::KTX_DDS_Free(void* _pUserData, void* _pMemory)
{
	static_cast<Texture*>(_pUserData)->KTX_DDS_Free(_pMemory);
}

size Texture::KTX_DDS_Read(void *_pUserData, void* _pMemory, size _size)
{
	return static_cast<Texture*>(_pUserData)->KTX_DDS_Read(_pMemory, _size);
}

bool Texture::KTX_DDS_Seek(void *_pUserData, int64 _offset)
{
	return static_cast<Texture*>(_pUserData)->KTX_DDS_Seek(_offset);
}

int64 Texture::KTX_DDS_Tell(void *_pUserData)
{
	return static_cast<Texture*>(_pUserData)->KTX_DDS_Tell();
}

void Texture::KTX_DDS_Error(void* _pUserData, char const *_msg)
{
	static_cast<Texture*>(_pUserData)->KTX_DDS_Error(_msg);
}

void* Texture::KTX_DDS_Allocation(size _size)
{
	return eosNewAlignedRaw(_size, GetAllocator(), alignof(uint8));
}

void Texture::KTX_DDS_Free(void* _pMemory)
{
	eosDeleteRaw(_pMemory, GetAllocator());
}

size Texture::KTX_DDS_Read(void* _pMemory, size _size)
{
	char* binary = reinterpret_cast<char*>(_pMemory);
	return m_fileStream.read(binary, _size).gcount();
}

bool Texture::KTX_DDS_Seek(int64 _offset)
{
	return !!m_fileStream.seekg(_offset, std::ios_base::beg);
}

int64 Texture::KTX_DDS_Tell()
{
	return static_cast<int64>(m_fileStream.tellg());
}

void Texture::KTX_DDS_Error(char const *_msg)
{
	nwAssertVoid(false, "[TEXTURE LOAD ERROR]: %s", _msg);
}


//////////////////////////////////////////////////////////////////////////


Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::Load2D(const VkDevice& m_device, const nwString& _path,
	VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
	VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 /*_maxAnisotrpy = 1*/)
{
	nwString ext = FileSystem::GetFileExt(_path);

	uint32 width;
	uint32 height;
	uint32 depth;
	uint32 slices;
	uint32 mipmaps;

	m_fileStream.open(_path.c_str(), std::ios::binary);

	if (ext == "ktx")
	{
		TinyKtx_Callbacks callbacks {
		   &Texture::KTX_DDS_Error,
		   &Texture::KTX_DDS_Allocation,
		   &Texture::KTX_DDS_Free,
		   &Texture::KTX_DDS_Read,
		   &Texture::KTX_DDS_Seek,
		   &Texture::KTX_DDS_Tell
		};

		TinyKtx_ContextHandle ctx = TinyKtx_CreateContext(&callbacks, this);
		if (!TinyKtx_ReadHeader(ctx))
		{
			TinyKtx_DestroyContext(ctx);
			m_fileStream.close();
			nwAssertReturnValue(false, false, "KTX Texture Header cannot be read!");
		}

		width = TinyKtx_Width(ctx);
		height = TinyKtx_Height(ctx);
		depth = TinyKtx_Depth(ctx);
		slices = TinyKtx_ArraySlices(ctx);

		TinyKtx_Format format = TinyKtx_GetFormat(ctx);
		if (format == TKTX_UNDEFINED)
		{
			TinyKtx_DestroyContext(ctx);
			m_fileStream.close();
			nwAssertReturnValue(false, false, "KTX Texture Format undefined!");
		}

		// worth checking or just forcing?
		// TinyKtx_Is1D
		// TinyKtx_Is2D
		// TinyKtx_Is3D
		// TinyKtx_IsCubemap
		// TinyKtx_IsArray

		mipmaps = TinyKtx_NumberOfMipmaps(ctx);

		// data return by ImageRawData is owned by the context. Don't free it!
		for (uint32 i = 0; i < mipmaps; ++i) 
		{
			uint32 size = TinyKtx_ImageSize(ctx, i);
			const uint8 * bufferr = reinterpret_cast<const uint8*>(TinyKtx_ImageRawData(ctx, i));
		}

		// TO-DO HERE

		TinyKtx_DestroyContext(ctx);
	}
	else if (ext == "dds")
	{
		TinyDDS_Callbacks callbacks{
		   &Texture::KTX_DDS_Error,
		   &Texture::KTX_DDS_Allocation,
		   &Texture::KTX_DDS_Free,
		   &Texture::KTX_DDS_Read,
		   &Texture::KTX_DDS_Seek,
		   &Texture::KTX_DDS_Tell
		};

		TinyDDS_ContextHandle ctx = TinyDDS_CreateContext(&callbacks, this);
		if (!TinyDDS_ReadHeader(ctx))
		{
			TinyDDS_DestroyContext(ctx);
			m_fileStream.close();
			nwAssertReturnValue(false, false, "DDS Texture Header cannot be read!");
		}

		width = TinyDDS_Width(ctx);
		height = TinyDDS_Height(ctx);
		depth = TinyDDS_Depth(ctx);
		slices = TinyDDS_ArraySlices(ctx);

		TinyDDS_Format format = TinyDDS_GetFormat(ctx);
		if (format == TDDS_UNDEFINED)
		{
			TinyDDS_DestroyContext(ctx);
			m_fileStream.close();
			nwAssertReturnValue(false, false, "DDS Texture Format undefined!");
		}

		mipmaps = TinyDDS_NumberOfMipmaps(ctx);


		// data return by ImageRawData is owned by the context. Don't free it!
		for (uint32 i = 0; i < mipmaps; ++i)
		{
			uint32 size = TinyDDS_ImageSize(ctx, i);
			const uint8 * bufferr = reinterpret_cast<const uint8*>(TinyDDS_ImageRawData(ctx, i));
		}

		// TO-DO HERE

		TinyDDS_DestroyContext(ctx);
	}
	else 
	{
		 
	}

	m_fileStream.close();

	return true;
}

bool Texture::LoadArray(const VkDevice& m_device, const nwString& _path,
	VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
	VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 /*_maxAnisotrpy = 1*/)
{
	return false;
}

bool Texture::LoadCube(const VkDevice& m_device, const nwString& _path,
	VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
	VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 /*_maxAnisotrpy = 1*/)
{
	return false;
}


bool Texture::Load2D(const VkDevice& m_device, const uint8* _buffer,
	VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
	VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 /*_maxAnisotrpy = 1*/)
{
	return false;
}

bool Texture::LoadArray(const VkDevice& m_device, const uint8* _buffer,
	VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
	VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 /*_maxAnisotrpy = 1*/)
{
	return false;
}

bool Texture::LoadCube(const VkDevice& m_device, const uint8* _buffer,
	VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
	VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 /*_maxAnisotrpy = 1*/)
{
	return false;
}

void Texture::Destroy()
{

}

void Texture::UpdateDescriptor()
{

}

NW_NAMESPACE_END