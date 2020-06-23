#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "BasicDefines.h"
#include "BasicTypes.h"


NW_NAMESPACE_BEGIN


using GenericAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;

extern NW_DLL GenericAllocator* GetGenericCommonAllocator();
extern NW_DLL GenericAllocator* GetGenericStringAllocator();

using nwString = eos::String<GenericAllocator, GetGenericStringAllocator>;
using nwWString = eos::WString<GenericAllocator, GetGenericStringAllocator>;

using nwOStream = eos::OStream<GenericAllocator, GetGenericCommonAllocator>;
using nwStringStream = eos::StringStream<GenericAllocator, GetGenericCommonAllocator>;
using nwIStringStream = eos::IStringStream<GenericAllocator, GetGenericCommonAllocator>;

using nwWOStream = eos::WOStream<GenericAllocator, GetGenericCommonAllocator>;
using nwWStringStream = eos::WStringStream<GenericAllocator, GetGenericCommonAllocator>;
using nwWIStringStream = eos::WIStringStream<GenericAllocator, GetGenericCommonAllocator>;

NW_NAMESPACE_END

