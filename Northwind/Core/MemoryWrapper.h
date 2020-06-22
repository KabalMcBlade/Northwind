#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "BasicDefines.h"
#include "BasicTypes.h"


NW_NAMESPACE_BEGIN


using GenericAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;

extern NW_DLL GenericAllocator* GetGenericCommonAllocator();
extern NW_DLL GenericAllocator* GetGenericStringAllocator();

using udString = eos::String<GenericAllocator, GetGenericStringAllocator>;
using udWString = eos::WString<GenericAllocator, GetGenericStringAllocator>;

using udOStream = eos::OStream<GenericAllocator, GetGenericCommonAllocator>;
using udStringStream = eos::StringStream<GenericAllocator, GetGenericCommonAllocator>;
using udIStringStream = eos::IStringStream<GenericAllocator, GetGenericCommonAllocator>;

using udWOStream = eos::WOStream<GenericAllocator, GetGenericCommonAllocator>;
using udWStringStream = eos::WStringStream<GenericAllocator, GetGenericCommonAllocator>;
using udWIStringStream = eos::WIStringStream<GenericAllocator, GetGenericCommonAllocator>;

NW_NAMESPACE_END

