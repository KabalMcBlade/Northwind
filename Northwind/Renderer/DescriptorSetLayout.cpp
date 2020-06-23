#include "DescriptorSetLayout.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"
#include "../GPU/GpuMemoryManager.h"


NW_NAMESPACE_BEGIN

DescriptorSetLayout::DescriptorSetLayoutAllocator* DescriptorSetLayout::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kDescriptorSetLayoutAllocatorSize);
	static DescriptorSetLayoutAllocator memoryAllocator(memoryArea, "DescriptorSetLayoutAllocator");

	return &memoryAllocator;
}

VkShaderStageFlags DescriptorSetLayout::ConvertDescriptorStage(EDescriptorStage _stage)
{
	switch (_stage)
	{
	case Northwind::EDescriptorStage_Vertex:
		return VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case Northwind::EDescriptorStage_TessellationControl:
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		break;
	case Northwind::EDescriptorStage_TessellationEvaluation:
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		break;
	case Northwind::EDescriptorStage_Geometry:
		return VK_SHADER_STAGE_GEOMETRY_BIT;
		break;
	case Northwind::EDescriptorStage_Fragment:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	case Northwind::EDescriptorStage_Compute:
		return VK_SHADER_STAGE_COMPUTE_BIT;
		break;
	default:
		nwAssertReturnValue(false, VK_SHADER_STAGE_VERTEX_BIT, "Cannot convert from EDescriptorStage to VkShaderStageFlags");
		break;
	}
}

VkDescriptorType DescriptorSetLayout::ConvertBindingType(EBindingType _type)
{
	switch (_type)
	{
	case Northwind::EBindingType_Uniform:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		break;
	case Northwind::EBindingType_Sampler:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		break;
	case Northwind::EBindingType_Storage:
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		break;
	default:
		nwAssertReturnValue(false, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, "Cannot convert from EBindingType to VkDescriptorType");
		break;
	}
}

DescriptorSetLayout::DescriptorSetLayout() : m_device(VK_NULL_HANDLE), m_descriptorSetLayout(VK_NULL_HANDLE)
{

}

DescriptorSetLayout::~DescriptorSetLayout()
{
	if (m_device != VK_NULL_HANDLE)
	{
		Destroy();
	}
}

bool DescriptorSetLayout::Create(const VkDevice& _device)
{
	m_device = _device;
	return true;
}

void DescriptorSetLayout::Destroy()
{
	if (m_descriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, GpuMemoryManager::Instance().GetVK());
		m_descriptorSetLayout = VK_NULL_HANDLE;
	}
}


void DescriptorSetLayout::Push(EDescriptorStage _stage, EBindingType _type, uint32 _index)
{
	VkDescriptorSetLayoutBinding binding = {};
	binding.descriptorCount = 1;
	binding.stageFlags = ConvertDescriptorStage(_stage);
	binding.descriptorType = ConvertBindingType(_type);
	binding.binding = _index;

	m_bindings.push_back(binding);
}

bool DescriptorSetLayout::Generate()
{
	VkDescriptorSetLayoutCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = (uint32)m_bindings.size();
	createInfo.pBindings = m_bindings.data();

	VkResult result = vkCreateDescriptorSetLayout(m_device, &createInfo, GpuMemoryManager::Instance().GetVK(), &m_descriptorSetLayout);

	nwAssertReturnValue(result == VK_SUCCESS, false, "Cannot create DescriptorSetLayout");
	return m_descriptorSetLayout != VK_NULL_HANDLE;
}

NW_NAMESPACE_END