
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "VertexLayout.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class VertexDescriptor final
{
public:
	VertexDescriptor();
	~VertexDescriptor();

	bool Create(EVertexLayout _layout);
	void Destroy();

	NW_INLINE operator const VkPipelineVertexInputStateCreateInfo& () const
	{
		return m_vertexInput;
	}

private:
	using VertexDescriptorAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static VertexDescriptorAllocator* GetAllocator();

	VkPipelineVertexInputStateCreateInfo m_vertexInput;
	eos::Vector<VkVertexInputBindingDescription, VertexDescriptorAllocator, VertexDescriptor::GetAllocator> m_bindingDescriptions;
	eos::Vector<VkVertexInputAttributeDescription, VertexDescriptorAllocator, VertexDescriptor::GetAllocator> m_attributeDescriptions;
};

NW_NAMESPACE_END