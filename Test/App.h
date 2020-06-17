#pragma once

#define GLFW_INCLUDE_VULKAN
#include "Dependencies/Include/GLFW/glfw3.h"

#include "../Undead/Undead.h"

UD_USING_NAMESPACE

class App
{
public:
	App(uint32 _width, uint32 _height, const char* _name, uint32 _version);
	~App();

	const VkInstance& GetInstance() const { return m_instance.GetInstance(); }
	const VkDevice& GetDevice() const { return m_device.GetDevice(); }
	const VkPhysicalDevice& GetPhysicalDevice() const { return m_device.GetPhysicalDevice(); }
	const VkPhysicalDeviceFeatures& GetEnabledFeatures() const { return m_enabledFeatures; }
	const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return m_device.GetPhysicalDeviceProperties(); }

	int32 GetFrameWidth() const { return m_frameWidth; }
	int32 GetFrameHeight() const { return m_frameheight; }

	uint32 GetWidth() const { return m_width; }
	uint32 GetHeight() const { return m_height; }

	const VkSurfaceKHR& GetSurafe() const { return m_surface; }
	const GLFWwindow* GetWindow() const { return m_window; }

private:
	RenderInstance m_instance;
	RenderDevice m_device;

	VkPhysicalDeviceFeatures m_enabledFeatures;
	VkSurfaceKHR m_surface;
	GLFWwindow* m_window;

	int32 m_frameWidth;
	int32 m_frameheight;
	uint32 m_width;
	uint32 m_height;

	const char* m_name;
};

