#include "EngineApp.h"

#include "../Core/MemoryWrapper.h"
#include "../Core/SettingsDefines.h"
#include "../Core/Assertions.h"


UD_NAMESPACE_BEGIN

EngineApp::EngineApp(uint32 _width, uint32 _height, const char* _name, uint32 _version, const VkPhysicalDeviceFeatures& _enabledFeatures /*= {}*/)
	: m_width(_width)
	, m_height(_height)
	, m_name(_name)
	, m_version(_version)
	, m_enabledFeatures(_enabledFeatures)
{
}

EngineApp::~EngineApp()
{
}

void EngineApp::Execute()
{
	InternalInitWindow();
	InternalInitEngine();
	InternalMainLoop();		// this has the loop
	InternalCleanup();
}

void EngineApp::InternalInitWindow()
{
	m_instance.Create(m_name, m_version);

	InitWindow();

	m_device.Create(m_instance.GetInstance(), GetSurafe(), m_enabledFeatures);
}

void EngineApp::InternalInitEngine()
{
	InitEngine();
}

void EngineApp::InternalMainLoop()
{
	MainLoop();		// this has the loop
}

void EngineApp::InternalCleanup()
{
	m_device.Destroy();

	Cleanup();

	m_instance.Destroy();
}

UD_NAMESPACE_END