#include "UniformBufferObject.h"

#include "../../Core/Assertions.h"
#include "../../GPU/GpuMemoryManager.h"
#include "../StagingBufferManager.h"


NW_NAMESPACE_BEGIN

UniformBufferObject::UniformBufferObject()
{
    //m_usage = EBufferUsage_Dynamic;
    SetUnmapped();
}

UniformBufferObject::~UniformBufferObject()
{
    Free(); //  need it in this case ?
}

bool UniformBufferObject::Alloc(const VkDevice& _device, const void* _data, size _allocSize, EBufferUsage _usage)
{
    nwAssertReturnValue(m_object == VK_NULL_HANDLE, false, "Buffer already allocated");
    nwAssertReturnValue(NW_IS_ALIGNED(_data, NW_MEMORY_ALIGNMENT_SIZE), false, "Buffer not aligned to 16");
    nwAssertReturnValue(_allocSize > 0, false, "Size must be greeter than 0");

    m_size = _allocSize;
    m_usage = _usage;
    m_device = _device;

    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.size = GetAllocedSize();
    createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (m_usage == EBufferUsage_Static)
    {
        createInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    VkResult result = vkCreateBuffer(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_object);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot allocate buffer");

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_device, m_object, &memoryRequirements);

    {
		GpuMemoryCreateInfo createInfo = {};
        createInfo.m_size = memoryRequirements.size;
        createInfo.m_align = memoryRequirements.alignment;
        createInfo.m_memoryTypeBits = memoryRequirements.memoryTypeBits;
        createInfo.m_usage = (m_usage == EBufferUsage_Static) ? EMemoryUsage_GPU : EMemoryUsage_CPU_to_GPU;
        createInfo.m_type = EGpuMemoryType_Buffer;

        m_allocation = GpuMemoryManager::Instance().GetGPU()->Alloc(createInfo);
        nwAssertReturnValue(m_allocation.m_result == VK_SUCCESS, false, "Cannot Allocate memory!");

        result = vkBindBufferMemory(m_device, m_object, m_allocation.m_memory, m_allocation.m_offset);
        nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot bind the image memory!");
    }

    if (_data != nullptr)
    {
        Update(_data, _allocSize);
    }

    return true;
}

void UniformBufferObject::Free()
{
    if (IsMapped())
    {
        UnmapBuffer();
    }

    if (!OwnsBuffer())
    {
        ClearWithoutFreeing();
        return;
    }

    if (m_object == VK_NULL_HANDLE)
    {
        return;
    }

    if (m_object != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(m_device, m_object, GpuMemoryManager::Instance().GetVK());
		GpuMemoryManager::Instance().GetGPU()->Free(m_allocation);

        m_object = VK_NULL_HANDLE;
        m_allocation = GpuMemoryAllocation();
    }

    ClearWithoutFreeing();
}

void UniformBufferObject::ReferenceTo(const UniformBufferObject& _other)
{
    nwAssertReturnVoid(!IsMapped(), "Buffer must be not mapped");
    nwAssertReturnVoid(_other.GetSize() > 0, "The other buffer must greater than 0");

    Free();
    m_size = _other.GetSize();
    m_offsetInOtherBuffer = _other.GetOffset();
    m_device = _other.m_device;
    m_usage = _other.m_usage;
    m_object = _other.m_object;
    m_allocation = _other.m_allocation;

    nwAssertReturnVoid(!OwnsBuffer(), "Should not own this buffer now!");
}

void UniformBufferObject::ReferenceTo(const UniformBufferObject& _other, size _refOffset, size _refSize)
{
    nwAssertReturnVoid(!IsMapped(), "Buffer must be not mapped");
    nwAssertReturnVoid(_refOffset >= 0, "Offset must be greater or equal to 0");
    nwAssertReturnVoid(_refSize >= 0, "Size must be greater or equal to 0");
    nwAssertReturnVoid(_refOffset + _refSize <= _other.GetSize(), "Size plus offset must be less or equal to other size");

    Free();
    m_size = _refSize;
    m_offsetInOtherBuffer = _other.GetOffset() + _refOffset;
    m_device = _other.m_device;
    m_usage = _other.m_usage;
    m_object = _other.m_object;
    m_allocation = _other.m_allocation;

    nwAssertReturnVoid(!OwnsBuffer(), "Should not own this buffer now!");
}

void UniformBufferObject::Update(const void* _data, size _size, size _offset /*= 0*/) const
{
    nwAssertReturnVoid(m_object != VK_NULL_HANDLE, "Buffer must be allocated");
    nwAssertReturnVoid(NW_IS_ALIGNED(_data, NW_MEMORY_ALIGNMENT_SIZE), "Buffer not aligned to 16");
    nwAssertReturnVoid((GetOffset() & 15) == 0, "Offset not aligned to 16");
    nwAssertReturnVoid(_size < GetSize(), "Size must be less of the total size than 0");

    if (m_usage == EBufferUsage_Dynamic)
    {
        nwAssertReturnVoid(NW_IS_ALIGNED(m_allocation.m_mappedData + GetOffset() + _offset, NW_MEMORY_ALIGNMENT_SIZE), "Buffer not aligned to 16");
        nwAssertReturnVoid(NW_IS_ALIGNED((const uint8*)_data, NW_MEMORY_ALIGNMENT_SIZE), "Buffer not aligned to 16");
		eos::MemUtils::MemCpy(m_allocation.m_mappedData + GetOffset() + _offset, (const uint8*)_data, _size);
    }
    else
    {
        VkBuffer stageBuffer;
        VkCommandBuffer commandBuffer;
        size stageOffset = 0;
        uint8* stageData = StagingBufferManager::Instance().Stage(_size, NW_MEMORY_ALIGNMENT_SIZE, commandBuffer, stageBuffer, stageOffset);

		eos::MemUtils::MemCpy(stageData, (const uint8*)_data, _size);

        VkBufferCopy bufferCopy = {};
        bufferCopy.srcOffset = stageOffset;
        bufferCopy.dstOffset = GetOffset() + _offset;
        bufferCopy.size = _size;

        vkCmdCopyBuffer(commandBuffer, stageBuffer, m_object, 1, &bufferCopy);
    }
}

void* UniformBufferObject::MapBuffer(EBufferMappingType _mapType)
{
    return MapBuffer(_mapType, 0);
}

void* UniformBufferObject::MapBuffer(EBufferMappingType _mapType, size _offset)
{
    nwAssertReturnValue(m_object != VK_NULL_HANDLE, nullptr, "Buffer must be allocated");
    nwAssertReturnValue(m_usage != EBufferUsage_Static, nullptr, "Cannot map static buffer");

    void* buffer = m_allocation.m_mappedData + GetOffset() + _offset;

    SetMapped();

    nwAssertReturnValue(buffer != nullptr, nullptr, "Failed to map the buffer");

    return buffer;
}

void UniformBufferObject::UnmapBuffer()
{
    nwAssertReturnVoid(m_object != VK_NULL_HANDLE, "Buffer must be allocated");
    nwAssertReturnVoid(m_usage != EBufferUsage_Static, "Cannot unmap static buffer");

    SetUnmapped();
}

void UniformBufferObject::ClearWithoutFreeing()
{
    m_size = 0;
    m_offsetInOtherBuffer = NW_BUFFER_OBJECT_MAPPED_FLAG;
    m_object = VK_NULL_HANDLE;
    m_allocation.m_memory = VK_NULL_HANDLE;
}

NW_NAMESPACE_END
