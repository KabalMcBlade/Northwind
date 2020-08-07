#include "Shader.h"

#include "../Core/SettingsDefines.h"
#include "../Core/Assertions.h"
#include "../Client/FileSystem.h"
#include "../GPU/GpuMemoryManager.h"
#include "../Utilities/HashTools.h"


NW_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////

using ShaderAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;

namespace
{
	ShaderAllocator* GetAllocator()
	{
		static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kShaderAllocatorSize);
		static ShaderAllocator memoryAllocator(memoryArea, "ShaderAllocator");

		return &memoryAllocator;
	}
}
//////////////////////////////////////////////////////////////////////////


constexpr uint32 kShaderNameHashSeed = 43690;

VkShaderStageFlagBits Shader::ConvertExtToShaderStage(const nwString& _ext)
{
	if (_ext == "vert")
	{
		return VK_SHADER_STAGE_VERTEX_BIT;
	}
	if (_ext == "ctrl")
	{
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	}
	if (_ext == "eval")
	{
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}
	if (_ext == "geom")
	{
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	}
	if (_ext == "frag")
	{
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	if (_ext == "comp")
	{
		return VK_SHADER_STAGE_COMPUTE_BIT;
	}

	nwAssertReturnValue(false, VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM, "No Valid conversion found!");
}

Shader::Shader(): m_device(VK_NULL_HANDLE), m_module(VK_NULL_HANDLE)
{
}

Shader::~Shader()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool Shader::Create(const VkDevice& _device, const nwString& _path)
{
	m_device = _device;

	nwString type = FileSystem::GetShaderTypeExt(_path);

	m_stage = ConvertExtToShaderStage(type);

	std::ifstream fileStream(_path.c_str(), std::ios::binary);

	fileStream.seekg(0, std::ios_base::end);
	const size fileSize = fileStream.tellg();

	nwAssertReturnValue(fileSize > 0, false, "Shader file size is 0!");
	nwAssertReturnValue(fileSize % sizeof(uint32) == 0, false, "Not valid file buffer");

	char* binary = reinterpret_cast<char*>(eosNewAlignedRaw(fileSize, GetAllocator(), alignof(char)));

	fileStream.seekg(0, std::ios_base::beg);
	fileStream.read(binary, fileSize);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = fileSize;
	createInfo.pCode = reinterpret_cast<const uint32*>(binary);

	VkResult result = vkCreateShaderModule(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_module);

	eosDeleteRaw(binary, GetAllocator());

	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create shader!");

	m_hash = HashTools::MakeHash32(_path.c_str(), static_cast<uint32>(_path.length()), kShaderNameHashSeed);

	return true;
}

void Shader::Destroy()
{
	if (m_module != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(m_device, m_module, GpuMemoryManager::Instance().GetVK());
		m_module = VK_NULL_HANDLE;
	}
}


NW_NAMESPACE_END