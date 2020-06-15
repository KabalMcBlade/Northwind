#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "BasicDefines.h"
#include "BasicTypes.h"


UD_NAMESPACE_BEGIN

class UD_DLL Settings final : public eos::NoCopyableMoveable
{
public:
	static Settings& Instance();

private:
	using SettingsAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	SettingsAllocator* GetAllocator();

	Settings();
	~Settings();
};

UD_NAMESPACE_END
