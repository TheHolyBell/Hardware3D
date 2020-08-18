#include "Blender.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Blender::Blender(Graphics& gfx, bool blending, std::optional<float> factor)
		: m_bBlending(blending)
	{
		INFOMAN(gfx);

		if (factor.has_value())
		{
			m_Factors.emplace();
			m_Factors->fill(*factor);
		}

		D3D11_BLEND_DESC _blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		auto& _blendRenderTarget = _blendDesc.RenderTarget[0];
		if (blending)
		{
			_blendRenderTarget.BlendEnable = true;

			if (factor)
			{
				_blendRenderTarget.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
				_blendRenderTarget.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			}
			else
			{
				_blendRenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				_blendRenderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			}
		}
		GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&_blendDesc, &m_pBlendState));
	}
	std::string Blender::GetUID() const noexcept
	{
		return GenerateUID(m_bBlending, m_Factors ? m_Factors->front() : std::optional<float>{});
	}
	std::shared_ptr<Blender> Blender::Resolve(Graphics& gfx, bool blending, std::optional<float> factor)
	{
		return Codex::Resolve<Blender>(gfx, blending, factor);
	}
	std::string Blender::GenerateUID(bool blending, std::optional<float> factor)
	{
		using namespace std::string_literals;
		return typeid(Blender).name() + "#"s + (blending ? "b"s : "n"s) + (factor ? "#f"s + std::to_string(*factor) : "");
	}

	void Blender::SetFactor(float factor) noxnd
	{
		assert(m_Factors);
		return m_Factors->fill(factor);
	}

	float Blender::GetFactor() const noxnd
	{
		assert(m_Factors);
		return m_Factors->front();
	}

	void Blender::Bind(Graphics& gfx) noxnd
	{
		const float* data = m_Factors ? m_Factors->data() : nullptr;
		GetContext(gfx)->OMSetBlendState(m_pBlendState.Get(), data, 0xFFFFFFFF);
	}
}
