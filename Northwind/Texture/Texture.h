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

	bool Load2D(const VkDevice& m_device, const nwString& _filename, 
		VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadArray(const VkDevice& m_device, const nwString& _filename,
		VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
		VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT, VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, uint32 _maxAnisotrpy = 1);
	bool LoadCube(const VkDevice& m_device, const nwString& _filename,
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
	Image m_image;
	ImageView m_view;
	VkDescriptorImageInfo m_descriptor;
	VkSampler m_sampler;
};

NW_NAMESPACE_END