#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class CommandPool
{
public:
	CommandPool();
	~CommandPool();

	bool Create(const VkDevice& _device, uint32 _queueFamilyIndex, VkCommandPoolCreateFlags _flags);
	void Destroy();

	NW_INLINE operator const VkCommandPool& () const
	{
		return m_commandPool;
	}

private:
	VkDevice m_device;
	VkCommandPool m_commandPool;
};

NW_NAMESPACE_END