#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class Sampler final
{
public:
	Sampler();
	~Sampler();
	
	bool Create(const VkDevice& _device);
	void Destroy();

	void SetMinFilter(VkFilter _minFilter);
	void SetMagFilter(VkFilter _magFilter);
	void SetAddressMode(VkSamplerAddressMode _adressMode);
	void SetAddressModeU(VkSamplerAddressMode _adressMode);
	void SetAddressModeV(VkSamplerAddressMode _adressMode);
	void SetAddressModeW(VkSamplerAddressMode _adressMode);
	void SetBorderColor(VkBorderColor _borderColor);
	void SetMaxAnisotropy(float _maxAnisotropy);
	
	NW_INLINE operator const VkSampler& () const
	{
		return m_sampler;
	}


private:
	VkDevice m_device;
	VkSampler m_sampler;
	VkFilter m_magFilter;
	VkFilter m_minFilter;
	VkSamplerMipmapMode m_mipmapMode;
	VkSamplerAddressMode m_addressModeU;
	VkSamplerAddressMode m_addressModeV;
	VkSamplerAddressMode m_addressModeW;
	VkBorderColor m_borderColor;
	float m_maxAnisotropy;
};

NW_NAMESPACE_END