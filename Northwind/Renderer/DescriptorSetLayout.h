#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

enum EBindingType
{
	EBindingType_Uniform,
	EBindingType_Sampler,
	EBindingType_Storage
};

enum EDescriptorStage
{
	EDescriptorStage_Vertex,
	EDescriptorStage_TessellationControl,
	EDescriptorStage_TessellationEvaluation,
	EDescriptorStage_Geometry,
	EDescriptorStage_Fragment,
	EDescriptorStage_Compute
};

class DescriptorSetLayout final
{
public:
	DescriptorSetLayout();
	~DescriptorSetLayout();

	bool Create(const VkDevice& _device);
	void Destroy();

	NW_INLINE operator const VkDescriptorSetLayout& () const
	{
		return m_descriptorSetLayout;
	}

	NW_INLINE operator const VkDescriptorSetLayout* () const
	{
		return &m_descriptorSetLayout;
	}

	void Push(EDescriptorStage _stage, EBindingType _type, uint32 _index);

private:
	static VkShaderStageFlags ConvertDescriptorStage(EDescriptorStage _stage);
	static VkDescriptorType ConvertBindingType(EBindingType _type);

private:
	using DescriptorSetLayoutAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static DescriptorSetLayoutAllocator* GetAllocator();

	VkDevice m_device;
	VkDescriptorSetLayout m_descriptorSetLayout;
	eos::Vector<VkDescriptorSetLayoutBinding, DescriptorSetLayoutAllocator, GetAllocator> m_bindings;
};

NW_NAMESPACE_END