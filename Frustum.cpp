#include "Frustum.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Sphere.h"
#include "Stencil.h"
#include "Channels.h"

namespace dx = DirectX;

Frustum::Frustum(Graphics& gfx, float width, float height, float nearZ, float farZ)
{
	using namespace Bind;

	std::vector<unsigned short> _Indices;
	{
		_Indices.push_back(0);
		_Indices.push_back(1);
		_Indices.push_back(1);
		_Indices.push_back(2);
		_Indices.push_back(2);
		_Indices.push_back(3);
		_Indices.push_back(3);
		_Indices.push_back(0);
		_Indices.push_back(4);
		_Indices.push_back(5);
		_Indices.push_back(5);
		_Indices.push_back(6);
		_Indices.push_back(6);
		_Indices.push_back(7);
		_Indices.push_back(7);
		_Indices.push_back(4);
		_Indices.push_back(0);
		_Indices.push_back(4);
		_Indices.push_back(1);
		_Indices.push_back(5);
		_Indices.push_back(2);
		_Indices.push_back(6);
		_Indices.push_back(3);
		_Indices.push_back(7);
	}

	SetVertices(gfx, width, height, nearZ, farZ);
	m_pIndices = IndexBuffer::Resolve(gfx, "$frustum", _Indices);
	m_pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	{
		Technique line{ Channels::main };
		{
			Step unoccluded("lambertian");

			auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
			unoccluded.AddBindable(InputLayout::Resolve(gfx, m_pVertices->GetLayout(), *pvs));
			unoccluded.AddBindable(std::move(pvs));

			unoccluded.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

			struct PSColorConstant
			{
				dx::XMFLOAT3 color = { 0.6f,0.2f,0.2f };
				float padding;
			} colorConst;
			unoccluded.AddBindable(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u));

			unoccluded.AddBindable(std::make_shared<TransformCbuf>(gfx));

			unoccluded.AddBindable(Rasterizer::Resolve(gfx, false));

			line.AddStep(std::move(unoccluded));
		}
		{
			Step occluded("wireframe");

			auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
			occluded.AddBindable(InputLayout::Resolve(gfx, m_pVertices->GetLayout(), *pvs));
			occluded.AddBindable(std::move(pvs));

			occluded.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

			struct PSColorConstant2
			{
				dx::XMFLOAT3 color = { 0.25f,0.08f,0.08f };
				float padding;
			} colorConst;
			occluded.AddBindable(PixelConstantBuffer<PSColorConstant2>::Resolve(gfx, colorConst, 1u));

			occluded.AddBindable(std::make_shared<TransformCbuf>(gfx));

			occluded.AddBindable(Rasterizer::Resolve(gfx, false));

			line.AddStep(std::move(occluded));
		}
		AddTechnique(std::move(line));
	}
}

void Frustum::SetVertices(Graphics& gfx, float width, float height, float nearZ, float farZ)
{
	Dynamic::VertexLayout layout;
	layout.Append(Dynamic::VertexLayout::Position3D);
	Dynamic::VertexBuffer vertices{ std::move(layout) };
	{
		const float zRatio = farZ / nearZ;
		const float nearX = width / 2.0f;
		const float nearY = height / 2.0f;
		const float farX = nearX * zRatio;
		const float farY = nearY * zRatio;
		vertices.EmplaceBack(dx::XMFLOAT3{ -nearX,nearY,nearZ });
		vertices.EmplaceBack(dx::XMFLOAT3{ nearX,nearY,nearZ });
		vertices.EmplaceBack(dx::XMFLOAT3{ nearX,-nearY,nearZ });
		vertices.EmplaceBack(dx::XMFLOAT3{ -nearX,-nearY,nearZ });
		vertices.EmplaceBack(dx::XMFLOAT3{ -farX,farY,farZ });
		vertices.EmplaceBack(dx::XMFLOAT3{ farX,farY,farZ });
		vertices.EmplaceBack(dx::XMFLOAT3{ farX,-farY,farZ });
		vertices.EmplaceBack(dx::XMFLOAT3{ -farX,-farY,farZ });
	}
	m_pVertices = std::make_shared<Bind::VertexBuffer>(gfx, vertices);
}

void Frustum::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	m_Pos = pos;
}
void Frustum::SetRotation(DirectX::XMFLOAT3 rot) noexcept
{
	m_Rot = rot;
}

DirectX::XMMATRIX Frustum::GetTransformXM() const noexcept
{
	return dx::XMMatrixRotationRollPitchYawFromVector(dx::XMLoadFloat3(&m_Rot)) *
		dx::XMMatrixTranslationFromVector(dx::XMLoadFloat3(&m_Pos));
}
