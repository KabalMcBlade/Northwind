#include "HashTools.h"

#include <functional>
#include <type_traits>


NW_NAMESPACE_BEGIN

namespace HashTools
{

	struct Hash8
	{
		uint8 m_seed;
		uint8 m_input;
	};

	struct Hash16
	{
		uint16 m_seed;
		uint16 m_input;
	};

	struct Hash32
	{
		uint32 m_seed;
		uint32 m_input;
	};

	struct Hash64
	{
		uint64 m_seed;
		uint64 m_input;
	};


	template<typename T> class HashGenerator;


	template<>
	class HashGenerator<Hash8>
	{
	public:
		uint8 operator()(const Hash8& _hash) const
		{
			uint8 a = static_cast<uint8>(std::hash<uint8>()(_hash.m_seed));
			uint8 b = static_cast<uint8>(std::hash<uint8>()(_hash.m_input));
			return a ^ b;
		}
	};


	template<>
	class HashGenerator<Hash16>
	{
	public:
		uint16 operator()(const Hash16& _hash) const
		{
			uint16 a = static_cast<uint16>(std::hash<uint16>()(_hash.m_seed));
			uint16 b = static_cast<uint16>(std::hash<uint16>()(_hash.m_input));
			return a ^ b;
		}
	};


	template<>
	class HashGenerator<Hash32>
	{
	public:
		uint32 operator()(const Hash32& _hash) const
		{
			uint32 a = static_cast<uint32>(std::hash<uint32>()(_hash.m_seed));
			uint32 b = static_cast<uint32>(std::hash<uint32>()(_hash.m_input));
			return a ^ b;
		}
	};


	template<>
	class HashGenerator<Hash64>
	{
	public:
		uint64 operator()(const Hash64& _hash) const
		{
			uint64 a = std::hash<uint64>()(_hash.m_seed);
			uint64 b = std::hash<uint64>()(_hash.m_input);
			return a ^ b;
		}
	};


	//////////////////////////////////////////////////////////////////////////

	uint8 MakeHash8(const void* _data, uint32 _size, uint8 _seed)
	{
		uint8 hash = _seed;
		const char* p = reinterpret_cast<const char*>(_data);

		for (uint8 i = 0; i < _size; ++i)
		{
			Hash8 ih;
			ih.m_seed = hash;
			ih.m_input = p[i];

			hash = HashGenerator<Hash8>()(ih);
		}
		return hash;
	}

	uint16 MakeHash16(const void* _data, uint32 _size, uint16 _seed)
	{
		uint16 hash = _seed;
		const char* p = reinterpret_cast<const char*>(_data);

		for (uint16 i = 0; i < _size; ++i)
		{
			Hash16 ih;
			ih.m_seed = hash;
			ih.m_input = p[i];

			hash = HashGenerator<Hash16>()(ih);
		}
		return hash;
	}

	uint32 MakeHash32(const void* _data, uint32 _size, uint32 _seed)
	{
		uint32 hash = _seed;
		const char* p = reinterpret_cast<const char*>(_data);

		for (uint32 i = 0; i < _size; ++i)
		{
			Hash32 ih;
			ih.m_seed = hash;
			ih.m_input = p[i];

			hash = HashGenerator<Hash32>()(ih);
		}
		return hash;
	}

	uint64 MakeHash64(const void* _data, uint32 _size, uint64 _seed)
	{
		uint64 hash = _seed;
		const char* p = reinterpret_cast<const char*>(_data);

		for (uint64 i = 0; i < _size; ++i)
		{
			Hash64 ih;
			ih.m_seed = hash;
			ih.m_input = p[i];

			hash = HashGenerator<Hash64>()(ih);
		}
		return hash;
	}
}

NW_NAMESPACE_END