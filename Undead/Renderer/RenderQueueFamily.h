#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


UD_NAMESPACE_BEGIN

class UD_DLL RenderQueueFamily final
{
public:
	RenderQueueFamily();
	~RenderQueueFamily();

	bool Create(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);
	void Destroy();

	void FindQueueFamilies(VkQueueFlags _desiredFamilies = VK_QUEUE_GRAPHICS_BIT);
	void Clear();
	void SetPhysicalDevice(VkPhysicalDevice _physicslDevice);

	int32 GetGraphicsFamily() const { return m_graphicsFamily; }
	int32 GetPresentFamily() const { return m_presentFamily; }
	int32 GetComputeFamily() const { return m_computeFamily; }
	int32 GetTransferFamily() const { return m_transferFamily; }

private:
	VkPhysicalDevice m_physicalDevice;
	VkSurfaceKHR m_surface;

	int32 m_graphicsFamily = -1;
	int32 m_presentFamily = -1;
	int32 m_computeFamily = -1;
	int32 m_transferFamily = -1;
};

UD_NAMESPACE_END