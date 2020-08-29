#pragma once
#include "Bindable.h"

namespace Bind
{
	class ShadowSampler : public Bindable
	{
	public:
		ShadowSampler(Graphics& gfx);
		virtual void Bind(Graphics& gfx) noxnd override;

		void SetBilinear(bool bilinear);
		void SetHwPcf(bool hwPcf);
		bool GetBilinear() const;
		bool GetHwPcf() const;
	private:
		UINT GetCurrentSlot() const;
		static size_t ShadowSamplerIndex(bool bilinear, bool hwPcf);
		static Microsoft::WRL::ComPtr<ID3D11SamplerState> MakeSampler(Graphics& gfx, bool bilinear, bool hwPcf);
	protected:
		size_t m_CurrentSampler;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerStates[4];
	};
}