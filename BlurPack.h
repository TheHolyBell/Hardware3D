#pragma once
#include "BindableCommon.h"
#include "ChiliMath.h"
#include "ImGui\imgui.h"

class BlurPack
{
public:
	BlurPack(Graphics& gfx, int radius = 7, float sigma = 2.6f, const char* shader = "Blur_PS.cso")
		:
		m_Shader(gfx, shader),
		m_Pcb(gfx, 0u),
		m_Ccb(gfx, 1u),
		m_Radius(radius),
		m_Sigma(sigma)
	{
		SetKernelGauss(gfx, radius, sigma);
	}
	void Bind(Graphics& gfx) noexcept
	{
		m_Shader.Bind(gfx);
		m_Pcb.Bind(gfx);
		m_Ccb.Bind(gfx);
	}
	void SetHorizontal(Graphics& gfx)
	{
		m_Ccb.Update(gfx, { TRUE });
	}
	void SetVertical(Graphics& gfx)
	{
		m_Ccb.Update(gfx, { FALSE });
	}
	void RenderWidgets(Graphics& gfx)
	{
		bool filterChanged = false;
		{
			const char* items[] = { "Gauss","Box" };
			static const char* curItem = items[0];
			if (ImGui::BeginCombo("Filter Type", curItem))
			{
				for (int n = 0; n < std::size(items); n++)
				{
					const bool isSelected = (curItem == items[n]);
					if (ImGui::Selectable(items[n], isSelected))
					{
						filterChanged = true;
						curItem = items[n];
						if (curItem == items[0])
						{
							m_KernelType = KernelType::Gauss;
						}
						else if (curItem == items[1])
						{
							m_KernelType = KernelType::Box;
						}
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		bool radChange = ImGui::SliderInt("Radius", &m_Radius, 0, 15);
		bool sigChange = ImGui::SliderFloat("Sigma", &m_Sigma, 0.1f, 10.0f);
		if (radChange || sigChange || filterChanged)
		{
			if (m_KernelType == KernelType::Gauss)
			{
				SetKernelGauss(gfx, m_Radius, m_Sigma);
			}
			else if (m_KernelType == KernelType::Box)
			{
				SetKernelBox(gfx, m_Radius);
			}
		}
	}
	// for more accurate coefs, need to integrate, but meh :/
	void SetKernelGauss(Graphics& gfx, int radius, float sigma) noxnd
	{
		assert(radius <= s_MaxRadius);
		Kernel k;
		k.nTaps = radius * 2 + 1;
		float sum = 0.0f;
		for (int i = 0; i < k.nTaps; i++)
		{
			const auto x = float(i - radius);
			const auto g = gauss(x, sigma);
			sum += g;
			k.coefficients[i].x = g;
		}
		for (int i = 0; i < k.nTaps; i++)
		{
			k.coefficients[i].x /= sum;
		}
		m_Pcb.Update(gfx, k);
	}
	void SetKernelBox(Graphics& gfx, int radius) noxnd
	{
		assert(radius <= s_MaxRadius);
		Kernel k;
		k.nTaps = radius * 2 + 1;
		const float c = 1.0f / k.nTaps;
		for (int i = 0; i < k.nTaps; i++)
		{
			k.coefficients[i].x = c;
		}
		m_Pcb.Update(gfx, k);
	}
private:
	enum class KernelType
	{
		Gauss,
		Box,
	};
	static constexpr int s_MaxRadius = 15;
	int m_Radius;
	float m_Sigma;
	KernelType m_KernelType = KernelType::Gauss;
	struct Kernel
	{
		int nTaps;
		float padding[3];
		DirectX::XMFLOAT4 coefficients[s_MaxRadius * 2 + 1];
	};
	struct Control
	{
		BOOL horizontal;
		float padding[3];
	};
	Bind::PixelShader m_Shader;
	Bind::PixelConstantBuffer<Kernel> m_Pcb;
	Bind::PixelConstantBuffer<Control> m_Ccb;
};