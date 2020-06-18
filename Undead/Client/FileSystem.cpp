#include "FileSystem.h"


UD_NAMESPACE_BEGIN

void FileSystem::ConvertSlash(udString& _path)
{
	static const udString from = "\\";
	static const udString to = "/";

	size start_pos = 0;
	while ((start_pos = _path.find(from, start_pos)) != std::string::npos)
	{
		_path.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

udString FileSystem::ConvertSlash(const udString& _path)
{
	static const udString from = "\\";
	static const udString to = "/";

	udString result = _path;

	size start_pos = 0;
	while ((start_pos = result.find(from, start_pos)) != std::string::npos)
	{
		result.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}

	return result;
}

udString FileSystem::GetFileName(const udString& _path)
{
	udString path = ConvertSlash(_path);

	size i = path.find_last_of("/");
	if (i != udString::npos)
	{
		return path.substr(i + 1);
	}

	return("");
}

udString FileSystem::GetFileExt(const udString& _file)
{
	size i = _file.rfind('.', _file.length());
	if (i != udString::npos)
	{
		return(_file.substr(i + 1, _file.length() - i));
	}

	return("");
}

udString FileSystem::GetFileLeftMostExt(const udString& _file)
{
	udString file = ConvertSlash(_file);

	size index = file.find_last_of("/");
	file = file.substr(index + 1);

	size i = file.find('.', 0);
	if (i != udString::npos)
	{
		return(file.substr(i + 1, file.length() - i));
	}

	return("");
}

udString FileSystem::RemoveExt(const udString& _file)
{
	size i = _file.find_last_of(".");
	if (i != udString::npos)
	{
		return(_file.substr(0, i));
	}

	return("");
}

FileSystem::FileSystem(const udString& _mainPath,
	const udString& _shadersPath,
	const udString& _texturesPath,
	const udString& _modelsPath)
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

udString FileSystem::GetShaderTypeExt(const udString& _name) const
{
	udString leftmostExt = Undead::FileSystem::GetFileLeftMostExt(_name);
	return Undead::FileSystem::RemoveExt(leftmostExt);
}

bool FileSystem::GetFullPath(const udString& partialPath, udString& _fullPath)
{
	char tmp[_MAX_PATH];
	if (_fullpath(tmp, partialPath.c_str(), _MAX_PATH) != NULL)
	{
		_fullPath = tmp;
		return true;
	}
	return false;
}

UD_NAMESPACE_END