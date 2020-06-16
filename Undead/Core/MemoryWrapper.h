#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "BasicDefines.h"
#include "BasicTypes.h"


UD_NAMESPACE_BEGIN

template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(T)> using udVector = eos::Vector<T, Allocator, _AllocatorCallback, Align>;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(T)> using udList = eos::List<T, Allocator, _AllocatorCallback, Align>;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(T)> using udStack = eos::Stack<T, Allocator, _AllocatorCallback, Align>;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(T)> using udDeque = eos::Deque<T, Allocator, _AllocatorCallback, Align>;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), size Align = __alignof(T)> using udQueue = eos::Queue<T, Allocator, _AllocatorCallback, Align>;

template<typename K, typename V, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Compare = std::less<K>, size Align = __alignof(std::pair<const K, V>)> using udMap = eos::Map<K, V, Allocator, _AllocatorCallback, Compare, Align>;
template<typename K, typename V, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Compare = std::less<K>, size Align = __alignof(std::pair<const K, V>)> using udUnorderedMap = eos::UnorderedMap<K, V, Allocator, _AllocatorCallback, Compare, Align>;

template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Compare = std::less<T>, size Align = __alignof(T)> using udSet = eos::Set<T, Allocator, _AllocatorCallback, Compare, Align>;
template<typename T, typename Allocator, Allocator*(*_AllocatorCallback)(void), typename Hasher = std::hash<T>, typename KeyEquality = std::equal_to<T>, size Align = __alignof(T)> using udUnorderedSet = eos::UnorderedSet<T, Allocator, _AllocatorCallback, Hasher, KeyEquality, Align>;


//////////////////////////////////////////////////////////////////////////
using GenericAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;

extern UD_DLL GenericAllocator* GetGenericCommonAllocator();
extern UD_DLL GenericAllocator* GetGenericStringAllocator();

using udString = eos::String<GenericAllocator, GetGenericStringAllocator>;
using udWString = eos::WString<GenericAllocator, GetGenericStringAllocator>;

using udOStream = eos::OStream<GenericAllocator, GetGenericCommonAllocator>;
using udStringStream = eos::StringStream<GenericAllocator, GetGenericCommonAllocator>;
using udIStringStream = eos::IStringStream<GenericAllocator, GetGenericCommonAllocator>;

using udWOStream = eos::WOStream<GenericAllocator, GetGenericCommonAllocator>;
using udWStringStream = eos::WStringStream<GenericAllocator, GetGenericCommonAllocator>;
using udWIStringStream = eos::WIStringStream<GenericAllocator, GetGenericCommonAllocator>;

UD_NAMESPACE_END



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


#define udNewAligned(Type, Allocator, Alignment, ...)	eosNewAligned(Type, Allocator, Alignment, __VA_ARGS__)
#define udNew(Type, Allocator, ...)						udNewAligned(Type, Allocator, ION_MEMORY_ALIGNMENT_SIZE, __VA_ARGS__)
#define udDelete(Object, Allocator)						eosDelete(Object, Allocator)

#define udNewAlignedRaw(Size, Allocator, Alignment)		eosNewAlignedRaw(Size, Allocator, Alignment)
#define udNewRaw(Size, Allocator)						udNewAlignedRaw(Size, Allocator, ION_MEMORY_ALIGNMENT_SIZE)
#define udDeleteRaw(Object, Allocator)					eosDeleteRaw(Object, Allocator)

#define udNewDynamicArray(Type, Count, Allocator)		eosNewDynamicArray(Type, Count, Allocator)
#define udNewArray(Type, Allocator)						eosNewArray(Type, Allocator)
#define udDeleteArray(ObjectArray, Allocator)			eosDeleteArray((ObjectArray), (Allocator))



namespace std
{
	template<>
    struct hash<Undead::udString>
    {
        typedef Undead::udString argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& _value) const noexcept
        {
            result_type const h(std::hash<std::string>{}(_value.c_str()));
            return h;
        }
    };
}
