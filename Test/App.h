#pragma once

#define GLFW_INCLUDE_VULKAN
#include "Dependencies/Include/GLFW/glfw3.h"

#include "../Undead/Undead.h"

UD_USING_NAMESPACE

class App final : public EngineApp
{
public:
	App(const char* _name, uint32 _version, const CommandLineParser& _commandLine) : EngineApp(_name, _version, _commandLine) {}
	~App() {}

	UD_INLINE const GLFWwindow* GetWindow() const { return m_window; }

	UD_INLINE int32 GetFrameWidth() const { return m_frameWidth; }
	UD_INLINE int32 GetFrameHeight() const { return m_frameheight; }

	UD_INLINE uint32 GetWidth() const { return m_width; }
	UD_INLINE uint32 GetHeight() const { return m_height; }

	virtual const VkSurfaceKHR& GetSurafe() const override { return m_surface; }

	virtual void InitWindow() override;
	virtual void InitEngine() override;
	virtual void MainLoop() override;
	virtual void Cleanup() override;

private:
	VkSurfaceKHR m_surface;
	GLFWwindow* m_window;

	int32 m_frameWidth;
	int32 m_frameheight;
	uint32 m_width;
	uint32 m_height;
};

