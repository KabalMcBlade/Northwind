#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "DescriptorSet.h"


NW_NAMESPACE_BEGIN

class Device;
class Pipeline;
class PipelineLayout;
class RenderPass;
class CommandPool;
class VertexBufferObject;
class IndexBufferObject;
class CommandBuffer final
{
public:
	using CommandBufferAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static CommandBufferAllocator* GetAllocator();

public:
	CommandBuffer();
	~CommandBuffer();

	bool Create(const VkDevice& _device, const CommandPool& _pool, VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	void Destroy();

	NW_INLINE operator const VkCommandBuffer& () const
	{
		return m_commandBuffer;
	}

	NW_INLINE operator const VkCommandBuffer* () const
	{
		return &m_commandBuffer;
	}

	bool Begin();
	bool End();
	void BeginRenderPass(RenderPass& _renderPass, VkFramebuffer _frameBuffer);
	void EndRenderPass();
	void SetViewport(float _x, float _y, float _width, float _height, float _minDepth, float _maxDepth);
	void SetScissor(int32 _offsetx, int32 _offsety, uint32 _width, uint32 _height);
	void BindDescriptorSets(PipelineLayout _layout, eos::Vector<DescriptorSet, CommandBufferAllocator, GetAllocator> _sets, uint32 firstSet = 0);
	void BindPipeline(const Pipeline& _pipeline);
	void BindVertexBuffer(const VertexBufferObject& _buffer, uint32 _binding, VkDeviceSize _offset = 0);
	void BindIndexBuffer(const IndexBufferObject& _buffer, VkIndexType _indexType, VkDeviceSize _offset = 0);
	void Draw(uint32 _vertexCount, uint32 _instanceCount, uint32 _firstVertex, uint32 _firstInstance);
	void DrawIndexed(uint32 _indexCount, uint32 _instanceCount, uint32 _firstIndex, uint32 vertexOffset, uint32 _firstInstance);
	void UpdatePushConstant(const PipelineLayout& _layout, uint32 _index, const void* _values);

private:
	VkDevice m_device;
	const CommandPool* m_pool;

	VkCommandBuffer m_commandBuffer;
};

NW_NAMESPACE_END