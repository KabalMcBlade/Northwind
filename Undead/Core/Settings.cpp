#include "Settings.h"

#include "SettingsDefines.h"

#include "../Core/Assertions.h"

#include "../Dependencies/json.hpp"


UD_NAMESPACE_BEGIN

static constexpr const char* kMemorySettingsFileName = "MemorySettings.json";

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

void Settings::LoadMemorySettings()
{
	// if is no file, it creates and fill with basic information and default values.
	std::ifstream is(kMemorySettingsFileName);
	if (!is.is_open())
	{
		// create the file and read again
		nlohmann::json nj = {
			{"Engine", {
				//{"None", 0}
			}},
			{"Generic", {
				{"Common", SettingsDefines::Generic::kCommonAllocatorSize},
				{"String", SettingsDefines::Generic::kStringAllocatorSize}
			}}
		};

 		std::ofstream os(kMemorySettingsFileName);
		os << nj;
		os.close();

		is.open(kMemorySettingsFileName);
		udAssertReturnVoid(is.is_open(), "Cannot open nor create the %s file", kMemorySettingsFileName);
	}

	nlohmann::json json;
	is >> json;
	is.close();

	// actual settings
	SettingsDefines::Generic::kCommonAllocatorSize = json["Generic"]["Common"];
	SettingsDefines::Generic::kStringAllocatorSize = json["Generic"]["String"];
}

UD_NAMESPACE_END