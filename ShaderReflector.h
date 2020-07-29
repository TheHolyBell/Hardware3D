#pragma once
#include <d3d11.h>
#include "Vertex.h"

class ShaderReflector
{
public:
	static Dynamic::VertexLayout GetLayoutFromShader(ID3DBlob* shaderByteCode);
};