#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "../Renderer/RenderQueueFamily.h"
#include "../Renderer/RenderDevice.h"
#include "../Renderer/RenderInstance.h"


NW_NAMESPACE_BEGIN

class CommandLineParser;
class NW_DLL EngineApp
{
public:
	EngineApp(const char* _name, uint32 _version, const CommandLineParser& _commandLine, const VkPhysicalDeviceFeatures& _enabledFeatures = {});
	~EngineApp();

	NW_INLINE const VkInstance& GetInstance() const { return m_instance.GetInstance(); }
	NW_INLINE const VkDevice& GetDevice() const { return m_device.GetDevice(); }
	NW_INLINE const VkPhysicalDevice& GetPhysicalDevice() const { return m_device.GetPhysicalDevice(); }
	NW_INLINE const VkPhysicalDeviceFeatures& GetEnabledFeatures() const { return m_enabledFeatures; }
	NW_INLINE const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return m_device.GetPhysicalDeviceProperties(); }

	virtual const VkSurfaceKHR& GetSurafe() const = 0;

	void Execute();

protected:
	virtual void InitWindow() {}
	virtual void InitEngine() {}
	virtual void MainLoop() {}
	virtual void Cleanup() {}

private:
	void InternalInitWindow();
	void InternalInitEngine();
	void InternalMainLoop();
	void InternalCleanup();


private:
	RenderInstance m_instance;
	RenderDevice m_device;

protected:
	const CommandLineParser& m_commandLine;
	VkPhysicalDeviceFeatures m_enabledFeatures;
	uint32 m_version;
	const char* m_name;
};

NW_NAMESPACE_END