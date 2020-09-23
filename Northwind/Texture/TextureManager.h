#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/MemoryWrapper.h"
#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "Texture.h"


NW_NAMESPACE_BEGIN


class NW_DLL TextureManager final : public eos::NoCopyableMoveable
{
public:
	static TextureManager& Instance();

	// note: the VkFormat _format can be avoided if loaded via KTX, because the type is internal to the file, u can pass it anyway in case the format found in the ktx file is VK_FORMAT_UNDEFINED
	Texture* Load2D(const Device& _device, const nwString& _path,
		VkFormat _format, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	Texture* LoadArray(const Device& _device, const nwString& _path,
		VkFormat _format, uint32 _sliceCount = 0, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	Texture* LoadCube(const Device& _device, const nwString& _path,
		VkFormat _format, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);


	Texture* Load2D(const Device& _device, const nwString& _name, const void* _buffer, size _size, bool _needGenerateMipmaps,
		VkFormat _format, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		uint64* _mipmpapsOffsets = nullptr,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	Texture* LoadArray(const Device& _device, const nwString& _name, const void* _buffer, size _size, bool _needGenerateMipmaps,
		VkFormat _format, uint32 _sliceCount = 1, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		uint64* _mipmpapsOffsets = nullptr,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	Texture* LoadCube(const Device& _device, const nwString& _name, const void* _buffer, size _size, bool _needGenerateMipmaps,
		VkFormat _format, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		uint64* _mipmpapsOffsets = nullptr,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);


	Texture* Generate2D(const Device& _device, const nwString& _name, uint32 _width, uint32 _height, bool _generateMipmaps,
		VkFormat _format, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);

	Texture* GenerateCube(const Device& _device, const nwString& _name, uint32 _width, uint32 _height, bool _generateMipmaps,
		VkFormat _format, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);


	void Destroy(const nwString& _pathOrName);
	void DestroyAll();

	Texture* GetTexture(const nwString& _pathOrName) const;

private:
	TextureManager();
	~TextureManager();

private:
	using TextureManagerAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static TextureManagerAllocator* GetAllocator();

	eos::Map<uint32, Texture*, TextureManagerAllocator, GetAllocator> m_textures;
	
};

NW_NAMESPACE_END