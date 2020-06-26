#include "FileLog.h"

NW_NAMESPACE_BEGIN

FileLog::FileLog() : m_file(nullptr), m_opened(false)
{
}


FileLog::~FileLog()
{
}

bool FileLog::Open(const char* _fileName, const char* _mode /*= "w"*/)
{
	if (fopen_s(&m_file, _fileName, _mode) != 0)
	{
		return false;
	}

	m_opened = true;

	return true;
}

bool FileLog::Close()
{
	if (!m_opened)
	{
		return false;
	}

	if (m_file != nullptr)
	{
		fclose(m_file);
		m_file = nullptr;
	}

	m_opened = false;

	return true;
}

bool FileLog::Write(const char* message)
{
	if (!m_opened)
	{
		return false;
	}

	fputs(message, m_file);

	return true;
}

NW_NAMESPACE_END