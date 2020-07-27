#include "Settings.h"

#include "SettingsDefines.h"

#include "../Core/Assertions.h"

#include "../Dependencies/json.hpp"


NW_NAMESPACE_BEGIN

static constexpr const char* kMemorySettingsFileName = "MemorySettings.json";

Settings& Settings::Instance()
{
	static Settings instance;
	return instance;
}

Settings::Settings()
{
}

Settings::~Settings()
{
}

void Settings::LoadMemorySettings()
{
	// if is no file, it creates and fill with basic information and default values.
	std::ifstream is(kMemorySettingsFileName);
	if (!is.is_open())
	{
		// create the file with the default values and read again
		nlohmann::json nj = {
			{"Engine", {
				{"Vulkan", SettingsDefines::Engine::kVulkanAllocatorSize},
				{"GPU", SettingsDefines::Engine::kGPUAllocatorSize},
				{"StagingBuffer", SettingsDefines::Engine::kStagingBufferSize},
				{"DeviceLocal", SettingsDefines::Engine::kGpuDeviceLocalSize},
				{"HostVisible", SettingsDefines::Engine::kGpuHostVisibleSize},
				{"Instance", SettingsDefines::Engine::kInstanceAllocatorSize},
				{"QueueFamily", SettingsDefines::Engine::kQueueFamilyAllocatorSize},
				{"Device", SettingsDefines::Engine::kDeviceAllocatorSize},
				{"DescriptorPool", SettingsDefines::Engine::kDescriptorPoolAllocatorSize},
				{"DescriptorSetLayout", SettingsDefines::Engine::kDescriptorSetLayoutAllocatorSize},
				{"DescriptorSet", SettingsDefines::Engine::kDescriptorSetAllocatorSize},
				{"VertexDescriptor", SettingsDefines::Engine::kVertexDescriptorAllocatorSize},
				{"Shader", SettingsDefines::Engine::kShaderAllocatorSize},
				{"SwapChain", SettingsDefines::Engine::kSwapChainAllocatorSize},
				{"RenderPass", SettingsDefines::Engine::kRenderPassAllocatorSize},
				{"PipelineLayout", SettingsDefines::Engine::kPipelineLayoutAllocatorSize},
				{"Pipeline", SettingsDefines::Engine::kPipelineAllocatorSize},
				{"CommandBuffer", SettingsDefines::Engine::kCommandBufferAllocatorSize},
				{"ThreadPool", SettingsDefines::Engine::kThreadPoolAllocatorSize},
				{"FrameBuffer", SettingsDefines::Engine::kFrameBufferAllocatorSize},
				
			}},
			{"Generic", {
				{"Common", SettingsDefines::Generic::kCommonAllocatorSize},
				{"String", SettingsDefines::Generic::kStringAllocatorSize},
				{"ClientOptions", SettingsDefines::Generic::kClientOptionAllocatorSize},
			}},
			{"Objects", {
				{"Node", SettingsDefines::Objects::kNodeAllocatorSize}
			}}
			
		};

 		std::ofstream os(kMemorySettingsFileName);
		os << nj.dump(4);
		os.close();

		is.open(kMemorySettingsFileName);
		nwAssertReturnVoid(is.is_open(), "Cannot open nor create the %s file", kMemorySettingsFileName);
	}

	nlohmann::json json;
	is >> json;
	is.close();

	// actual settings - Engine
	SettingsDefines::Engine::kVulkanAllocatorSize = json["Engine"]["Vulkan"];
	SettingsDefines::Engine::kGPUAllocatorSize = json["Engine"]["GPU"];
	SettingsDefines::Engine::kVulkanAllocatorSize = json["Engine"]["StagingBuffer"];
	SettingsDefines::Engine::kGPUAllocatorSize = json["Engine"]["DeviceLocal"];
	SettingsDefines::Engine::kGPUAllocatorSize = json["Engine"]["HostVisible"];
	SettingsDefines::Engine::kInstanceAllocatorSize = json["Engine"]["Instance"];
	SettingsDefines::Engine::kQueueFamilyAllocatorSize = json["Engine"]["QueueFamily"];
	SettingsDefines::Engine::kDeviceAllocatorSize = json["Engine"]["Device"];
	SettingsDefines::Engine::kDescriptorPoolAllocatorSize = json["Engine"]["DescriptorPool"];
	SettingsDefines::Engine::kDescriptorSetLayoutAllocatorSize = json["Engine"]["DescriptorSetLayout"];
	SettingsDefines::Engine::kDescriptorSetAllocatorSize = json["Engine"]["DescriptorSet"];
	SettingsDefines::Engine::kVertexDescriptorAllocatorSize = json["Engine"]["VertexDescriptor"];
	SettingsDefines::Engine::kShaderAllocatorSize = json["Engine"]["Shader"];
	SettingsDefines::Engine::kSwapChainAllocatorSize = json["Engine"]["SwapChain"];
	SettingsDefines::Engine::kRenderPassAllocatorSize = json["Engine"]["RenderPass"];
	SettingsDefines::Engine::kPipelineLayoutAllocatorSize = json["Engine"]["PipelineLayout"];
	SettingsDefines::Engine::kPipelineAllocatorSize = json["Engine"]["Pipeline"];
	SettingsDefines::Engine::kCommandBufferAllocatorSize = json["Engine"]["CommandBuffer"];
	SettingsDefines::Engine::kThreadPoolAllocatorSize = json["Engine"]["ThreadPool"];
	SettingsDefines::Engine::kFrameBufferAllocatorSize = json["Engine"]["FrameBuffer"];


	// actual settings - Generic
	SettingsDefines::Generic::kCommonAllocatorSize = json["Generic"]["Common"];
	SettingsDefines::Generic::kStringAllocatorSize = json["Generic"]["String"];
	SettingsDefines::Generic::kClientOptionAllocatorSize = json["Generic"]["ClientOptions"];


	// actual settings - Objects
	SettingsDefines::Objects::kNodeAllocatorSize = json["Objects"]["Node"];
}

NW_NAMESPACE_END