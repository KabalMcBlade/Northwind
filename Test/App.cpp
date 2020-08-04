#include "pch.h"
#include "App.h"


NW_USING_NAMESPACE


static void KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
	if (_key == GLFW_KEY_ESCAPE && _action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(_window, GLFW_TRUE);
	}
}


//////////////////////////////////////////////////////////////////////////

void App::InitWindow()
{
	m_width = m_commandLine.GetValue<uint32>("-width", 1024);
	m_height = m_commandLine.GetValue<uint32>("-height", 768);

	glfwInit();

	int isVulkanSupported = glfwVulkanSupported();
	nwAssertReturnVoid(isVulkanSupported == GLFW_TRUE, "No Vulkan Support.");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	bool fullScreen = m_commandLine.GetValue<bool>("-fullscreen", false);
	bool showcursor = m_commandLine.GetValue<bool>("-showcursor", true);

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

	glfwSetKeyCallback(m_window, KeyCallback);

	glfwMakeContextCurrent(m_window);

	VkResult result = glfwCreateWindowSurface(m_instance.GetInstance(), m_window, GpuMemoryManager::Instance().GetVK(), &m_surface);
	nwAssertReturnVoid(result == VK_SUCCESS, "Failed to create window surface.");

	m_enabledFeatures = {};
	m_enabledFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
	m_enabledFeatures.geometryShader = VK_TRUE;
}

void App::InitEngine()
{
	GpuMemoryManager::Instance().Init(m_device.GetPhysicalDevice(), m_device.GetDevice(), m_device.GetPhysicalDeviceProperties().limits.bufferImageGranularity);
	StagingBufferManager::Instance().Create(m_device.GetDevice(), m_device.GetGraphicsQueue(), m_device.GetQueueFamily().GetGraphicsFamily());
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

	StagingBufferManager::Instance().Destroy();
	GpuMemoryManager::Instance().Shutdown();
}