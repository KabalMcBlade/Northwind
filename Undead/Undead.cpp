// Undead.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "Undead.h"

UD_NAMESPACE_BEGIN

UndeadExporter::UndeadExporter()
{
	m_version = 1;
}

UndeadExporter::~UndeadExporter()
{

}

int32 UndeadExporter::GetVersion()
{
	return m_version;
}

UD_NAMESPACE_END