#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "../Renderer/RenderQueueFamily.h"
#include "../Renderer/RenderDevice.h"
#include "../Renderer/RenderInstance.h"


UD_NAMESPACE_BEGIN

class UD_DLL EngineApp
{
public:
	EngineApp(uint32 _width, uint32 _height, const char* _name, uint32 _version, const VkPhysicalDeviceFeatures& _enabledFeatures = {});
	~EngineApp();

	UD_INLINE const VkInstance& GetInstance() const { return m_instance.GetInstance(); }
	UD_INLINE const VkDevice& GetDevice() const { return m_device.GetDevice(); }
	UD_INLINE const VkPhysicalDevice& GetPhysicalDevice() const { return m_device.GetPhysicalDevice(); }
	UD_INLINE const VkPhysicalDeviceFeatures& GetEnabledFeatures() const { return m_enabledFeatures; }
	UD_INLINE const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return m_device.GetPhysicalDeviceProperties(); }

	UD_INLINE int32 GetFrameWidth() const { return m_frameWidth; }
	UD_INLINE int32 GetFrameHeight() const { return m_frameheight; }

	UD_INLINE uint32 GetWidth() const { return m_width; }
	UD_INLINE uint32 GetHeight() const { return m_height; }

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

protected:
	RenderInstance m_instance;
	RenderDevice m_device;

	VkPhysicalDeviceFeatures m_enabledFeatures;

	uint32 m_version;

	int32 m_frameWidth;
	int32 m_frameheight;
	uint32 m_width;
	uint32 m_height;

	const char* m_name;
};

UD_NAMESPACE_END