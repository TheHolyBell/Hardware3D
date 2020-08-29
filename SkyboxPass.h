#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include "Camera.h"
#include "Sampler.h"
#include "Rasterizer.h"
#include "DepthStencil.h"
#include "Cube.h"
#include "CubeTexture.h"
#include "SkyboxTransformCBuf.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Topology.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "Sphere.h"
#include "ImGui/imgui.h"

class Graphics;

namespace RenderGraph
{
	class SkyboxPass : public BindingPass
	{
	public:
		SkyboxPass(Graphics& gfx, std::string name)
			:
			BindingPass(std::move(name))
		{
			using namespace Bind;
			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", m_RenderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", m_DepthStencil));
			AddBind(std::make_shared<CubeTexture>(gfx, "Images\\SpaceBox"));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::DepthFirst));
			AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear));
			AddBind(Rasterizer::Resolve(gfx, true));
			AddBind(std::make_shared<SkyboxTransformCbuf>(gfx));
			AddBind(PixelShader::Resolve(gfx, "Skybox_PS.cso"));
			AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
			{ // geometry-related
				auto pvs = Bind::VertexShader::Resolve(gfx, "Skybox_VS.cso");
				{ // cube
					auto model = Cube::Make();
					const auto geometryTag = "$cube_map";
					m_pCubeVertices = VertexBuffer::Resolve(gfx, geometryTag, std::move(model.vertices));
					m_pCubeIndices = IndexBuffer::Resolve(gfx, geometryTag, std::move(model.indices));
					m_CubeCount = (UINT)model.indices.size();
					// layout is shared between cube and sphere; use cube data to generate
					AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
				}
				{ // sphere
					auto model = Sphere::Make();
					const auto geometryTag = "$sphere_map";
					m_pSphereVertices = VertexBuffer::Resolve(gfx, geometryTag, std::move(model.vertices));
					m_pSphereIndices = IndexBuffer::Resolve(gfx, geometryTag, std::move(model.indices));
					m_SphereCount = (UINT)model.indices.size();
				}
				AddBind(std::move(pvs));
			}
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", m_RenderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", m_DepthStencil));
		}

		void BindMainCamera(const Camera& camera) noexcept
		{
			m_pMainCamera = &camera;
		}

		virtual void Execute(Graphics& gfx) const noxnd override
		{
			assert(m_pMainCamera);
			UINT indexCount;
			m_pMainCamera->BindToGraphics(gfx);
			if (m_bUseSphere)
			{
				m_pSphereVertices->Bind(gfx);
				m_pSphereIndices->Bind(gfx);
				indexCount = m_SphereCount;
			}
			else
			{
				m_pCubeVertices->Bind(gfx);
				m_pCubeIndices->Bind(gfx);
				indexCount = m_CubeCount;
			}
			BindAll(gfx);
			gfx.DrawIndexed(indexCount);
		}
		void RenderWindow()
		{
			if (ImGui::Begin("Skybox"))
				ImGui::Checkbox("Use sphere", &m_bUseSphere);
			ImGui::End();
		}
	private:
		bool m_bUseSphere = true;
		const Camera* m_pMainCamera = nullptr;
		std::shared_ptr<Bind::VertexBuffer> m_pCubeVertices;
		std::shared_ptr<Bind::IndexBuffer> m_pCubeIndices;
		UINT m_CubeCount;
		std::shared_ptr<Bind::VertexBuffer> m_pSphereVertices;
		std::shared_ptr<Bind::IndexBuffer> m_pSphereIndices;
		UINT m_SphereCount;
	};
}