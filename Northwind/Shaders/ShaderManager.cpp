#include "ShaderManager.h"


#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"
#include "../GPU/GpuMemoryManager.h"

#include "../Utilities/HashTools.h"

#include "../Renderer/Device.h"


NW_NAMESPACE_BEGIN

constexpr uint32 kShaderNameHashSeed = 17282;

ShaderManager::ShaderManagerAllocator* ShaderManager::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kShaderManagerAllocatorSize);
	static ShaderManagerAllocator memoryAllocator(memoryArea, "ShaderManagerAllocator");

	return &memoryAllocator;
}


ShaderManager& ShaderManager::Instance()
{
	static ShaderManager instance;
	return instance;
}


Shader* ShaderManager::Load(const Device& _device, const nwString& _path)
{
	if (_path.empty())
	{
		return nullptr;
	}

	Shader* shader = GetShader(_path);
	if (shader == nullptr)
	{
		shader = eosNew(Shader, GetAllocator());
		shader->Create(_device.GetDevice(), _path);

		uint32 hash = HashTools::MakeHash32(_path.c_str(), static_cast<uint32>(_path.length()), kShaderNameHashSeed);
		m_shaders[hash] = shader;
	}

	return shader;
}


//////////////////////////////////////////////////////////////////////////

void ShaderManager::Destroy(const nwString& _path)
{
	uint32 hash = HashTools::MakeHash32(_path.c_str(), static_cast<uint32>(_path.length()), kShaderNameHashSeed);

	auto search = m_shaders.find(hash);
	if (search != m_shaders.end())
	{
		if (search->second != nullptr)
		{
			// I do not need to call Destroy of the shader here, because destroying the pointer call the destructor of the inner instances
			eosDelete(search->second, GetAllocator());
		}

		m_shaders.erase(hash);
	}
}

void ShaderManager::DestroyAll()
{
	auto begin = m_shaders.begin(), end = m_shaders.end();
	eos::Map<uint32, Shader*, ShaderManagerAllocator, GetAllocator>::iterator it = begin;
	for (; it != end; ++it)
	{
		if (it->second != nullptr)
		{
			// I do not need to call Destroy of the shader here, because destroying the pointer call the destructor of the inner instances
			eosDelete(it->second, GetAllocator());
		}
	}

	m_shaders.clear();
}

Shader* ShaderManager::GetShader(const nwString& _path) const
{
	if (_path.empty())
	{
		return nullptr;
	}

	uint32 hash = HashTools::MakeHash32(_path.c_str(), static_cast<uint32>(_path.length()), kShaderNameHashSeed);

	auto search = m_shaders.find(hash);
	if (search != m_shaders.end())
	{
		return search->second;
	}
	else
	{
		return nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////

ShaderManager::ShaderManager()
{
}


ShaderManager::~ShaderManager()
{
}

NW_NAMESPACE_END