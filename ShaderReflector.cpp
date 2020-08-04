#include "ShaderReflector.h"

#include <wrl\client.h>
#include <d3dcompiler.h>
#include <string>
#include <iostream>

static bool icompare_pred(unsigned char a, unsigned char b)
{
    return std::tolower(a) == std::tolower(b);
}

static bool icompare(const std::string& a, const std::string& b)
{
    if (a.length() == b.length()) {
        return std::equal(b.begin(), b.end(),
            a.begin(), icompare_pred);
    }
    else {
        return false;
    }
}

Dynamic::VertexLayout ShaderReflector::GetLayoutFromShader(ID3DBlob* shaderByteCode)
{
	using Dynamic::VertexLayout;
	VertexLayout _vertexLayout;

	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> _pReflector;
	D3DReflect(shaderByteCode->GetBufferPointer(), shaderByteCode->GetBufferSize(),
		IID_ID3D11ShaderReflection, &_pReflector);

	D3D11_SHADER_DESC _ShaderDesc = {};
	_pReflector->GetDesc(&_ShaderDesc);

	auto InputParametersCount = _ShaderDesc.InputParameters;

    DXGI_FORMAT Format;

	for (int i = 0; i < InputParametersCount; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC _inputParameterDesc = {};
		_pReflector->GetInputParameterDesc(i, &_inputParameterDesc);
		std::string _SemanticName(_inputParameterDesc.SemanticName);

        // determine DXGI format
        if (_inputParameterDesc.Mask == 1)
        {
            if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) Format = DXGI_FORMAT_R32_UINT;
            else if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) Format = DXGI_FORMAT_R32_SINT;
            else if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) Format = DXGI_FORMAT_R32_FLOAT;
        }
        else if (_inputParameterDesc.Mask <= 3)
        {
            if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) Format = DXGI_FORMAT_R32G32_UINT;
            else if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) Format = DXGI_FORMAT_R32G32_SINT;
            else if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) Format = DXGI_FORMAT_R32G32_FLOAT;
        }
        else if (_inputParameterDesc.Mask <= 7)
        {
            if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) Format = DXGI_FORMAT_R32G32B32_UINT;
            else if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) Format = DXGI_FORMAT_R32G32B32_SINT;
            else if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) Format = DXGI_FORMAT_R32G32B32_FLOAT;
        }
        else if (_inputParameterDesc.Mask <= 15)
        {
            if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) Format = DXGI_FORMAT_R32G32B32A32_UINT;
            else if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) Format = DXGI_FORMAT_R32G32B32A32_SINT;
            else if (_inputParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }

        if (icompare(_SemanticName,"POSITION"))
        {
            if (Format == DXGI_FORMAT_R32G32_FLOAT)
                _vertexLayout.Append(VertexLayout::Position2D);
            else if (Format == DXGI_FORMAT_R32G32B32_FLOAT)
                _vertexLayout.Append(VertexLayout::Position3D);
        }
        else if (icompare(_SemanticName,"NORMAL"))
        {
            if (Format == DXGI_FORMAT_R32G32B32_FLOAT)
                _vertexLayout.Append(VertexLayout::Normal);
        }
        else if (icompare(_SemanticName,"TEXCOORD"))
        {
            if (Format == DXGI_FORMAT_R32G32_FLOAT)
                _vertexLayout.Append(VertexLayout::Texture2D);
        }
        else if (icompare(_SemanticName,"COLOR"))
        {
            if (Format == DXGI_FORMAT_R32G32B32_FLOAT)
                _vertexLayout.Append(VertexLayout::Float3Color);
            else if (Format == DXGI_FORMAT_R32G32B32A32_FLOAT)
                _vertexLayout.Append(VertexLayout::Float4Color);
        }
        else if (icompare(_SemanticName,"BITANGENT"))
        {
            if (Format == DXGI_FORMAT_R32G32B32_FLOAT)
                _vertexLayout.Append(VertexLayout::Bitangent);
        }
        else if (icompare(_SemanticName,"TANGENT"))
        {
            if (Format == DXGI_FORMAT_R32G32B32_FLOAT)
                _vertexLayout.Append(VertexLayout::Tangent);
        }
	}

	return _vertexLayout;
}
