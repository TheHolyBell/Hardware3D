#pragma once
#include <d3d11.h>
#include "Vertex.h"
#include "DynamicConstant.h"

class ShaderReflector
{
public:
	static Dynamic::VertexLayout GetLayoutFromShader(ID3DBlob* shaderByteCode);
	static Dynamic::Buffer GetBufferByRegister(ID3DBlob* shaderByteCode, UINT Register);
};