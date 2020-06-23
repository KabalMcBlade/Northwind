#include "SwapChain.h"

#include <algorithm>

#include "Device.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"
#include "../GPU/GpuMemoryManager.h"


NW_NAMESPACE_BEGIN

SwapChain::SwapChainAllocator* SwapChain::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kSwapChainAllocatorSize);
	static SwapChainAllocator memoryAllocator(memoryArea, "SwapChainAllocator");

	return &memoryAllocator;
}

SwapChain::SwapChainDetails SwapChain::QuerySwapChainSupport(const VkPhysicalDevice& _device, VkSurfaceKHR _surface)
{
	SwapChain::SwapChainDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device, _surface, &details.m_capabilities);

	uint32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(_device, _surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.m_formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(_device, _surface, &formatCount, details.m_formats.data());
	}

	uint32 presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(_device, _surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.m_presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(_device, _surface, &presentModeCount, details.m_presentModes.data());
	}

	return details;
}

//////////////////////////////////////////////////////////////////////////

SwapChain::SwapChain(): m_device(VK_NULL_HANDLE), m_surface(VK_NULL_HANDLE), m_swapChain(VK_NULL_HANDLE), m_width(0), m_height(0)
{
}


SwapChain::~SwapChain()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool SwapChain::Create(const Device& _device, const VkSurfaceKHR& _surface, uint32 _width, uint32 _height)
{
	m_device = &_device;
	m_surface = _surface;
	m_width = _width;
	m_height = _height;

	return RecreateSwapChain();
}

void SwapChain::Destroy()
{
	if (m_swapChain != VK_NULL_HANDLE)
	{
		for (uint32 i = 0; i < m_swapChainImageViews.size(); ++i)
		{
			vkDestroyImageView(m_device->GetDevice(), m_swapChainImageViews[i], GpuMemoryManager::Instance().GetVK());
		}

		m_swapChainImageViews.clear();

		vkDestroySwapchainKHR(m_device->GetDevice(), m_swapChain, GpuMemoryManager::Instance().GetVK());
	}
	m_swapChain = VK_NULL_HANDLE;
}

bool SwapChain::RecreateSwapChain()
{
	if (!CreateSwapChain())
	{
		return false;
	}

	if (!CreateSwapChainImageViews())
	{
		return false;
	}

	return true;
}

bool SwapChain::CreateSwapChain()
{
	SwapChain::SwapChainDetails swapChainSupport = SwapChain::QuerySwapChainSupport(m_device->GetPhysicalDevice(), m_surface);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.m_formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.m_presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.m_capabilities);

	uint32 imageCount = swapChainSupport.m_capabilities.minImageCount + 1;
	if (swapChainSupport.m_capabilities.maxImageCount > 0 && imageCount > swapChainSupport.m_capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.m_capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1; // 2 for VR or generic stereoscopic 
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	const QueueFamily &indices = m_device->GetQueueFamily();
	uint32 queueFamilyIndices[] = { (uint32)indices.GetGraphicsFamily(), (uint32)indices.GetPresentFamily() };

	if (indices.GetGraphicsFamily() != indices.GetPresentFamily())
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.m_capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	VkSwapchainKHR oldSwapChain = m_swapChain;
	createInfo.oldSwapchain = oldSwapChain;

	VkSwapchainKHR newSwapChain;
	VkResult result = vkCreateSwapchainKHR(m_device->GetDevice(), &createInfo, GpuMemoryManager::Instance().GetVK(), &newSwapChain);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Failed to create swap chain");

	m_swapChain = newSwapChain;

	vkGetSwapchainImagesKHR(m_device->GetDevice(), m_swapChain, &imageCount, nullptr);
	m_swapChainImages.resize(imageCount);

	vkGetSwapchainImagesKHR(m_device->GetDevice(), m_swapChain, &imageCount, m_swapChainImages.data());
	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;

	return true;
}

bool SwapChain::CreateSwapChainImageViews()
{
	m_swapChainImageViews.resize(m_swapChainImages.size(), VK_NULL_HANDLE);

	for (uint32 i = 0; i < m_swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_swapChainImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = m_swapChainImageFormat;
		viewInfo.components = {};
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.flags = 0;

		VkResult result = vkCreateImageView(m_device->GetDevice(), &viewInfo, GpuMemoryManager::Instance().GetVK(), &m_swapChainImageViews[i]);
		nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot get swap chain image");
	}

	return true;
}

VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const eos::Vector<VkSurfaceFormatKHR, SwapChainAllocator, GetAllocator>& _availableFormats)
{
	if (_availableFormats.size() == 1 && _availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const VkSurfaceFormatKHR& availableFormat : _availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return _availableFormats[0];
}

VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const eos::Vector<VkPresentModeKHR, SwapChainAllocator, GetAllocator>& _availablePresentModes)
{
	for (const VkPresentModeKHR& availablePresentMode : _availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { m_width, m_height };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}


NW_NAMESPACE_END