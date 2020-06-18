#pragma once

#include "../Core/BasicTypes.h"


UD_NAMESPACE_BEGIN

#define UD_MEMORY_1_KB			(1u << 10u)
#define UD_MEMORY_2_KB			(1u << 11u)
#define UD_MEMORY_4_KB			(1u << 12u)
#define UD_MEMORY_8_KB			(1u << 13u)
#define UD_MEMORY_16_KB			(1u << 14u)
#define UD_MEMORY_32_KB			(1u << 15u)
#define UD_MEMORY_64_KB			(1u << 16u)
#define UD_MEMORY_128_KB		(1u << 17u)
#define UD_MEMORY_256_KB		(1u << 18u)
#define UD_MEMORY_512_KB		(1u << 19u)
#define UD_MEMORY_1_MB			(1u << 20u)
#define UD_MEMORY_2_MB			(1u << 21u)
#define UD_MEMORY_4_MB			(1u << 22u)
#define UD_MEMORY_8_MB			(1u << 23u)
#define UD_MEMORY_16_MB			(1u << 24u)
#define UD_MEMORY_32_MB			(1u << 25u)
#define UD_MEMORY_64_MB			(1u << 26u)
#define UD_MEMORY_128_MB        (1u << 27u)
#define UD_MEMORY_256_MB		(1u << 28u)
#define UD_MEMORY_512_MB		(1u << 29u)
#define UD_MEMORY_1024_MB		(1u << 30u)
#define UD_MEMORY_2048_MB		(1u << 31u)


namespace SettingsDefines
{
	namespace Engine
	{
		static uint32 kVulkanAllocatorSize = UD_MEMORY_16_MB;
		static uint32 kGPUAllocatorSize = UD_MEMORY_8_MB;
		static uint32 kRenderInstanceAllocatorSize = UD_MEMORY_4_MB;
		static uint32 kRenderQueueFamilyAllocatorSize = UD_MEMORY_256_KB;
		static uint32 kRenderDeviceAllocatorSize = UD_MEMORY_256_KB;
		static uint32 kStagingBufferSize = UD_MEMORY_512_MB;
		static uint32 kGpuDeviceLocalSize = UD_MEMORY_1024_MB;
		static uint32 kGpuHostVisibleSize = UD_MEMORY_1024_MB;
	}

	namespace Generic
	{
		static uint32 kCommonAllocatorSize = UD_MEMORY_4_MB;
		static uint32 kStringAllocatorSize = UD_MEMORY_8_MB;
		static uint32 kClientOptionAllocatorSize = UD_MEMORY_4_KB;
	}
}

UD_NAMESPACE_END