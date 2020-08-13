#include "Sampler.h"

#include "../Core/Assertions.h"
#include "../GPU/GpuMemoryManager.h"


NW_NAMESPACE_BEGIN

Sampler::Sampler() 
	: m_device(VK_NULL_HANDLE)
	, m_sampler(VK_NULL_HANDLE)
	, m_minFilter(VK_FILTER_NEAREST)
	, m_magFilter(VK_FILTER_LINEAR)
	, m_mipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
	, m_addressModeU(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
	, m_addressModeV(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
	, m_addressModeW(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
	, m_borderColor(VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE)
	, m_maxAnisotropy(0.0f)
	, m_minLod(0.0f)
	, m_maxLod(0.0f)
	, m_mipmapLodBias(0.0f)
	, m_isDepthSampler(false)
{
}

Sampler::~Sampler()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool Sampler::Create(const VkDevice& _device)
{
	m_device = _device;

	VkSamplerCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.addressModeU = m_addressModeU;
	createInfo.addressModeV = m_addressModeV;
	createInfo.addressModeW = m_addressModeW;
	createInfo.borderColor = m_borderColor;
	createInfo.minFilter = m_minFilter;
	createInfo.magFilter = m_magFilter;
	createInfo.maxAnisotropy = m_maxAnisotropy;
	createInfo.anisotropyEnable = (m_maxAnisotropy > 0.0f);
	createInfo.compareEnable = m_isDepthSampler;	
	createInfo.compareOp = m_isDepthSampler ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_NEVER;
	createInfo.unnormalizedCoordinates = VK_FALSE;	// I'll take care later if I need unnormalized
	createInfo.flags = 0;	// no additional bit flags
	createInfo.minLod = m_minLod;
	createInfo.maxLod = m_maxLod;
	createInfo.mipmapMode = m_mipmapMode;
	createInfo.mipLodBias = m_mipmapLodBias;

	VkResult result = vkCreateSampler(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_sampler);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create Sampler");

	return m_sampler != VK_NULL_HANDLE;
}

void Sampler::Destroy()
{
	if (m_sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(m_device, m_sampler, GpuMemoryManager::Instance().GetVK());
		m_sampler = VK_NULL_HANDLE;
	}
}

void Sampler::SetMinFilter(VkFilter _minFilter)
{
	m_minFilter = _minFilter;
}

void Sampler::SetMagFilter(VkFilter _magFilter)
{
	m_magFilter = _magFilter;
}

void Sampler::SetAddressMode(VkSamplerAddressMode _adressMode)
{
	m_addressModeU = _adressMode;
	m_addressModeV = _adressMode;
	m_addressModeW = _adressMode;
}

void Sampler::SetAddressModeU(VkSamplerAddressMode _adressMode)
{
	m_addressModeU = _adressMode;
}

void Sampler::SetAddressModeV(VkSamplerAddressMode _adressMode)
{
	m_addressModeV = _adressMode;
}

void Sampler::SetAddressModeW(VkSamplerAddressMode _adressMode)
{
	m_addressModeW = _adressMode;
}

void Sampler::SetBorderColor(VkBorderColor _borderColor)
{
	m_borderColor = _borderColor;
}

void Sampler::SetMaxAnisotropy(float _maxAnisotropy)
{
	m_maxAnisotropy = _maxAnisotropy;
}

void Sampler::SetDepthSampler(bool _isDepthSampler)
{
	m_isDepthSampler = _isDepthSampler;
}

void Sampler::SetLod(float _min, float _max)
{
	m_minLod = _min;
	m_maxLod = _max;
}

void Sampler::SetMipMap(VkSamplerMipmapMode _mode, float _lodBias)
{
	m_mipmapMode = _mode;
	m_mipmapLodBias = _lodBias;
}

NW_NAMESPACE_END