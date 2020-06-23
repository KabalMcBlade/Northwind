#pragma once

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

namespace HashTools
{
	extern uint8 MakeHash8(const void* _data, uint32 _size, uint8 _seed = 0);
	extern uint16 MakeHash16(const void* _data, uint32 _size, uint16 _seed = 0);
	extern uint32 MakeHash32(const void* _data, uint32 _size, uint32 _seed = 0);
	extern uint64 MakeHash64(const void* _data, uint32 _size, uint64 _seed = 0);
}

NW_NAMESPACE_END