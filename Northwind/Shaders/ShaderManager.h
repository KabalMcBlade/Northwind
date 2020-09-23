#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/MemoryWrapper.h"
#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "Shader.h"


NW_NAMESPACE_BEGIN

class Device;
class NW_DLL ShaderManager final : public eos::NoCopyableMoveable
{
public:
	static ShaderManager& Instance();

	Shader* Load(const Device& _device, const nwString& _path);

	void Destroy(const nwString& _path);
	void DestroyAll();

	Shader* GetShader(const nwString& _path) const;

private:
	ShaderManager();
	~ShaderManager();

private:
	using ShaderManagerAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static ShaderManagerAllocator* GetAllocator();

	eos::Map<uint32, Shader*, ShaderManagerAllocator, GetAllocator> m_shaders;
};

NW_NAMESPACE_END