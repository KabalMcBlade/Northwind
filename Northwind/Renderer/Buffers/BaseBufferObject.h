#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../../Core/BasicDefines.h"
#include "../../Core/BasicTypes.h"

#include "../../GPU/GpuDataStructure.h"


NW_NAMESPACE_BEGIN

#define NW_BUFFER_OBJECT_MAPPED_FLAG   0x80000000	//(1 << ( 4 * 8 - 1 ))        //  0x80000000      // 0b10000000000000000000000000000000     
// ~0x80000000  =   0x7FFFFFFF      // 0b0111 1111 1111 1111 1111 1111 1111 1111
// ~15          =   0xFFFFFFF0      // 0b1111 1111 1111 1111 1111 1111 1111 0000        

enum EBufferMappingType 
{
    EBufferMappingType_Read,
    EBufferMappingType_Write
};

enum EBufferUsage
{
    EBufferUsage_Static,	// GPU Read
    EBufferUsage_Dynamic,	// GPU Read, CPU Read/Write
};

class BaseBufferObject 
{
public:
	BaseBufferObject() : m_size(0), m_offsetInOtherBuffer(NW_BUFFER_OBJECT_MAPPED_FLAG), m_usage(EBufferUsage_Static), m_object(VK_NULL_HANDLE), m_device(VK_NULL_HANDLE) {}
	~BaseBufferObject() {}

    NW_INLINE size GetSize() const { return (m_size & ~NW_BUFFER_OBJECT_MAPPED_FLAG); }
	NW_INLINE size GetAllocedSize() const { return ((m_size & ~NW_BUFFER_OBJECT_MAPPED_FLAG) + 15) & ~15; }
	NW_INLINE EBufferUsage GetUsage() const { return m_usage; }
	NW_INLINE VkBuffer GetObject() const { return m_object; }
	NW_INLINE const VkBuffer* GetObjectPtr() const { return &m_object; }
	NW_INLINE size GetOffset() const { return (m_offsetInOtherBuffer & ~NW_BUFFER_OBJECT_MAPPED_FLAG); }

	NW_INLINE bool IsMapped() const { return (m_size & NW_BUFFER_OBJECT_MAPPED_FLAG) != 0; }

protected:
	NW_INLINE void SetMapped() const { const_cast<size&>(m_size) |= NW_BUFFER_OBJECT_MAPPED_FLAG; }
	NW_INLINE void SetUnmapped() const { const_cast<size&>(m_size) &= ~NW_BUFFER_OBJECT_MAPPED_FLAG; }
	NW_INLINE bool OwnsBuffer() const { return ((m_offsetInOtherBuffer & NW_BUFFER_OBJECT_MAPPED_FLAG) != 0); }

protected:
	size m_size;
	size m_offsetInOtherBuffer;
    EBufferUsage m_usage;
    VkDevice m_device;
    VkBuffer m_object;
    GpuMemoryAllocation m_allocation;
};

NW_NAMESPACE_END