#pragma once

#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

#define NW_MEMORY_1_KB			(1u << 10u)
#define NW_MEMORY_2_KB			(1u << 11u)
#define NW_MEMORY_4_KB			(1u << 12u)
#define NW_MEMORY_8_KB			(1u << 13u)
#define NW_MEMORY_16_KB			(1u << 14u)
#define NW_MEMORY_32_KB			(1u << 15u)
#define NW_MEMORY_64_KB			(1u << 16u)
#define NW_MEMORY_128_KB		(1u << 17u)
#define NW_MEMORY_256_KB		(1u << 18u)
#define NW_MEMORY_512_KB		(1u << 19u)
#define NW_MEMORY_1_MB			(1u << 20u)
#define NW_MEMORY_2_MB			(1u << 21u)
#define NW_MEMORY_4_MB			(1u << 22u)
#define NW_MEMORY_8_MB			(1u << 23u)
#define NW_MEMORY_16_MB			(1u << 24u)
#define NW_MEMORY_32_MB			(1u << 25u)
#define NW_MEMORY_64_MB			(1u << 26u)
#define NW_MEMORY_128_MB        (1u << 27u)
#define NW_MEMORY_256_MB		(1u << 28u)
#define NW_MEMORY_512_MB		(1u << 29u)
#define NW_MEMORY_1024_MB		(1u << 30u)
#define NW_MEMORY_2048_MB		(1u << 31u)


namespace SettingsDefines
{
	namespace Engine
	{
		static uint32 kVulkanAllocatorSize = NW_MEMORY_16_MB;
		static uint32 kGPUAllocatorSize = NW_MEMORY_8_MB;
		static uint32 kInstanceAllocatorSize = NW_MEMORY_4_MB;
		static uint32 kQueueFamilyAllocatorSize = NW_MEMORY_256_KB;
		static uint32 kDeviceAllocatorSize = NW_MEMORY_256_KB;
		static uint32 kDescriptorPoolAllocatorSize = NW_MEMORY_16_KB;
		static uint32 kDescriptorSetLayoutAllocatorSize = NW_MEMORY_64_KB;
		static uint32 kDescriptorSetAllocatorSize = NW_MEMORY_128_KB;
		static uint32 kVertexDescriptorAllocatorSize = NW_MEMORY_32_KB;
		static uint32 kShaderAllocatorSize = NW_MEMORY_128_KB;
		static uint32 kSwapChainAllocatorSize = NW_MEMORY_512_KB;
		static uint32 kRenderPassAllocatorSize = NW_MEMORY_512_KB;
		static uint32 kPipelineLayoutAllocatorSize = NW_MEMORY_128_KB;
		static uint32 kStagingBufferSize = NW_MEMORY_512_MB;
		static uint32 kGpuDeviceLocalSize = NW_MEMORY_1024_MB;
		static uint32 kGpuHostVisibleSize = NW_MEMORY_1024_MB;
	}

	namespace Generic
	{
		static uint32 kCommonAllocatorSize = NW_MEMORY_4_MB;
		static uint32 kStringAllocatorSize = NW_MEMORY_8_MB;
		static uint32 kClientOptionAllocatorSize = NW_MEMORY_4_KB;
	}
}

NW_NAMESPACE_END