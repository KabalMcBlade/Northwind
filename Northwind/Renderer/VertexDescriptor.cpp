#include "VertexDescriptor.h"

#include "../Core/Assertions.h"
#include "../Core/SettingsDefines.h"


NW_NAMESPACE_BEGIN

VertexDescriptor::VertexDescriptorAllocator* VertexDescriptor::GetAllocator()
{
	static eos::HeapAreaR memoryArea(SettingsDefines::Engine::kVertexDescriptorAllocatorSize);
	static VertexDescriptorAllocator memoryAllocator(memoryArea, "VertexDescriptorAllocator");

	return &memoryAllocator;
}

VertexDescriptor::VertexDescriptor() : m_vertexInput({})
{

}

VertexDescriptor::~VertexDescriptor()
{
	// does not do anything
}

bool VertexDescriptor::Create(EVertexLayout _layout)
{
	m_vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_vertexInput.pNext = nullptr;
	m_vertexInput.flags = 0;

	VkVertexInputBindingDescription binding = {};
	VkVertexInputAttributeDescription attribute = {};

	uint32 locationIndex = 0;
	uint32 locationOffset = 0;

	switch (_layout)
	{
	case Northwind::EVertexLayout_P:
		binding.stride = sizeof(Vertex_P);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

	case Northwind::EVertexLayout_C:
		binding.stride = sizeof(Vertex_C);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_C::m_position);

		// Color
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

	case Northwind::EVertexLayout_N:
		binding.stride = sizeof(Vertex_N);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_N::m_position);

		// Normal
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_N::m_normal);

		// TexCoord0
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

	case Northwind::EVertexLayout_T:
		binding.stride = sizeof(Vertex_T);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_T::m_position);

		// Normal
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_T::m_normal);

		// TexCoord0
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_T::m_texCoord0);

		// TexCoord1
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

	case Northwind::EVertexLayout_F:
		binding.stride = sizeof(Vertex_F);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_F::m_position);

		// Normal
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_F::m_normal);

		// TexCoord0
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_F::m_texCoord0);

		// Tangent
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

	case Northwind::EVertexLayout_E:
		binding.stride = sizeof(Vertex_E);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_E::m_position);

		// Normal
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_E::m_normal);

		// TexCoord0
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_E::m_texCoord0);

		// TexCoord1
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_E::m_texCoord1);

		// Tangent
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

		//
		// SKINNED VERSION OF THE ABOVE
	case Northwind::EVertexLayout_P_S:
		binding.stride = sizeof(Vertex_P_S);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_P_S::m_position);

		// Weights
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_P_S::m_weights);

		// Joints
		attribute.format = VK_FORMAT_R16G16B16A16_UINT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

	case Northwind::EVertexLayout_C_S:
		binding.stride = sizeof(Vertex_C_S);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_C_S::m_position);

		// Color
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_C_S::m_color);

		// Weights
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_C_S::m_weights);

		// Joints
		attribute.format = VK_FORMAT_R16G16B16A16_UINT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

	case Northwind::EVertexLayout_N_S:
		binding.stride = sizeof(Vertex_N_S);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_N_S::m_position);

		// Normal
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_N_S::m_normal);

		// TexCoord0
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_N_S::m_texCoord0);

		// Weights
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_N_S::m_weights);

		// Joints
		attribute.format = VK_FORMAT_R16G16B16A16_UINT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

	case Northwind::EVertexLayout_T_S:
		binding.stride = sizeof(Vertex_T_S);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_T_S::m_position);

		// Normal
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_T_S::m_normal);

		// TexCoord0
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_T_S::m_texCoord0);

		// TexCoord1
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_T_S::m_texCoord1);

		// Weights
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_T_S::m_weights);

		// Joints
		attribute.format = VK_FORMAT_R16G16B16A16_UINT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

	case Northwind::EVertexLayout_F_S:
		binding.stride = sizeof(Vertex_F_S);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_F_S::m_position);

		// Normal
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_F_S::m_normal);

		// TexCoord0
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_F_S::m_texCoord0);

		// Tangent
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_F_S::m_tangent);

		// Weights
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_F_S::m_weights);

		// Joints
		attribute.format = VK_FORMAT_R16G16B16A16_UINT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

	case Northwind::EVertexLayout_E_S:
		binding.stride = sizeof(Vertex_E_S);
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_bindingDescriptions.push_back(binding);

		// Position
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_E_S::m_position);

		// Normal
		attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_E_S::m_normal);

		// TexCoord0
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_E_S::m_texCoord0);

		// TexCoord1
		attribute.format = VK_FORMAT_R32G32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_E_S::m_texCoord1);

		// Tangent
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_E_S::m_tangent);

		// Weights
		attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		locationOffset += sizeof(Vertex_E_S::m_weights);

		// Joints
		attribute.format = VK_FORMAT_R16G16B16A16_UINT;
		attribute.location = locationIndex++;
		attribute.offset = locationOffset;
		m_attributeDescriptions.push_back(attribute);
		break;

	default:
		nwAssertReturnValue(false, false, "The vertex layout %d does not match any valid vertex descriptor!", _layout);
		break;
	}

	m_vertexInput.vertexBindingDescriptionCount = (uint32)m_bindingDescriptions.size();
	m_vertexInput.pVertexBindingDescriptions = m_bindingDescriptions.data();
	m_vertexInput.vertexAttributeDescriptionCount = (uint32)m_attributeDescriptions.size();
	m_vertexInput.pVertexAttributeDescriptions = m_attributeDescriptions.data();

	return true;
}

void VertexDescriptor::Destroy()
{
}

NW_NAMESPACE_END