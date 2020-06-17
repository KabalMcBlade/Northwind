#pragma once

#define GLFW_INCLUDE_VULKAN
#include "Dependencies/Include/GLFW/glfw3.h"

#include "../Undead/Undead.h"

UD_USING_NAMESPACE

class App final : public EngineApp
{
public:
	App(uint32 _width, uint32 _height, const char* _name, uint32 _version) : EngineApp(_width, _height, _name, _version) {}
	~App() {}

	const GLFWwindow* GetWindow() const { return m_window; }

	virtual const VkSurfaceKHR& GetSurafe() const override { return m_surface; }

	virtual void InitWindow() override;
	virtual void InitEngine() override;
	virtual void MainLoop() override;
	virtual void Cleanup() override;

private:
	VkSurfaceKHR m_surface;
	GLFWwindow* m_window;
};

