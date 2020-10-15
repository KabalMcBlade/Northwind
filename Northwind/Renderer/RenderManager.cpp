#include "RenderManager.h"

#include "Device.h"
#include "RenderPass.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "PipelineLayout.h"
#include "VertexCacheManager.h"
#include "Pipeline.h"
#include "DescriptorSetLayout.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "VertexDescriptor.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"

#include "../Texture/TextureManager.h"

#include "../Shaders/Shader.h"
#include "../Shaders/ShaderManager.h"

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

Texture* RenderManager::GenerateBRDF(const Device& _device, const nwString& _name, const Shader& _brdfVert, const Shader& _brdfFrag, const uint32 _dimension)
{
	Texture* brdflut = TextureManager::Instance().Generate2D(_device, _name, _dimension, _dimension, false, VK_FORMAT_R16G16_SFLOAT, 
		VK_FILTER_LINEAR, VK_FILTER_LINEAR,
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);


	// RenderPass

	RenderPass renderPass;

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = VK_FORMAT_R16G16_SFLOAT;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference colorRef[1];
	colorRef[0].attachment = 0;
	colorRef[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	renderPass.PushAttachmentDescription(colorAttachment);


	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef[0];
	subpass.pDepthStencilAttachment = VK_NULL_HANDLE;

	renderPass.PushSubpassDescription(subpass);


	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPass.PushSubpassDependency(dependency);

	renderPass.SetDimensions(static_cast<int32>(_dimension), static_cast<int32>(_dimension));
	renderPass.PushColorDepthStencilClearValue(0.0f, 0.0f, 0.0f, 1.0f);

	renderPass.Create(_device);


	// Descriptors

	DescriptorSetLayout descriptorSetLayout;
	descriptorSetLayout.Push(EDescriptorStage_Fragment, EBindingType_Sampler, 0);
	descriptorSetLayout.Create(_device.GetDevice());

	DescriptorPool descriptorPool;
	descriptorPool.Push(DescriptorSetLayout::ConvertBindingType(EBindingType_Sampler), 1);
	descriptorPool.Create(_device.GetDevice(), 0, 1);

	DescriptorSet descriptorSet;
	descriptorSet.PushLayout(descriptorSetLayout);
	descriptorSet.Create(_device.GetDevice(), descriptorPool);


	// VertexDescriptor

	VertexDescriptor vertexDescriptor;
	vertexDescriptor.Create(EVertexLayout_P);


	// Pipeline

	PipelineLayout pipelinelayout;
	pipelinelayout.Create(_device.GetDevice(), descriptorSetLayout);

	Pipeline pipeline;
	pipeline.PushShader(_brdfVert);
	pipeline.PushShader(_brdfFrag);
	pipeline.SetPolygonMode(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE);
	pipeline.SetDepthStencil(VK_FALSE);
	pipeline.SetVertexInput(vertexDescriptor);
	pipeline.Create(_device.GetDevice());

	RenderToQuad(renderPass, _device, brdflut, pipelinelayout, pipeline, descriptorSet, _dimension, _dimension, VK_FORMAT_R16G16_SFLOAT, 0);

	// cleanup made by destructor

	return brdflut;
}

Texture* RenderManager::GenerateEnvironmentMap(const Device& _device, const nwString& _name, const Texture* _hdrTexture2D)
{
	return nullptr;
}

Texture* RenderManager::GeneratePrefilterDiffuse(const Device& _device, const nwString& _name, const Shader& _filtercube, const Shader& _prefilterDiffuse, const uint32 _dimension, const Texture* _hdrTexture2D, const Texture* _environmentMap)
{
	return GeneratePrefiltered(_device, _name, _filtercube, _prefilterDiffuse, _dimension, _hdrTexture2D, _environmentMap, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
}

Texture* RenderManager::GeneratePrefilterGlossy(const Device& _device, const nwString& _name, const Shader& _filtercube, const Shader& _prefilterGlossy, const uint32 _dimension, const Texture* _hdrTexture2D, const Texture* _environmentMap)
{
	return GeneratePrefiltered(_device, _name, _filtercube, _prefilterGlossy, _dimension, _hdrTexture2D, _environmentMap, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
}

Texture* RenderManager::GeneratePrefiltered(const Device& _device, const nwString& _name, const Shader& _filtercube, const Shader& _prefilterShader, const uint32 _dimension, const Texture* _hdrTexture2D, const Texture* _environmentMap, VkImageUsageFlags _usageFlags)
{
	Texture* prefilter = TextureManager::Instance().GenerateCube(_device, _name, _dimension, _dimension, true, VK_FORMAT_R16G16B16A16_SFLOAT, 
		VK_FILTER_LINEAR, VK_FILTER_LINEAR, 
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		_usageFlags, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);


	// RenderPass

	RenderPass renderPass;

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorRef[1];
	colorRef[0].attachment = 0;
	colorRef[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	renderPass.PushAttachmentDescription(colorAttachment);

	
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef[0];
	subpass.pDepthStencilAttachment = VK_NULL_HANDLE;

	renderPass.PushSubpassDescription(subpass);


	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPass.PushSubpassDependency(dependency);

	renderPass.SetDimensions(static_cast<int32>(_dimension), static_cast<int32>(_dimension));
	renderPass.PushColorDepthStencilClearValue(0.0f, 0.0f, 0.2f, 0.0f);

	renderPass.Create(_device);


	// Descriptors

	DescriptorSetLayout descriptorSetLayout;
	descriptorSetLayout.Push(EDescriptorStage_Fragment, EBindingType_Sampler, 0);
	descriptorSetLayout.Push(EDescriptorStage_Fragment, EBindingType_Sampler, 1);
	descriptorSetLayout.Create(_device.GetDevice());


	DescriptorPool descriptorPool;
	descriptorPool.Push(DescriptorSetLayout::ConvertBindingType(EBindingType_Sampler), 2);
	descriptorPool.Create(_device.GetDevice(), 0, 1);


	DescriptorSet descriptorSet;
	descriptorSet.PushLayout(descriptorSetLayout);
	descriptorSet.PushDescriptor(0, DescriptorSetLayout::ConvertBindingType(EBindingType_Sampler), _hdrTexture2D->GetDescriptor());
	descriptorSet.PushDescriptor(1, DescriptorSetLayout::ConvertBindingType(EBindingType_Sampler), _environmentMap->GetDescriptor());
	descriptorSet.Create(_device.GetDevice(), descriptorPool);


	// VertexDescriptor

	VertexDescriptor vertexDescriptor;
	vertexDescriptor.Create(EVertexLayout_P);


	// Pipeline

	PipelineLayout pipelinelayout;
	pipelinelayout.PushConstant(DescriptorSetLayout::ConvertDescriptorStage(EDescriptorStage_Vertex) | DescriptorSetLayout::ConvertDescriptorStage(EDescriptorStage_Fragment), 0, sizeof(PushBlock));
	pipelinelayout.Create(_device.GetDevice(), descriptorSetLayout);


	Pipeline pipeline;
	pipeline.PushShader(_filtercube);
	pipeline.PushShader(_prefilterShader);
	pipeline.SetPolygonMode(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE);
	pipeline.SetDepthStencil(VK_FALSE);
	pipeline.SetVertexInput(vertexDescriptor);
	pipeline.Create(_device.GetDevice());

	RenderToCube(renderPass, _device, prefilter, pipelinelayout, pipeline, descriptorSet, _dimension, _dimension, VK_FORMAT_R16G16B16A16_SFLOAT, _dimension);

	// cleanup made by destructor

	return prefilter;
}

void RenderManager::RenderToCube(RenderPass& _renderpassToUpdate, const Device& _device, const Texture* _texture, const PipelineLayout& _pipelinelayout, const Pipeline& _pipeline, const DescriptorSet& _descSet, const uint32 _width, const uint32 _height, const VkFormat _format, const uint32 _numMips)
{
	// Rendering to image, then copy to cube
	FrameBuffer offscreen;

	// only one attachment here, because is for Prefiltered and BRDF
	// Where required VkImageLayout, this framebuffer is VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	FrameBufferAttachment attachment;
	attachment.Create(_device.GetDevice(), _width, _height, 1, _format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	offscreen.PushAttachment(attachment);
	offscreen.Create(_device.GetDevice(), _renderpassToUpdate, _width, _height);

	// add offscreen frame buffer
	_renderpassToUpdate.SetFrameBuffer(offscreen);


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

	cmdBuf.SetViewport(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height), 0.0f, 1.0f);
	cmdBuf.SetScissor(0, 0, _width, _height);
	
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.layerCount = 6;
	subresourceRange.levelCount = _numMips;

	Texture::SetImageLayout(cmdBuf, _texture->GetImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

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
			const float width = static_cast<float>(_width * std::pow(0.5f, mip));
			const float height = static_cast<float>(_height * std::pow(0.5f, mip));

			cmdBuf.SetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);

			// Render scene from cube face's point of view
			cmdBuf.BeginRenderPass(_renderpassToUpdate, offscreen);

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
				_texture->GetImage(),
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&copyRegion);

			Texture::SetImageLayout(cmdBuf, attachment.GetImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}
	}

	Texture::SetImageLayout(cmdBuf, _texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
	cmdBuf.End();

	VertexCacheManager::Instance().SwapFrame();
}

void RenderManager::RenderToQuad(RenderPass& _renderpassToUpdate, const Device& _device, const Texture* _texture, const PipelineLayout& _pipelinelayout, const Pipeline& _pipeline, const DescriptorSet& _descSet, const uint32 _width, const uint32 _height, const VkFormat _format, const uint32 _numMips)
{
	// Rendering to image, then copy to cube
	FrameBuffer offscreen;

	// only one attachment here, because is for Prefiltered and BRDF
	// Where required VkImageLayout, this framebuffer is VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	FrameBufferAttachment attachment;
	attachment.Create(_device.GetDevice(), _width, _height, 1, _format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	offscreen.PushAttachment(attachment);
	offscreen.Create(_device.GetDevice(), _renderpassToUpdate, _width, _height);

	// add offscreen frame buffer
	_renderpassToUpdate.SetFrameBuffer(offscreen);

	CommandPool cp;
	cp.Create(_device.GetDevice(), _device.GetQueueFamily().GetGraphicsFamily(), 0);

	CommandBuffer cmdBuf;
	cmdBuf.Create(_device.GetDevice(), cp);

	cmdBuf.Begin();

	cmdBuf.SetViewport(0.0f, 0.0f, static_cast<float>(_width), static_cast<float>(_height), 0.0f, 1.0f);
	cmdBuf.SetScissor(0, 0, _width, _height);

	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.layerCount = 6;
	subresourceRange.levelCount = _numMips;

	Texture::SetImageLayout(cmdBuf, _texture->GetImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

	// create cube and vertex/index cache
	Vertex_P vertices[4];
	Index indices[6];
	PrimitiveHelper::GenerateQuad(vertices, indices);

	VertexCacheHandler vertexCache = VertexCacheManager::Instance().AllocVertex(&vertices, 4, sizeof(Vertex_P));
	VertexCacheHandler indexCache = VertexCacheManager::Instance().AllocIndex(&indices, 6, sizeof(Index));

	for (uint32 mip = 0; mip < _numMips; ++mip)
	{
		const float width = static_cast<float>(_width * std::pow(0.5f, mip));
		const float height = static_cast<float>(_height * std::pow(0.5f, mip));

		cmdBuf.SetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);

		// Render scene from cube face's point of view
		cmdBuf.BeginRenderPass(_renderpassToUpdate, offscreen);

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

		cmdBuf.DrawIndexed(6, 1, 0, 0, 0);

		cmdBuf.EndRenderPass();


		Texture::SetImageLayout(cmdBuf, attachment.GetImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);


		VkImageCopy copyRegion{};

		copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.srcSubresource.baseArrayLayer = 0;
		copyRegion.srcSubresource.mipLevel = 0;
		copyRegion.srcSubresource.layerCount = 1;
		copyRegion.srcOffset = { 0, 0, 0 };

		copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.dstSubresource.baseArrayLayer = 0;
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
			_texture->GetImage(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&copyRegion);

		Texture::SetImageLayout(cmdBuf, attachment.GetImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}

	Texture::SetImageLayout(cmdBuf, _texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
	cmdBuf.End();

	VertexCacheManager::Instance().SwapFrame();
}

NW_NAMESPACE_END