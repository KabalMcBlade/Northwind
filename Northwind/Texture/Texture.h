#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/MemoryWrapper.h"
#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "../Renderer/Image.h"
#include "../Renderer/ImageView.h"
#include "../Renderer/Sampler.h"


NW_NAMESPACE_BEGIN

class Device;
class NW_DLL Texture final
{
public:
	Texture();
	~Texture();

	void UpdateDescriptor();

	NW_INLINE operator VkDescriptorImageInfo() const { return m_descriptor; }

	NW_INLINE const uint32 GetId() const { return m_hash; }

	NW_INLINE const Image& GetImage() const { return m_image; }
	NW_INLINE const ImageView& GetView() const { return m_view; }
	NW_INLINE const Sampler& GetSampler() const { return m_sampler; }

	NW_INLINE const uint32 GetMipmapsCount() const { return m_mipmaps; }
	NW_INLINE const uint32 GetWidth() const { return m_width; }
	NW_INLINE const uint32 GetHeight() const { return m_height; }


private:
	friend class TextureManager;

	// note: the VkFormat _format can be avoided if loaded via KTX, because the type is internal to the file, u can pass ti anyway in case the format found in the ktx file is VK_FORMAT_UNDEFINED
	bool Load2D(const Device& _device, const nwString& _path,
		VkFormat _format, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadArray(const Device& _device, const nwString& _path,
		VkFormat _format, uint32 _sliceCount = 0, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadCube(const Device& _device, const nwString& _path,
		VkFormat _format, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);


	bool Load2D(const Device& _device, const void* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
		VkFormat _format, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		uint64* _mipmpapsOffsets = nullptr,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadArray(const Device& _device, const void* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
		VkFormat _format, uint32 _sliceCount = 1, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		uint64* _mipmpapsOffsets = nullptr,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadCube(const Device& _device, const void* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
		VkFormat _format, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		uint64* _mipmpapsOffsets = nullptr,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);

	void Destroy();

public:
	static void SetImageLayout(VkCommandBuffer _cmdbuffer, VkImage _image, VkImageLayout _oldImageLayout, VkImageLayout _newImageLayout, VkImageSubresourceRange _subresourceRange, VkPipelineStageFlags _srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags _dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
	static void SetImageLayout(VkCommandBuffer _cmdbuffer, VkImage _image, VkImageAspectFlags _aspectMask, VkImageLayout _oldImageLayout, VkImageLayout _newImageLayout, VkPipelineStageFlags _srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags _dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

private:
	// _type: 0 == 2D, 1 == array, 2 == cube
	bool Load(const Device& _device, const nwString& _path, uint8 _type,
		VkFormat _format, uint32 _sliceCount = 0, VkFilter _magFilter = VK_FILTER_LINEAR, VkFilter _minFilter = VK_FILTER_LINEAR,
		VkSamplerAddressMode _addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerAddressMode _addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);

	void GenerateMipmaps(const Device& _device, VkImageSubresourceRange _subresourceRange, uint32 _faceCount = 1);
	bool CanGenerateMipmaps(const Device& _device, VkFormat _format);

private:
	static void* KTX_Allocation(void* _pUserData, size _size);
	static void KTX_Free(void* _pUserData, void* _pMemory);
	static size KTX_Read(void *_pUserData, void* _pMemory, size _size);
	static bool KTX_Seek(void *_pUserData, int64 _offset);
	static int64 KTX_Tell(void *_pUserData);
	static void KTX_Error(void* _pUserData, char const *_msg);

	void* KTX_Allocation(size _size);
	void KTX_Free(void* _pMemory);
	size KTX_Read(void* _pMemory, size _size);
	bool KTX_Seek(int64 _offset);
	int64 KTX_Tell();
	void KTX_Error(char const *_msg);


private:
	Image m_image;
	ImageView m_view;
	Sampler m_sampler;
	VkDescriptorImageInfo m_descriptor;
	std::ifstream m_fileStream;	// this is not great to store here, but it will let me load different texture from different thread without issue
	uint32 m_hash;
	uint32 m_mipmaps;
	uint32 m_width;
	uint32 m_height;
};

NW_NAMESPACE_END