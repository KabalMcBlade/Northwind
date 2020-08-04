#include "EngineApp.h"

#include "../Core/MemoryWrapper.h"
#include "../Core/SettingsDefines.h"
#include "../Core/Assertions.h"

#include "CommandLineParser.h"


NW_NAMESPACE_BEGIN

EngineApp::EngineApp(const char* _name, uint32 _version, const CommandLineParser& _commandLine, const VkPhysicalDeviceFeatures& _enabledFeatures /*= {}*/)
	: m_name(_name)
	, m_version(_version)
	, m_commandLine(_commandLine)
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
	Cleanup();

	m_device.Destroy();
	m_instance.Destroy();
}

NW_NAMESPACE_END