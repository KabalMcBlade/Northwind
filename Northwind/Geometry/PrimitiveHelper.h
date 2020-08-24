#pragma once

//#include "../Dependencies/Nix/Nix/Nix.h"
#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"

#include "../Renderer/IndexLayout.h"
#include "../Renderer/VertexLayout.h"


NW_NAMESPACE_BEGIN

namespace PrimitiveHelper
{
	void GenerateCube(Vertex_P(&_verices)[8], Index(&_indices)[36]);
	void GenerateCube(Vertex_C(&_verices)[8], Index(&_indices)[36]);
};

NW_NAMESPACE_END