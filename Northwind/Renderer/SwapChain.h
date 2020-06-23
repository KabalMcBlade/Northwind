#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class Device;

class SwapChain final
{
public:
	using SwapChainAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static SwapChainAllocator* GetAllocator();

	struct SwapChainDetails
	{
		VkSurfaceCapabilitiesKHR m_capabilities;
		eos::Vector<VkSurfaceFormatKHR, SwapChainAllocator, GetAllocator> m_formats;
		eos::Vector<VkPresentModeKHR, SwapChainAllocator, GetAllocator> m_presentModes;
	};

	SwapChain::SwapChainDetails QuerySwapChainSupport(const VkPhysicalDevice& _device, VkSurfaceKHR _surface);

public:
	SwapChain();
	~SwapChain();

	bool Create(const Device& _device, const VkSurfaceKHR& _surface, uint32 _width, uint32 _height);
	void Destroy();

	bool RecreateSwapChain();

	NW_INLINE operator VkSwapchainKHR() const { return m_swapChain; }

	NW_INLINE const eos::Vector<VkImageView, SwapChainAllocator, GetAllocator>& GetImageViews() const { return m_swapChainImageViews; }
	NW_INLINE const VkImageView& GetImageView(uint32 _index) const { return m_swapChainImageViews[_index]; }
	NW_INLINE const size GetImageViewsCOunt() const { return m_swapChainImageViews.size(); }

	NW_INLINE VkExtent2D GetExtent() const { return m_swapChainExtent; }
	NW_INLINE VkFormat GetFormat() const { return m_swapChainImageFormat; }
	NW_INLINE uint32 GetSwapImageSize() const { return static_cast<uint32>(m_swapChainImages.size()); }

private:
	bool CreateSwapChain();
	bool CreateSwapChainImageViews();

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const eos::Vector<VkSurfaceFormatKHR, SwapChainAllocator, GetAllocator>& _availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const eos::Vector<VkPresentModeKHR, SwapChainAllocator, GetAllocator>& _availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities);
	
private:
	const Device* m_device;
	VkSurfaceKHR m_surface;
	uint32 m_width;
	uint32 m_height;

	eos::Vector<VkImage, SwapChainAllocator, GetAllocator> m_swapChainImages;
	eos::Vector<VkImageView, SwapChainAllocator, GetAllocator> m_swapChainImageViews;
	VkSwapchainKHR m_swapChain;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
};

NW_NAMESPACE_END