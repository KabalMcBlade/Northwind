#include "Texture.h"

#include "../Core/Assertions.h"

#include "../Dependencies/stb_image.h"
#include "../Dependencies/tiny_ktx.h"
#include "../Dependencies/tiny_dds.h"


NW_NAMESPACE_BEGIN

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::Load2D(const VkDevice& m_device, const nwString& _filename,
	VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
	VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 /*_maxAnisotrpy = 1*/)
{
	return false;
}

bool Texture::LoadArray(const VkDevice& m_device, const nwString& _filename,
	VkFilter _magFilter, VkFilter _minFilter, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW,
	VkQueue _copyQueue, VkImageUsageFlags _imageUsageFlags /*= VK_IMAGE_USAGE_SAMPLED_BIT*/, VkImageLayout _imageLayout /*= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/, uint32 /*_maxAnisotrpy = 1*/)
{
	return false;
}

bool Texture::LoadCube(const VkDevice& m_device, const nwString& _filename,
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