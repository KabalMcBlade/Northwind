#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "BasicDefines.h"
#include "BasicTypes.h"


UD_NAMESPACE_BEGIN

class UD_DLL Settings final : public eos::NoCopyableMoveable
{
public:
	static Settings& Instance();

public:
	// This is hard-coded, the file is MemorySettings.json, and it is at the root level
	void LoadMemorySettings();

private:
	Settings();
	~Settings();
};

UD_NAMESPACE_END
