#include "RenderQueueFamily.h"

#include "../Core/MemoryWrapper.h"
#include "../Core/SettingsDefines.h"
#include "../Core/Assertions.h"

#include "../GPU/GpuMemoryManager.h"


UD_NAMESPACE_BEGIN

using RenderQueueFamilyAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;

namespace
{
	RenderQueueFamilyAllocator* GetAllocator()
	{
		static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kRenderQueueFamilyAllocatorSize);
		static RenderQueueFamilyAllocator memoryAllocator(memoryArea, "RenderQueueFamilyAllocator");

		return &memoryAllocator;
	}
}

RenderQueueFamily::RenderQueueFamily()
	: m_physicalDevice(VK_NULL_HANDLE)
	, m_surface(VK_NULL_HANDLE)
{}

RenderQueueFamily::~RenderQueueFamily()
{
	Destroy();
}

bool RenderQueueFamily::Create(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
{
	m_physicalDevice = _physicalDevice;
	m_surface = _surface;

	return true;
}

void RenderQueueFamily::Destroy()
{
	Clear();
}

void RenderQueueFamily::FindQueueFamilies(VkQueueFlags _desiredFamilies /*= VK_QUEUE_GRAPHICS_BIT*/)
{
	bool wantGraphics = _desiredFamilies & VK_QUEUE_GRAPHICS_BIT;
	bool dedicatedCompute = _desiredFamilies & VK_QUEUE_COMPUTE_BIT;
	bool dedicatedTransfer = _desiredFamilies & VK_QUEUE_TRANSFER_BIT;

	VkPhysicalDevice device = m_physicalDevice;
	VkSurfaceKHR surface = m_surface;

	Clear();

	uint32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	eos::Vector<VkQueueFamilyProperties, RenderQueueFamilyAllocator, GetAllocator> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	if (wantGraphics)
	{
		for (uint32 i = 0; i < queueFamilyCount; ++i)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			if (queueFamily.queueCount > 0 && m_graphicsFamily < 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				m_graphicsFamily = i;
				break;
			}
		}

		for (uint32 i = 0; i < queueFamilyCount; ++i)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (queueFamily.queueCount > 0 && m_presentFamily < 0 && presentSupport)
			{
				m_presentFamily = i;
				break;
			}
		}
	}

	if (dedicatedCompute)
	{
		for (int32 i = 0; i < queueFamilies.size(); ++i)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			if (queueFamily.queueCount > 0 && m_computeFamily < 0 && i != m_graphicsFamily && i != m_presentFamily && (queueFamily.queueCount & VK_QUEUE_COMPUTE_BIT))
			{
				m_computeFamily = i;
				break;
			}
		}

		if (m_computeFamily < 0)
		{
			m_computeFamily = m_graphicsFamily;
		}
	}
	else
	{
		m_computeFamily = m_graphicsFamily;
	}

	if (dedicatedTransfer)
	{
		for (int32 i = 0; i < queueFamilies.size(); ++i)
		{
			const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

			if (queueFamily.queueCount > 0 && m_transferFamily < 0 && i != m_graphicsFamily && i != m_presentFamily && i != m_computeFamily && (queueFamily.queueCount & VK_QUEUE_TRANSFER_BIT))
			{
				m_transferFamily = i;
				break;
			}
		}

		if (m_transferFamily < 0)
		{
			m_transferFamily = m_graphicsFamily < 0 ? m_computeFamily : m_graphicsFamily;
		}
	}
	else
	{
		m_transferFamily = m_graphicsFamily < 0 ? m_computeFamily : m_graphicsFamily;
	}

	udAssertReturnVoid(wantGraphics && (m_graphicsFamily >= 0 && m_presentFamily >= 0), "FindQueueFamilies: no suitable graphics or present queue were found.");
	udAssertReturnVoid(wantGraphics && m_computeFamily >= 0, "FindQueueFamilies: annot find compute queue.");
	udAssertReturnVoid(wantGraphics && m_transferFamily >= 0, "FindQueueFamilies: annot find transfer queue.");
}

void RenderQueueFamily::Clear()
{
	m_graphicsFamily = -1;
	m_presentFamily = -1;
	m_computeFamily = -1;
	m_transferFamily = -1;
}

void RenderQueueFamily::SetPhysicalDevice(VkPhysicalDevice _physicslDevice)
{
	m_physicalDevice = _physicslDevice;
}

UD_NAMESPACE_END