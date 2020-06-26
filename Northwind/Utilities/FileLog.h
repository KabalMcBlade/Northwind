#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class NW_DLL FileLog : public eos::NoCopyableMoveable
{
public:
	FileLog();
	~FileLog();
	bool Open(const char* _fileName, const char* _mode = "w");
	bool Close();

	bool Write(const char* message);

private:
	FILE* m_file;
	bool m_opened;
};

NW_NAMESPACE_END
