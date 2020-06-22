#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "QueueFamily.h"


NW_NAMESPACE_BEGIN

class Device final
{
public:
	Device();
	~Device();

	bool Create(const VkInstance& _instance, const VkSurfaceKHR& _surface, const VkPhysicalDeviceFeatures& _enabledFeatures = {});
	void Destroy();

	const VkPhysicalDevice& GetPhysicalDevice() const
	{
		return m_physicalDevice;
	}

	const VkDevice& GetDevice() const
	{
		return m_device;
	}

	const QueueFamily& GetQueueFamily() const
	{
		return m_queueFamily;
	}

	const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const
	{
		return m_physicalDeviceProperties;
	}

	VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
	VkQueue GetComputeQueue() const { return m_computeQueue; }
	VkQueue GetPresentQueue() const { return m_presentQueue; }

private:
	using RenderDeviceAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static RenderDeviceAllocator* GetAllocator();

	bool CheckDeviceExtensionSupport(VkPhysicalDevice _device, const eos::Vector<const char *, RenderDeviceAllocator, GetAllocator>& _deviceExtensions);
	bool IsDeviceSuitable(VkPhysicalDevice _physicalDevice);

private:
	VkInstance m_instance;
	VkSurfaceKHR m_surface;
	VkPhysicalDeviceFeatures m_enabledDeviceFeatures;

	VkPhysicalDeviceProperties m_physicalDeviceProperties;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;

	QueueFamily m_queueFamily;

	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	VkQueue m_computeQueue;

	bool m_enableValidationLayers;
};

NW_NAMESPACE_END