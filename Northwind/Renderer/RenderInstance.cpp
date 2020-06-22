#include "RenderInstance.h"

#include "../Core/MemoryWrapper.h"
#include "../Core/SettingsDefines.h"
#include "../Core/Assertions.h"

#include "../GPU/GpuMemoryManager.h"


NW_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
{
	udString prefix;
	if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
	{
		prefix = "[INFO]";
	};
	if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		prefix = "[WARNING]";
	};
	if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
	{
		prefix = "[PERFORMANCE]";
	};
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		prefix = "[ERROR]";
	};
	if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
	{
		prefix = "[DEBUG]";
	}

	std::cerr << prefix << "[TYPE:" << layerPrefix << "]" << std::endl << msg << std::endl << std::endl;
	return VK_FALSE;
}

//////////////////////////////////////////////////////////////////////////

using RenderInstanceMemoryAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;

namespace
{
	RenderInstanceMemoryAllocator* GetAllocator()
	{
		static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kRenderInstanceAllocatorSize);
		static RenderInstanceMemoryAllocator memoryAllocator(memoryArea, "RenderInstanceMemoryAllocator");

		return &memoryAllocator;
	}
}

//////////////////////////////////////////////////////////////////////////

RenderInstance::RenderInstance() : m_instance(VK_NULL_HANDLE), m_debugReportCreated(false)
{
#if _DEBUG
	m_validationLayerEnabled = true;
#else
	m_validationLayerEnabled = false;
#endif
}

RenderInstance::~RenderInstance()
{
	if (m_validationLayerEnabled)
	{
		DestroyDebugReport();
	}

	Destroy();
}

bool RenderInstance::Create(const char* _appName, uint32 _appVersion /*= VK_MAKE_VERSION(0, 0, 1)*/)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = _appName;
	appInfo.pEngineName = "Northwind";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;
	appInfo.applicationVersion = _appVersion;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.pApplicationInfo = &appInfo;

	eos::Vector<const char*, RenderInstanceMemoryAllocator, GetAllocator> enabledExtensions;
	enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

	VkDebugReportCallbackCreateInfoEXT validationCreateInfo = {};

	eos::Vector<const char*, RenderInstanceMemoryAllocator, GetAllocator> enabledLayers;
	if (m_validationLayerEnabled && !m_debugReportCreated)
	{
		enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

		enabledLayers.push_back("VK_LAYER_LUNARG_standard_validation");

		uint32 layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		eos::Vector<VkLayerProperties, RenderInstanceMemoryAllocator, GetAllocator> layers;
		layers.resize(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		bool found = false;
		for (uint32 i = 0; i < layerCount; ++i)
		{
			if (std::strcmp("VK_LAYER_LUNARG_standard_validation", layers[i].layerName) == 0)
			{
				found = true;
				break;
			}
		}
		udAssertReturnValue(found, false, "Cannot find validation layer");

		PopulateDebugReport(validationCreateInfo);
		createInfo.pNext = (VkDebugReportCallbackCreateInfoEXT*)&validationCreateInfo;

		createInfo.enabledLayerCount = (uint32)enabledLayers.size();
		createInfo.ppEnabledLayerNames = enabledLayers.data();
	}

	createInfo.enabledExtensionCount = (uint32)enabledExtensions.size();
	createInfo.ppEnabledExtensionNames = enabledExtensions.data();

	VkResult result = vkCreateInstance(&createInfo, GpuMemoryManager::Instance().GetVK(), &m_instance);

	if (m_validationLayerEnabled && !m_debugReportCreated && result == VK_SUCCESS)
	{
		CreateDebugReport(validationCreateInfo);

		m_debugReportCreated = true;
	}

	udAssertReturnValue(result == VK_SUCCESS, false, "Cannot create instance");

	return true;
}

void RenderInstance::Destroy()
{
	if (m_instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(m_instance, GpuMemoryManager::Instance().GetVK());
		m_instance = VK_NULL_HANDLE;
	}
}

void RenderInstance::PopulateDebugReport(VkDebugReportCallbackCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
	createInfo.pfnCallback = DebugCallback;
}

void RenderInstance::CreateDebugReport(const VkDebugReportCallbackCreateInfoEXT& createInfo)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr)
	{
		VkResult result = func(m_instance, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_debugCallback);
		udAssertReturnVoid(result == VK_SUCCESS, "Impossible create Debug Report!");
	}
}

void RenderInstance::DestroyDebugReport()
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr)
	{
		func(m_instance, m_debugCallback, GpuMemoryManager::Instance().GetVK());
	}
}

NW_NAMESPACE_END