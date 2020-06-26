#pragma once

#include "Dependencies/Eos/Eos/Eos.h"
#include "Dependencies/Nix/Nix/Nix.h"

#include "Core/BasicDefines.h"
#include "Core/Assertions.h"
#include "Core/BasicTypes.h"
#include "Core/Settings.h"
#include "Core/SettingsDefines.h"
#include "Core/MemoryWrapper.h"

#include "GPU/GpuAllocator.h"
#include "GPU/GpuDataStructure.h"
#include "GPU/VulkanAllocator.h"
#include "GPU/GpuMemoryAllocator.h"
#include "GPU/GpuMemoryManager.h"
#include "GPU/GpuMemoryList.h"

#include "Utilities/HashTools.h"

#include "Renderer/Device.h"
#include "Renderer/QueueFamily.h"
#include "Renderer/CommandPool.h"
#include "Renderer/DescriptorPool.h"
#include "Renderer/DescriptorSetLayout.h"
#include "Renderer/DescriptorSet.h"
#include "Renderer/Device.h"
#include "Renderer/SwapChain.h"
#include "Renderer/Instance.h"
#include "Renderer/RenderPass.h"
#include "Renderer/PipelineLayout.h"
#include "Renderer/IndexLayout.h"
#include "Renderer/VertexLayout.h"
#include "Renderer/Pipeline.h"
#include "Renderer/StagingBufferManager.h"
#include "Renderer/Buffers/BaseBufferObject.h"
#include "Renderer/Buffers/IndexBufferObject.h"
#include "Renderer/Buffers/StorageBufferObject.h"
#include "Renderer/Buffers/UniformBufferObject.h"
#include "Renderer/Buffers/VertexBufferObject.h"
#include "Renderer/CommandBuffer.h"

#include "Shaders/Shader.h"



#include "Client/FileSystem.h"
#include "Client/CommandLineParser.h"
#include "Client/EngineApp.h"