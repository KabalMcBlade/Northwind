#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "../Renderer/QueueFamily.h"
#include "../Renderer/Device.h"
#include "../Renderer/Instance.h"
#include "../Renderer/StagingBufferManager.h"

NW_NAMESPACE_BEGIN

class CommandLineParser;
class NW_DLL EngineApp
{
public:
	EngineApp(const char* _name, uint32 _version, const CommandLineParser& _commandLine, const VkPhysicalDeviceFeatures& _enabledFeatures = {});
	~EngineApp();

	void Execute();

protected:
	virtual const VkSurfaceKHR& GetSurafe() const = 0;

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


protected:
	const CommandLineParser& m_commandLine;
	Instance m_instance;
	Device m_device;
	VkPhysicalDeviceFeatures m_enabledFeatures;
	uint32 m_version;
	const char* m_name;
};

NW_NAMESPACE_END