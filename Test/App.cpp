#include "pch.h"
#include "App.h"


UD_USING_NAMESPACE


void App::InitWindow()
{
	glfwInit();

	int isVulkanSupported = glfwVulkanSupported();
	udAssertReturnVoid(isVulkanSupported == GLFW_TRUE, "No Vulkan Support.");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	bool fullScreen = false;
	bool showcursor = true;

	if (fullScreen)
	{
		m_window = glfwCreateWindow(m_width, m_height, m_name, glfwGetPrimaryMonitor(), nullptr);
	}
	else
	{
		m_window = glfwCreateWindow(m_width, m_height, m_name, nullptr, nullptr);
	}

	if (!showcursor)
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	glfwGetFramebufferSize(m_window, &m_frameWidth, &m_frameheight);

	VkResult result = glfwCreateWindowSurface(m_instance.GetInstance(), m_window, GpuMemoryManager::Instance().GetVK(), &m_surface);
	udAssertReturnVoid(result == VK_SUCCESS, "Failed to create window surface.");

	m_enabledFeatures = {};
	m_enabledFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
	m_enabledFeatures.geometryShader = VK_TRUE;
}

void App::InitEngine()
{
	GpuMemoryManager::Instance().Init(GetPhysicalDevice(), GetDevice(), GetPhysicalDeviceProperties().limits.bufferImageGranularity);
}

void App::MainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
	}
}

void App::Cleanup()
{
	vkDestroySurfaceKHR(m_instance.GetInstance(), m_surface, GpuMemoryManager::Instance().GetVK());

	glfwDestroyWindow(m_window);

	glfwTerminate();

	GpuMemoryManager::Instance().Shutdown();
}