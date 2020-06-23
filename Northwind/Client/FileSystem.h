#pragma once

#include "../Core/MemoryWrapper.h"
#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class NW_DLL FileSystem final
{
public:
	static void ConvertSlash(nwString& _path);
	static nwString ConvertSlash(const nwString& _path);
	static nwString GetFileName(const nwString& _path);
	static nwString GetFileExt(const nwString& _file);
	static nwString GetFileLeftMostExt(const nwString& _file);
	static nwString RemoveExt(const nwString& _file);

public:
	FileSystem(const nwString& _mainPath,
		const nwString& _shadersPath,
		const nwString& _texturesPath,
		const nwString& _modelsPath);
	~FileSystem();

	const nwString& GetMainPath() const { return m_mainPath; }
	const nwString& GetShadersPath() const { return m_shadersPath; }
	const nwString& GetTexturesPath() const { return m_texturesPath; }
	const nwString& GetModelsPath() const { return m_modelsPath; }

	nwString GetShaderTypeExt(const nwString& _name) const;

private:
	bool GetFullPath(const nwString& partialPath, nwString& _fullPath);

private:
	nwString m_mainPath;
	nwString m_shadersPath;
	nwString m_texturesPath;
	nwString m_modelsPath;
};

NW_NAMESPACE_END