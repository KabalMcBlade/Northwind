#include "VertexCacheManager.h"

#include "../Core/SettingsDefines.h"
#include "../core/Assertions.h"


#define NW_VERTEX_CACHE_ALIGN	32	// 16
#define NW_INDEX_CACHE_ALIGN	16

#define NW_CACHE_SIZE_SHIFT		1
#define NW_CACHE_SIZE_MASK		0x7fffff		// 8 mb 
#define NW_CACHE_OFFSET_SHIFT   24
#define NW_CACHE_OFFSET_MASK	0x1ffffff		// 32 mb 
#define NW_CACHE_FRAME_SHIFT	49
#define NW_CACHE_FRAME_MASK		0x7fff			// 15 bits = 32k frames to wrap around


NW_NAMESPACE_BEGIN

VertexCacheManager::VertexCacheManager()
{
}

VertexCacheManager::~VertexCacheManager()
{
}

VertexCacheManager& VertexCacheManager::Instance()
{
	static VertexCacheManager instance;
	return instance;
}


bool VertexCacheManager::Create(const VkDevice& _device, VkDeviceSize _uniformBufferOffsetAlignment)
{
	m_device = _device;

	m_currentFrame = 0;
	m_listNum = 0;

	m_uniformBufferOffsetAlignment = _uniformBufferOffsetAlignment;

	m_mostUsedVertex = 0;
	m_mostUsedIndex = 0;

	for (uint32 i = 0; i < kNumFramesData; ++i)
	{
		Alloc(m_frameData[i], SettingsDefines::Engine::kRenderVertexMemorySizePerFrame, SettingsDefines::Engine::kRenderIndexMemorySizePerFrame, EBufferUsage_Dynamic);
	}

	MapGeometryBuffer(m_frameData[m_listNum]);

	return true;
}

void VertexCacheManager::Destroy()
{
	for (uint32 i = 0; i < kNumFramesData; ++i)
	{
		m_frameData[i].m_vertexBuffer.Free();
		m_frameData[i].m_indexBuffer.Free();
	}
}


void VertexCacheManager::MapGeometryBuffer(GeometryBuffer& _buffer)
{
	if (_buffer.m_mappedVertexBase == nullptr)
	{
		_buffer.m_mappedVertexBase = (uint8*)_buffer.m_vertexBuffer.MapBuffer(EBufferMappingType_Write);
	}
	if (_buffer.m_mappedIndexBase == nullptr)
	{
		_buffer.m_mappedIndexBase = (uint8*)_buffer.m_indexBuffer.MapBuffer(EBufferMappingType_Write);
	}
}

void VertexCacheManager::UnmapGeometryBuffer(GeometryBuffer& _buffer)
{
	if (_buffer.m_mappedVertexBase != nullptr)
	{
		_buffer.m_vertexBuffer.UnmapBuffer();
		_buffer.m_mappedVertexBase = nullptr;
	}
	if (_buffer.m_mappedIndexBase != nullptr)
	{
		_buffer.m_indexBuffer.UnmapBuffer();
		_buffer.m_mappedIndexBase = nullptr;
	}
}

void VertexCacheManager::Alloc(GeometryBuffer& _buffer, size _vertexBytes, size _indexBytes, EBufferUsage _usage)
{
	_buffer.m_vertexBuffer.Alloc(m_device, nullptr, _vertexBytes, _usage);
	_buffer.m_indexBuffer.Alloc(m_device, nullptr, _indexBytes, _usage);

	ClearGeometryBuffer(_buffer);
}

VertexCacheHandler VertexCacheManager::Alloc(GeometryBuffer& _buffer, const void* _data, size _bytes, ECacheType _type)
{
	if (_bytes == 0)
	{
		return (VertexCacheHandler)0;
	}

	nwAssertReturnValue((((uintptr_t)(_data)) & 15) == 0, (VertexCacheHandler)0, "Pointer is misaligned");
	nwAssertReturnValue((_bytes & 15) == 0, (VertexCacheHandler)0, "size is misaligned");

	size    endPos = 0;
	size offset = 0;

	switch (_type)
	{
	case ECacheType_Index:
	{
		_buffer.m_indexMemUsed.fetch_add(_bytes, std::memory_order_relaxed);
		endPos = _buffer.m_indexMemUsed.load();
		if (endPos > _buffer.m_indexBuffer.GetAllocedSize())
		{
			nwAssertReturnValue(false, (VertexCacheHandler)0, "Out of index cache");
		}

		offset = endPos - _bytes;

		if (_data != nullptr)
		{
			if (_buffer.m_indexBuffer.GetUsage() == EBufferUsage_Dynamic)
			{
				MapGeometryBuffer(_buffer);
			}
			_buffer.m_indexBuffer.Update(_data, _bytes, offset);
		}

		break;
	}
	case ECacheType_Vertex:
	{
		_buffer.m_vertexMemUsed.fetch_add(_bytes, std::memory_order_relaxed);
		endPos = _buffer.m_vertexMemUsed.load();
		if (endPos > _buffer.m_vertexBuffer.GetAllocedSize())
		{
			nwAssertReturnValue(false, (VertexCacheHandler)0, "Out of vertex cache");
		}

		offset = endPos - _bytes;

		if (_data != nullptr)
		{
			if (_buffer.m_vertexBuffer.GetUsage() == EBufferUsage_Dynamic)
			{
				MapGeometryBuffer(_buffer);
			}
			_buffer.m_vertexBuffer.Update(_data, _bytes, offset);
		}

		break;
	}
	default:
		nwAssert(false, "Cache type invalid!");
	}

	++_buffer.m_allocations;

	/*
	VertexCacheHandler handler = ((ionU64)(offset & ION_VERTCACHE_OFFSET_MASK) << ION_VERTCACHE_OFFSET_SHIFT) | ((ionU64)(_bytes & ION_VERTCACHE_SIZE_MASK) << ION_VERTCACHE_SIZE_SHIFT);
	if (&_buffer == &m_staticData)
	{
		handler |= ION_VERTCACHE_STATIC;
	}*/
	VertexCacheHandler handler =
		((uint64)(m_currentFrame & NW_CACHE_FRAME_MASK) << NW_CACHE_FRAME_SHIFT) |
		((uint64)(offset & NW_CACHE_OFFSET_MASK) << NW_CACHE_OFFSET_SHIFT) |
		((uint64)(_bytes & NW_CACHE_SIZE_MASK) << NW_CACHE_SIZE_SHIFT);

	return handler;
}


