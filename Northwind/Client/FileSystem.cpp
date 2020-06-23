#include "FileSystem.h"


NW_NAMESPACE_BEGIN

void FileSystem::ConvertSlash(nwString& _path)
{
	static const nwString from = "\\";
	static const nwString to = "/";

	size start_pos = 0;
	while ((start_pos = _path.find(from, start_pos)) != std::string::npos)
	{
		_path.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

nwString FileSystem::ConvertSlash(const nwString& _path)
{
	static const nwString from = "\\";
	static const nwString to = "/";

	nwString result = _path;

	size start_pos = 0;
	while ((start_pos = result.find(from, start_pos)) != std::string::npos)
	{
		result.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}

	return result;
}

nwString FileSystem::GetFileName(const nwString& _path)
{
	nwString path = ConvertSlash(_path);

	size i = path.find_last_of("/");
	if (i != nwString::npos)
	{
		return path.substr(i + 1);
	}

	return("");
}

nwString FileSystem::GetFileExt(const nwString& _file)
{
	size i = _file.rfind('.', _file.length());
	if (i != nwString::npos)
	{
		return(_file.substr(i + 1, _file.length() - i));
	}

	return("");
}

nwString FileSystem::GetFileLeftMostExt(const nwString& _file)
{
	nwString file = ConvertSlash(_file);

	size index = file.find_last_of("/");
	file = file.substr(index + 1);

	size i = file.find('.', 0);
	if (i != nwString::npos)
	{
		return(file.substr(i + 1, file.length() - i));
	}

	return("");
}

nwString FileSystem::RemoveExt(const nwString& _file)
{
	size i = _file.find_last_of(".");
	if (i != nwString::npos)
	{
		return(_file.substr(0, i));
	}

	return("");
}

FileSystem::FileSystem(const nwString& _mainPath,
	const nwString& _shadersPath,
	const nwString& _texturesPath,
	const nwString& _modelsPath)
{
	GetFullPath("./", m_mainPath);

	ConvertSlash(m_mainPath);

	m_mainPath.append(_mainPath);
	m_mainPath.append("/");

	m_shadersPath = m_mainPath;
	m_shadersPath.append(_shadersPath);
	m_shadersPath.append("/");

	m_texturesPath = m_mainPath;
	m_texturesPath.append(_texturesPath);
	m_texturesPath.append("/");

	m_modelsPath = m_mainPath;
	m_modelsPath.append(_modelsPath);
	m_modelsPath.append("/");
}


FileSystem::~FileSystem()
{
}

nwString FileSystem::GetShaderTypeExt(const nwString& _name) const
{
	nwString leftmostExt = Northwind::FileSystem::GetFileLeftMostExt(_name);
	return Northwind::FileSystem::RemoveExt(leftmostExt);
}

bool FileSystem::GetFullPath(const nwString& partialPath, nwString& _fullPath)
{
	char tmp[_MAX_PATH];
	if (_fullpath(tmp, partialPath.c_str(), _MAX_PATH) != NULL)
	{
		_fullPath = tmp;
		return true;
	}
	return false;
}

NW_NAMESPACE_END