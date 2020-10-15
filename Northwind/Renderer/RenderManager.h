#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemoryWrapper.h"
#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "FrameBuffer.h"


NW_NAMESPACE_BEGIN

class Device;
class RenderPass;
class PipelineLayout;
class Pipeline;
class DescriptorSet;
class Texture;
class Shader;
class RenderManager final : public eos::NoCopyableMoveable
{
public:
	RenderManager();
	~RenderManager();

	static RenderManager& Instance();

	Texture* GenerateBRDF(const Device& _device, const nwString& _name, const Shader& _brdfVert, const Shader& _brdfFrag, const uint32 _dimension);
	Texture* GenerateEnvironmentMap(const Device& _device, const nwString& _name, const Texture* _hdrTexture2D);
	Texture* GeneratePrefilterDiffuse(const Device& _device, const nwString& _name, const Shader& _filtercube, const Shader& _prefilterDiffuse, const uint32 _dimension, const Texture* _hdrTexture2D, const Texture* _environmentMap);
	Texture* GeneratePrefilterGlossy(const Device& _device, const nwString& _name, const Shader& _filtercube, const Shader& _prefilterGlossy, const uint32 _dimension, const Texture* _hdrTexture2D, const Texture* _environmentMap);

	void RenderToCube(RenderPass& _renderpassToUpdate, const Device& _device, const Texture* _texture, const PipelineLayout& _pipelinelayout, const Pipeline& _pipeline, const DescriptorSet& _descSet, const uint32 _width, const uint32 _height, const VkFormat _format, const uint32 _numMips);
	void RenderToQuad(RenderPass& _renderpassToUpdate, const Device& _device, const Texture* _texture, const PipelineLayout& _pipelinelayout, const Pipeline& _pipeline, const DescriptorSet& _descSet, const uint32 _width, const uint32 _height, const VkFormat _format, const uint32 _numMips);

private:
	Texture* GeneratePrefiltered(const Device& _device, const nwString& _name, const Shader& _filtercube, const Shader& _prefilterShader, const uint32 _dimension, const Texture* _hdrTexture2D, const Texture* _environmentMap, VkImageUsageFlags _usageFlags);
};

NW_NAMESPACE_END
