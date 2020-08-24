#include "RenderManager.h"

#include "Device.h"
#include "RenderPass.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "PipelineLayout.h"
#include "VertexCacheManager.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"

#include "../Texture/TextureManager.h"

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Scene/Transform.h"

#include "../Geometry/PrimitiveHelper.h"


//#include "../GPU/GpuMemoryManager.h"

//#include "../Shaders/Shader.h"

//#include "PipelineLayout.h"
//#include "RenderPass.h"
//#include "VertexDescriptor.h"


NW_NAMESPACE_BEGIN

struct PushBlock
{
	nix::Matrix4x4	m_mvp;
	float			m_roughness { 0.0f };
	uint32			m_numSamples { 32u };
} m_pushBlock;

RenderManager& RenderManager::Instance()
{
	static RenderManager instance;
	return instance;
}

RenderManager::RenderManager()
{
}


RenderManager::~RenderManager()
{
}

Texture* RenderManager::GenerateBRDF(const uint32 _dimension)
{
	return nullptr;
}

Texture* RenderManager::GeneratePrefilterDiffuse(const uint32 _dimension)
{
	return nullptr;
}

Texture* RenderManager::GeneratePrefilterGlossy(const uint32 _dimension)
{
	return nullptr;
}

void RenderManager::RenderToCube(const Device& _device, RenderPass& _renderpass, const Texture& _filteredEnv, const PipelineLayout& _pipelinelayout, const Pipeline& _pipeline, const DescriptorSet& _descSet, const uint32 _dimension, const VkFormat _format, const uint32 _numMips)
{
	// Rendering to image, then copy to cube
	FrameBuffer offscreen;

	// only one attachment here, because is for Prefiltered and BRDF
	// Where required VkImageLayout, this framebuffer is VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	FrameBufferAttachment attachment;
	attachment.Create(_device.GetDevice(), _dimension, _dimension, 1, _format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	offscreen.PushAttachment(attachment);
	offscreen.Create(_device.GetDevice(), _renderpass, _dimension, _dimension);


	_renderpass.SetFrameBuffer(offscreen);
	_renderpass.SetDimensions(static_cast<int32>(_dimension), static_cast<int32>(_dimension));
	_renderpass.PushColorDepthStencilClearValue(0.0f, 0.0f, 0.2f, 0.0f);

	const nix::Vector4 pos;	// 0
	nix::Matrix4x4 mv[6];
	mv[0] = Transform::LookAt(pos, nix::Vector4(1.0f, 0.0f, 0.0f, 0.0f), nix::Vector4(0.0f, -1.0f, 0.0f, 0.0f));  // Positive X
	mv[1] = Transform::LookAt(pos, nix::Vector4(-1.0f, 0.0f, 0.0f, 0.0f), nix::Vector4(0.0f, -1.0f, 0.0f, 0.0f));  // Negative X
	mv[2] = Transform::LookAt(pos, nix::Vector4(0.0f, -1.0f, 0.0f, 0.0f), nix::Vector4(0.0f, 0.0f, -1.0f, 0.0f));  // Positive Y
	mv[3] = Transform::LookAt(pos, nix::Vector4(0.0f, 1.0f, 0.0f, 0.0f), nix::Vector4(0.0f, 0.0f, 1.0f, 0.0f));  // Negative Y
	mv[4] = Transform::LookAt(pos, nix::Vector4(0.0f, 0.0f, 1.0f, 0.0f), nix::Vector4(0.0f, -1.0f, 0.0f, 0.0f));  // Positive Z
	mv[5] = Transform::LookAt(pos, nix::Vector4(0.0f, 0.0f, -1.0f, 0.0f), nix::Vector4(0.0f, -1.0f, 0.0f, 0.0f));  // Negative Z


	CommandPool cp;
	cp.Create(_device.GetDevice(), _device.GetQueueFamily().GetGraphicsFamily(), 0);

	CommandBuffer cmdBuf;
	cmdBuf.Create(_device.GetDevice(), cp);
	
	cmdBuf.Begin();

	cmdBuf.SetViewport(0.0f, 0.0f, static_cast<float>(_dimension), static_cast<float>(_dimension), 0.0f, 1.0f);
	cmdBuf.SetScissor(0, 0, _dimension, _dimension);
	
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.layerCount = 6;
	subresourceRange.levelCount = _numMips;

	Texture::SetImageLayout(cmdBuf, _filteredEnv.GetImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

	nix::Matrix4x4 matPers = Transform::PerspectiveProjection(NIX_DEG_TO_RAD(90.0f), 1.0f, 0.1f, 10.0f);

	// create cube and vertex/index cache
	Vertex_P vertices[8];
	Index indices[36];
	PrimitiveHelper::GenerateCube(vertices, indices);

	VertexCacheHandler vertexCache = VertexCacheManager::Instance().AllocVertex(&vertices, 8, sizeof(Vertex_P));
	VertexCacheHandler indexCache = VertexCacheManager::Instance().AllocIndex(&indices, 36, sizeof(Index));

	for (uint32 mip = 0; mip < _numMips; ++mip)
	{
		for (uint32 f = 0; f < 6; ++f)
		{
			const float width = static_cast<float>(_dimension * std::pow(0.5f, mip));
			const float height = static_cast<float>(_dimension * std::pow(0.5f, mip));

			cmdBuf.SetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);

			// Render scene from cube face's point of view
			cmdBuf.BeginRenderPass(_renderpass, offscreen);

			// Update shader push constant block
			float roughness = static_cast<float>(mip) / static_cast<float>(_numMips - 1.0f);
			m_pushBlock.m_roughness = roughness;
			m_pushBlock.m_mvp = matPers * mv[f];

			// push block vertex and fragment
			// note that it must be created the layout index 0 or will crash!
			cmdBuf.UpdatePushConstant(_pipelinelayout, 0, &m_pushBlock);

			cmdBuf.BindPipeline(_pipeline);
			cmdBuf.BindDescriptorSets(_pipelinelayout, { _descSet }, 0);

			VertexBufferObject VBO;
			IndexBufferObject IBO;

			if (VertexCacheManager::Instance().GetVertexBuffer(vertexCache, &VBO))
			{
				cmdBuf.BindVertexBuffer(VBO, 0, VBO.GetOffset());
			}

			if (VertexCacheManager::Instance().GetIndexBuffer(indexCache, &IBO))
			{
				cmdBuf.BindIndexBuffer(IBO, VK_INDEX_TYPE_UINT32, IBO.GetOffset());
			}

			cmdBuf.DrawIndexed(36, 1, 0, 0, 0);

			cmdBuf.EndRenderPass();


			Texture::SetImageLayout(cmdBuf, attachment.GetImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);


			VkImageCopy copyRegion{};

			copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.srcSubresource.baseArrayLayer = 0;
			copyRegion.srcSubresource.mipLevel = 0;
			copyRegion.srcSubresource.layerCount = 1;
			copyRegion.srcOffset = { 0, 0, 0 };

			copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.dstSubresource.baseArrayLayer = f;
			copyRegion.dstSubresource.mipLevel = mip;
			copyRegion.dstSubresource.layerCount = 1;
			copyRegion.dstOffset = { 0, 0, 0 };

			copyRegion.extent.width = static_cast<uint32>(width);
			copyRegion.extent.height = static_cast<uint32>(height);
			copyRegion.extent.depth = 1;

			vkCmdCopyImage(
				cmdBuf,
				attachment.GetImage(),
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				_filteredEnv.GetImage(),
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&copyRegion);

			Texture::SetImageLayout(cmdBuf, attachment.GetImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}
	}

	Texture::SetImageLayout(cmdBuf, _filteredEnv.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
	cmdBuf.End();

	// ?
	VertexCacheManager::Instance().SwapFrame();
}


NW_NAMESPACE_END