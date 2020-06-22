#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class Instance final
{
public:
	Instance();
	~Instance();

	bool Create(const char* _appName, uint32 _appVersion = VK_MAKE_VERSION(0, 0, 1));
	void Destroy();

	NW_INLINE bool IsInitialized() const { return m_instance != VK_NULL_HANDLE; }
	NW_INLINE const VkInstance& GetInstance() const { return m_instance; }

private:
	void PopulateDebugReport(VkDebugReportCallbackCreateInfoEXT& _createInfo);
	void CreateDebugReport(const VkDebugReportCallbackCreateInfoEXT& _createInfo);
	void DestroyDebugReport();

private:
	VkInstance m_instance;
	VkDebugReportCallbackEXT m_debugCallback;
	bool m_validationLayerEnabled;
	bool m_debugReportCreated;
};

NW_NAMESPACE_END
