#include "CommandBuffer.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"
#include "../GPU/GpuMemoryManager.h"

#include "CommandPool.h"
#include "RenderPass.h"
#include "PipelineLayout.h"
#include "Pipeline.h"

#include "Buffers/BaseBufferObject.h"
#include "Buffers/VertexBufferObject.h"
#include "Buffers/IndexBufferObject.h"


NW_NAMESPACE_BEGIN

CommandBuffer::CommandBufferAllocator* CommandBuffer::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kCommandBufferAllocatorSize);
	static CommandBufferAllocator memoryAllocator(memoryArea, "CommandBufferFreeListAllocator");

	return &memoryAllocator;
}

CommandBuffer::CommandBuffer()
	: m_device(VK_NULL_HANDLE)
	, m_pool(nullptr)
	, m_commandBuffer(VK_NULL_HANDLE)
{

}

CommandBuffer::~CommandBuffer()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool CommandBuffer::Create(const VkDevice& _device, const CommandPool& _pool)
{
	m_device = _device;
	m_pool = &_pool;

	return true;
}

void CommandBuffer::Destroy()
{
	if (m_commandBuffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(m_device, *m_pool, 1, &m_commandBuffer);
	}
}

bool CommandBuffer::Generate(VkCommandBufferLevel _level /*= VK_COMMAND_BUFFER_LEVEL_PRIMARY*/)
{
	VkCommandBufferAllocateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	createInfo.commandPool = *m_pool;
	createInfo.level = _level;
	createInfo.commandBufferCount = 1;

	VkResult result = vkAllocateCommandBuffers(m_device, &createInfo, &m_commandBuffer);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create CommandBuffer");

	return m_commandBuffer != VK_NULL_HANDLE;
}

bool CommandBuffer::Begin()
{
	VkCommandBufferBeginInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkResult result = vkBeginCommandBuffer(m_commandBuffer, &createInfo);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot begin the CommandBuffer");

	return true;
}

bool CommandBuffer::End()
{
	VkResult result = vkEndCommandBuffer(m_commandBuffer);
	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot end the CommandBuffer");

	return true;
}

void CommandBuffer::SetViewport(float _x, float _y, float _width, float _height, float _minDepth, float _maxDepth)
{
	VkViewport viewport = { _x, _y, _width, _height, _minDepth, _maxDepth };
	vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
}

void CommandBuffer::SetScissor(int32 _offsetx, int32 _offsety, uint32 _width, uint32 _height)
{
	VkRect2D scissor = { _offsetx, _offsety, _width, _height };
	vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

void CommandBuffer::BeginRenderPass(RenderPass& _renderPass, VkFramebuffer _frameBuffer)
{
	_renderPass.SetFrameBuffer(_frameBuffer);
	VkRenderPassBeginInfo beginInfo = _renderPass.GetBeginInfo();

	vkCmdBeginRenderPass(m_commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::EndRenderPass()
{
	vkCmdEndRenderPass(m_commandBuffer);
}

void CommandBuffer::BindDescriptorSets(PipelineLayout _layout, eos::Vector<DescriptorSet, CommandBufferAllocator, GetAllocator> _sets, uint32 firstSet /*= 0*/)
{
	std::vector<VkDescriptorSet> descSets;
	for (auto set : _sets)
	{
		descSets.push_back(set);
	}
	vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _layout, firstSet, static_cast<uint32>(descSets.size()), descSets.data(), 0, nullptr);
}

void CommandBuffer::BindPipeline(const Pipeline& _pipeline)
{
	vkCmdBindPipeline(m_commandBuffer, _pipeline, _pipeline);
}

void CommandBuffer::BindVertexBuffer(const VertexBufferObject& _buffer, uint32 _binding, VkDeviceSize _offset /*= 0*/)
{
	vkCmdBindVertexBuffers(m_commandBuffer, _binding, 1, _buffer.GetObjectPtr(), &_offset);
}

void CommandBuffer::BindIndexBuffer(const IndexBufferObject& _buffer, VkIndexType _indexType, VkDeviceSize _offset /*= 0*/)
{
	vkCmdBindIndexBuffer(m_commandBuffer, _buffer.GetObject(), _offset, _indexType);
}

void CommandBuffer::Draw(uint32 _vertexCount, uint32 _instanceCount, uint32 _firstVertex, uint32 _firstInstance)
{
	vkCmdDraw(m_commandBuffer, _vertexCount, _instanceCount, _firstVertex, _firstInstance);
}

void CommandBuffer::DrawIndexed(uint32 _indexCount, uint32 _instanceCount, uint32 _firstIndex, uint32 _vertexOffset, uint32 _firstInstance)
{
	vkCmdDrawIndexed(m_commandBuffer, _indexCount, _instanceCount, _firstIndex, _vertexOffset, _firstInstance);
}

void CommandBuffer::UpdatePushConstant(const PipelineLayout& _layout, uint32 _index, const void* _values)
{
	VkPushConstantRange pushConstantRange = _layout.GetPushConstant(_index);
	vkCmdPushConstants(m_commandBuffer, _layout, pushConstantRange.stageFlags, pushConstantRange.offset, pushConstantRange.size, _values);
}

NW_NAMESPACE_END