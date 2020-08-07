#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/MemoryWrapper.h"
#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "../Renderer/Image.h"
#include "../Renderer/ImageView.h"


NW_NAMESPACE_BEGIN

class Texture final
{
public:
	Texture();
	~Texture();

	bool Load2D(const VkDevice& m_device, const nwString& _path,
		VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadArray(const VkDevice& m_device, const nwString& _path,
		VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadCube(const VkDevice& m_device, const nwString& _path,
		VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);

	bool Load2D(const VkDevice& m_device, const uint8* _buffer,
		VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadArray(const VkDevice& m_device, const uint8* _buffer,
		VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadCube(const VkDevice& m_device, const uint8* _buffer,
		VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);

	void Destroy();

	void UpdateDescriptor();


private:
	static void* KTX_DDS_Allocation(void* _pUserData, size _size);
	static void KTX_DDS_Free(void* _pUserData, void* _pMemory);
	static size KTX_DDS_Read(void *_pUserData, void* _pMemory, size _size);
	static bool KTX_DDS_Seek(void *_pUserData, int64 _offset);
	static int64 KTX_DDS_Tell(void *_pUserData);
	static void KTX_DDS_Error(void* _pUserData, char const *_msg);

	void* KTX_DDS_Allocation(size _size);
	void KTX_DDS_Free(void* _pMemory);
	size KTX_DDS_Read(void* _pMemory, size _size);
	bool KTX_DDS_Seek(int64 _offset);
	int64 KTX_DDS_Tell();
	void KTX_DDS_Error(char const *_msg);


private:
	nwString m_name;
	Image m_image;
	ImageView m_view;
	VkDescriptorImageInfo m_descriptor;
	VkSampler m_sampler;
	std::ifstream m_fileStream;
	uint32 m_hash;
};

NW_NAMESPACE_END