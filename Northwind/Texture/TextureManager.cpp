#include "TextureManager.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"
#include "../GPU/GpuMemoryManager.h"

#include "../Utilities/HashTools.h"


NW_NAMESPACE_BEGIN


TextureManager::TextureManagerAllocator* TextureManager::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kTextureManagerAllocatorSize);
	static TextureManagerAllocator memoryAllocator(memoryArea, "TextureManagerAllocator");

	return &memoryAllocator;
}


TextureManager& TextureManager::Instance()
{
	static TextureManager instance;
	return instance;
}


Texture* TextureManager::Load2D(const Device& _device, const nwString& _path,
	VkFormat _format, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	if (_path.empty())
	{
		return nullptr;
	}

	Texture* texture = GetTexture(_path);
	if (texture == nullptr)
	{
		texture = eosNew(Texture, GetAllocator());
		texture->Load(_device, _path, 0, _format, 0, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, _imageUsageFlags, _imageLayout, _maxAnisotrpy);

		uint32 hash = HashTools::MakeHash32(_path.c_str(), static_cast<uint32>(_path.length()), Texture::kTextureNameHashSeed);
		m_textures[hash] = texture;
	}

	return texture;
}

Texture* TextureManager::LoadArray(const Device& _device, const nwString& _path,
	VkFormat _format, uint32 _sliceCount /*= 0*/, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	if (_path.empty())
	{
		return nullptr;
	}

	Texture* texture = GetTexture(_path);
	if (texture == nullptr)
	{
		texture = eosNewAligned(Texture, GetAllocator(), 16);
		texture->Load(_device, _path, 1, _format, _sliceCount, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, _imageUsageFlags, _imageLayout, _maxAnisotrpy);

		uint32 hash = HashTools::MakeHash32(_path.c_str(), static_cast<uint32>(_path.length()), Texture::kTextureNameHashSeed);
		m_textures[hash] = texture;
	}

	return texture;
}

Texture* TextureManager::LoadCube(const Device& _device, const nwString& _path,
	VkFormat _format, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	if (_path.empty())
	{
		return nullptr;
	}

	Texture* texture = GetTexture(_path);
	if (texture == nullptr)
	{
		texture = eosNewAligned(Texture, GetAllocator(), 16);
		texture->Load(_device, _path, 2, _format, 0, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, _imageUsageFlags, _imageLayout, _maxAnisotrpy);

		uint32 hash = HashTools::MakeHash32(_path.c_str(), static_cast<uint32>(_path.length()), Texture::kTextureNameHashSeed);
		m_textures[hash] = texture;
	}

	return texture;
}


//////////////////////////////////////////////////////////////////////////

Texture* TextureManager::Load2D(const Device& _device, const void* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
	VkFormat _format, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/,
	uint64* _mipmpapsOffsets /*= nullptr*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	if (_buffer == nullptr || _name.empty())
	{
		return nullptr;
	}

	Texture* texture = GetTexture(_name);
	if (texture == nullptr)
	{
		texture = eosNewAligned(Texture, GetAllocator(), 16);
		texture->Load2D(_device, _buffer, _size, _needGenerateMipmaps, _name, _format, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, _mipmpapsOffsets, _imageUsageFlags, _imageLayout, _maxAnisotrpy);

		uint32 hash = HashTools::MakeHash32(_name.c_str(), static_cast<uint32>(_name.length()), Texture::kTextureNameHashSeed);
		m_textures[hash] = texture;
	}

	return texture;
}

Texture* TextureManager::LoadArray(const Device& _device, const void* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
	VkFormat _format, uint32 _sliceCount /*= 1*/, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/,
	uint64* _mipmpapsOffsets /*= nullptr*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	if (_buffer == nullptr || _name.empty())
	{
		return nullptr;
	}

	Texture* texture = GetTexture(_name);
	if (texture == nullptr)
	{
		texture = eosNewAligned(Texture, GetAllocator(), 16);
		texture->LoadArray(_device, _buffer, _size, _needGenerateMipmaps, _name, _format, _sliceCount, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, _mipmpapsOffsets, _imageUsageFlags, _imageLayout, _maxAnisotrpy);

		uint32 hash = HashTools::MakeHash32(_name.c_str(), static_cast<uint32>(_name.length()), Texture::kTextureNameHashSeed);
		m_textures[hash] = texture;
	}

	return texture;
}

Texture* TextureManager::LoadCube(const Device& _device, const void* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
	VkFormat _format, VkFilter _magFilter /*= VK_FILTER_LINEAR*/, VkFilter _minFilter /*= VK_FILTER_LINEAR*/,
	VkSamplerAddressMode _addressModeU /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeV /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/, VkSamplerAddressMode _addressModeW /*= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE*/,
	uint64* _mipmpapsOffsets /*= nullptr*/,
	VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 _maxAnisotrpy /*= 1*/)
{
	if (_buffer == nullptr || _name.empty())
	{
		return nullptr;
	}

	Texture* texture = GetTexture(_name);
	if (texture == nullptr)
	{
		texture = eosNewAligned(Texture, GetAllocator(), 16);
		texture->LoadCube(_device, _buffer, _size, _needGenerateMipmaps, _name, _format, _magFilter, _minFilter, _addressModeU, _addressModeV, _addressModeW, _mipmpapsOffsets, _imageUsageFlags, _imageLayout, _maxAnisotrpy);

		uint32 hash = HashTools::MakeHash32(_name.c_str(), static_cast<uint32>(_name.length()), Texture::kTextureNameHashSeed);
		m_textures[hash] = texture;
	}

	return texture;
}

void TextureManager::Destroy(const nwString& _pathOrName)
{
	uint32 hash = HashTools::MakeHash32(_pathOrName.c_str(), static_cast<uint32>(_pathOrName.length()), Texture::kTextureNameHashSeed);

	auto search = m_textures.find(hash);
	if (search != m_textures.end())
	{
		if (search->second != nullptr)
		{
			// I do not need to call Destroy of the texture here, because destroying the pointer call the destructor of the inner instances
			eosDelete(search->second, GetAllocator());
		}

		m_textures.erase(hash);
	}
}

void TextureManager::DestroyAll()
{
	auto begin = m_textures.begin(), end = m_textures.end();
	eos::Map<uint32, Texture*, TextureManagerAllocator, GetAllocator>::iterator it = begin;
	for (; it != end; ++it)
	{
		if (it->second != nullptr)
		{
			// I do not need to call Destroy of the texture here, because destroying the pointer call the destructor of the inner instances
			eosDelete(it->second, GetAllocator());
		}
	}

	m_textures.clear();
}

Texture* TextureManager::GetTexture(const nwString& _pathOrName) const
{
	if (_pathOrName.empty())
	{
		return nullptr;
	}

	uint32 hash = HashTools::MakeHash32(_pathOrName.c_str(), static_cast<uint32>(_pathOrName.length()), Texture::kTextureNameHashSeed);

	auto search = m_textures.find(hash);
	if (search != m_textures.end())
	{
		return search->second;
	}
	else
	{
		return nullptr;
	}
}

TextureManager::TextureManager()
{
}


TextureManager::~TextureManager()
{
}

NW_NAMESPACE_END