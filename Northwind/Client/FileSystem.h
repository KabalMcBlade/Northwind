#pragma once

#include "../Core/MemoryWrapper.h"
#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class NW_DLL FileSystem final
{
public:
	static void ConvertSlash(udString& _path);
	static udString ConvertSlash(const udString& _path);
	static udString GetFileName(const udString& _path);
	static udString GetFileExt(const udString& _file);
	static udString GetFileLeftMostExt(const udString& _file);
	static udString RemoveExt(const udString& _file);

public:
	FileSystem(const udString& _mainPath,
		const udString& _shadersPath,
		const udString& _texturesPath,
		const udString& _modelsPath);
	~FileSystem();

	const udString& GetMainPath() const { return m_mainPath; }
	const udString& GetShadersPath() const { return m_shadersPath; }
	const udString& GetTexturesPath() const { return m_texturesPath; }
	const udString& GetModelsPath() const { return m_modelsPath; }

	udString GetShaderTypeExt(const udString& _name) const;

private:
	bool GetFullPath(const udString& partialPath, udString& _fullPath);

private:
	udString m_mainPath;
	udString m_shadersPath;
	udString m_texturesPath;
	udString m_modelsPath;
};

NW_NAMESPACE_END