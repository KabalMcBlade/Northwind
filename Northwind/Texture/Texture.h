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

	// note: the VkFormat _format can be avoided if loaded via KTX, because the type is internal to the file, so just pass VK_FORMAT_UNDEFINED
	bool Load2D(const Device& m_device, const nwString& _path,
		VkFormat _format, VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadArray(const Device& m_device, const nwString& _path,
		VkFormat _format, VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadCube(const Device& m_device, const nwString& _path,
		VkFormat _format, VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);


	bool Load2D(const Device& _device, const uint8* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
		VkFormat _format, VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		uint64* _mipmpapsOffsets = nullptr,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadArray(const Device& m_device, const uint8* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
		VkFormat _format, VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		uint64* _mipmpapsOffsets = nullptr,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadCube(const Device& m_device, const uint8* _buffer, size _size, bool _needGenerateMipmaps, const nwString& _name,
		VkFormat _format, VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		uint64* _mipmpapsOffsets = nullptr,
		VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);

	void Destroy();

	void UpdateDescriptor();

	NW_INLINE const uint32 GetId() const { return m_hash; }
	NW_INLINE const VkDescriptorImageInfo GetDescriptor() const { return m_descriptor; }

public:
	static void SetImageLayout(VkCommandBuffer _cmdbuffer, VkImage _image, VkImageLayout _oldImageLayout, VkImageLayout _newImageLayout, VkImageSubresourceRange _subresourceRange, VkPipelineStageFlags _srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags _dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
	static void SetImageLayout(VkCommandBuffer _cmdbuffer, VkImage _image, VkImageAspectFlags _aspectMask, VkImageLayout _oldImageLayout, VkImageLayout _newImageLayout, VkPipelineStageFlags _srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags _dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

private:
	void GenerateMipmaps2D(const Device& _device, VkImageSubresourceRange _subresourceRange);
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
	std::ifstream m_fileStream;
	uint32 m_hash;
	uint32 m_mipmaps;
	uint32 m_width;
	uint32 m_height;
	//uint32 m_depth;
	//uint32 m_slices;
};

NW_NAMESPACE_END