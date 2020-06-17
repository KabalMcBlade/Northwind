#pragma once

#include "Dependencies/Eos/Eos/Eos.h"
#include "Dependencies/Nix/Nix/Nix.h"

#include "Core/BasicDefines.h"
#include "Core/Assertions.h"
#include "Core/BasicTypes.h"
#include "Core/Settings.h"
#include "Core/SettingsDefines.h"
#include "Core/MemoryWrapper.h"

#include "GPU/GpuAllocator.h"
#include "GPU/GpuDataStructure.h"
#include "GPU/VulkanAllocator.h"
#include "GPU/GpuMemoryAllocator.h"
#include "GPU/GpuMemoryManager.h"
#include "GPU/GpuMemoryList.h"

#include "Renderer/RenderQueueFamily.h"
#include "Renderer/RenderDevice.h"
#include "Renderer/RenderQueueFamily.h"
#include "Renderer/RenderDevice.h"
#include "Renderer/RenderInstance.h"


UD_NAMESPACE_BEGIN

class UD_DLL UndeadExporter
{
public:
	UndeadExporter();
	~UndeadExporter();

	int32 GetVersion();

private:
	int32 m_version;
};

UD_NAMESPACE_END