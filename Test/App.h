#pragma once

#define GLFW_INCLUDE_VULKAN
#include "Dependencies/Include/GLFW/glfw3.h"

#include "../Northwind/Northwind.h"

NW_USING_NAMESPACE

class App final : public EngineApp
{
public:
	App(const char* _name, uint32 _version, const CommandLineParser& _commandLine, const FileSystem& _fileSystem) : EngineApp(_name, _version, _commandLine, _fileSystem) {}
	~App() {}

	NW_INLINE const GLFWwindow* GetWindow() const { return m_window; }

	NW_INLINE int32 GetFrameWidth() const { return m_frameWidth; }
	NW_INLINE int32 GetFrameHeight() const { return m_frameheight; }

	NW_INLINE uint32 GetWidth() const { return m_width; }
	NW_INLINE uint32 GetHeight() const { return m_height; }

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

