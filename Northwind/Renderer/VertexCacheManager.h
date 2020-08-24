#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "VertexLayout.h"
#include "RenderConfig.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "Buffers/VertexBufferObject.h"
#include "Buffers/IndexBufferObject.h"


NW_NAMESPACE_BEGIN


typedef uint64 NW_MEMORY_ALIGNED VertexCacheHandler;


enum ECacheType
{
	ECacheType_Vertex = 0,
	ECacheType_Index
};


struct GeometryBuffer final
{
private:
	typedef std::atomic_size_t   AtomicSize;

public:
	VertexBufferObject	m_vertexBuffer;
	IndexBufferObject	m_indexBuffer;
	uint8*              m_mappedVertexBase;
	uint8*              m_mappedIndexBase;
	AtomicSize          m_vertexMemUsed;
	AtomicSize          m_indexMemUsed;
	int32				m_allocations;    // index and vertex allocations count combined
};


class NW_DLL VertexCacheManager final : public eos::NoCopyableMoveable
{
public:
	static VertexCacheManager& Instance();

	bool Create(const VkDevice& _device, VkDeviceSize _uniformBufferOffsetAlignment);
	void Destroy();

	VertexCacheHandler AllocVertex(const void* _data, size _num, size _size = sizeof(Vertex_F));
	VertexCacheHandler AllocIndex(const void* _data, size _num, size _size = sizeof(Index));

	bool GetVertexBuffer(VertexCacheHandler _handler, VertexBufferObject* _vb);
	bool GetIndexBuffer(VertexCacheHandler _handler, IndexBufferObject* _ib);

	// to call at the end when swap the command buffer, to release the GPU of the current frame
	void SwapFrame();

private:
	VertexCacheManager();
	~VertexCacheManager();

	void Alloc(GeometryBuffer& _buffer, size _vertexBytes, size _indexBytes, EBufferUsage _usage);
	VertexCacheHandler Alloc(GeometryBuffer& _buffer, const void* _data, size _bytes, ECacheType _type);

	void ClearGeometryBuffer(GeometryBuffer& _buffer);
	void MapGeometryBuffer(GeometryBuffer& _buffer);
	void UnmapGeometryBuffer(GeometryBuffer& _buffer);

private:
	GeometryBuffer m_frameData[kNumFramesData];

	VkDevice m_device;
	VkDeviceSize m_uniformBufferOffsetAlignment;

	int32 m_currentFrame;	// for determining the active buffers
	int32 m_listNum;		// m_currentFrame % kNumFramesData
	int32 m_drawListNum;	// (m_currentFrame - 1) % kNumFramesData

	size m_mostUsedVertex;
	size m_mostUsedIndex;
};

NW_NAMESPACE_END