VertexCacheHandler VertexCacheManager::AllocVertex(const void* _data, size _num, size _size /*= sizeof(Vertex_F)*/)
{
	size uiMask = NW_VERTEX_CACHE_ALIGN - 1;
	size uiSize = ((_num * _size) + uiMask) & ~uiMask;

	return Alloc(m_frameData[m_listNum], _data, uiSize, ECacheType_Vertex);
}

VertexCacheHandler VertexCacheManager::AllocIndex(const void* _data, size _num, size _size /*= sizeof(Index)*/)
{
	size uiMask = NW_INDEX_CACHE_ALIGN - 1;
	size uiSize = ((_num * _size) + uiMask) & ~uiMask;

	return Alloc(m_frameData[m_listNum], _data, uiSize, ECacheType_Index);
}

bool VertexCacheManager::GetVertexBuffer(VertexCacheHandler _handler, VertexBufferObject* _vb)
{
	if (_handler == 0)
	{
		return false;
	}

	const uint64 size = (int32)(_handler >> NW_CACHE_SIZE_SHIFT) & NW_CACHE_SIZE_MASK;
	const uint64 offset = (int32)(_handler >> NW_CACHE_OFFSET_SHIFT) & NW_CACHE_OFFSET_MASK;
	const uint64 frameNum = (int32)(_handler >> NW_CACHE_FRAME_SHIFT) & NW_CACHE_FRAME_MASK;

	if (frameNum != ((m_currentFrame - 1) & NW_CACHE_FRAME_MASK)) {
		return false;
	}

	_vb->ReferenceTo(m_frameData[m_drawListNum].m_vertexBuffer, offset, size);
	return true;
}

bool VertexCacheManager::GetIndexBuffer(VertexCacheHandler _handler, IndexBufferObject* _ib)
{
	if (_handler == 0)
	{
		return false;
	}

	const uint64 size = (int32)(_handler >> NW_CACHE_SIZE_SHIFT) & NW_CACHE_SIZE_MASK;
	const uint64 offset = (int32)(_handler >> NW_CACHE_OFFSET_SHIFT) & NW_CACHE_OFFSET_MASK;
	const uint64 frameNum = (int32)(_handler >> NW_CACHE_FRAME_SHIFT) & NW_CACHE_FRAME_MASK;

	if (frameNum != ((m_currentFrame - 1) & NW_CACHE_FRAME_MASK)) {
		return false;
	}

	_ib->ReferenceTo(m_frameData[m_drawListNum].m_indexBuffer, offset, size);
	return true;
}

void VertexCacheManager::ClearGeometryBuffer(GeometryBuffer& _buffer)
{
	_buffer.m_indexMemUsed = 0;
	_buffer.m_vertexMemUsed = 0;
	_buffer.m_allocations = 0;
}

void VertexCacheManager::SwapFrame()
{
	m_mostUsedVertex = std::max(m_mostUsedVertex, m_frameData[m_listNum].m_vertexMemUsed.load());
	m_mostUsedIndex = std::max(m_mostUsedIndex, m_frameData[m_listNum].m_indexMemUsed.load());

	// unmap the current frame so the GPU can read it
	UnmapGeometryBuffer(m_frameData[m_listNum]);
	m_drawListNum = m_listNum;

	// prepare the next frame for writing to by the CPU
	++m_currentFrame;

	m_listNum = m_currentFrame % kNumFramesData;

	MapGeometryBuffer(m_frameData[m_listNum]);

	ClearGeometryBuffer(m_frameData[m_listNum]);
}

NW_NAMESPACE_END