#include "stdafx.h"
#include "Settings.h"


UD_NAMESPACE_BEGIN

Settings::SettingsAllocator* Settings::GetAllocator()
{
	static const uint32 kSettingsAllocatorSize = 1024;
	static eos::HeapArea<kSettingsAllocatorSize> memoryArea;
	static SettingsAllocator memoryAllocator(memoryArea, "SettingAllocator");

	return &memoryAllocator;
}

Settings& Settings::Instance()
{
	static Settings instance;
	return instance;
}

Settings::Settings()
{
}


Settings::~Settings()
{
}

UD_NAMESPACE_END