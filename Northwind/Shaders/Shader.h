#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"
#include "../Core/MemoryWrapper.h"


NW_NAMESPACE_BEGIN

class FileSystem;
class NW_DLL Shader final
{
public:
	Shader();
	~Shader();

	bool Create(const VkDevice& _device, const nwString& _path);
	void Destroy();

	NW_INLINE operator const VkShaderModule& () const { return m_module; }
	NW_INLINE operator VkShaderModule& () { return m_module; }

	NW_INLINE operator const VkShaderStageFlagBits& () const { return m_stage; }
	NW_INLINE operator VkShaderStageFlagBits& () { return m_stage; }

	NW_INLINE const uint32 GetId() const { return m_hash; }

private:
	static VkShaderStageFlagBits ConvertExtToShaderStage(const nwString& _ext);

private:
	VkDevice m_device;
	VkShaderModule m_module;
	VkShaderStageFlagBits m_stage;
	uint32 m_hash;
};

NW_NAMESPACE_END
