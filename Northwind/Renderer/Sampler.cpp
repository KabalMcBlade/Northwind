#include "Sampler.h"

#include "../Core/Assertions.h"
#include "../GPU/GpuMemoryManager.h"


NW_NAMESPACE_BEGIN

Sampler::Sampler() : m_device(VK_NULL_HANDLE), m_sampler(VK_NULL_HANDLE)
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

	VkResult result = vkCreateSampler(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_sampler);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create Sampler");

	return m_sampler != VK_NULL_HANDLE;
}

void Sampler::Destroy()
{
	if (m_sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(m_device, m_sampler, GpuMemoryManager::Instance().GetVK());
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

NW_NAMESPACE_END