#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "BaseBufferObject.h"

#include "../../Core/BasicDefines.h"
#include "../../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class StorageBufferObject : public BaseBufferObject
{
public:
    StorageBufferObject();
    ~StorageBufferObject();

	bool Alloc(const VkDevice& _device, const void* _data, size _allocSize, EBufferUsage _usage);
    void Free();

    void ReferenceTo(const StorageBufferObject& _other);
    void ReferenceTo(const StorageBufferObject& _other, size _refOffset, size _refSize);

    void Update(const void* _data, size _size, size _offset = 0) const;

    void* MapBuffer(EBufferMappingType _mapType);
    void* MapBuffer(EBufferMappingType _mapType, size _offset);
    void UnmapBuffer();

private:
    void ClearWithoutFreeing();
};

NW_NAMESPACE_END