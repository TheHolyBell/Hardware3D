#pragma once
#include "Bindable.h"

namespace Bind
{
	class CubeTexture : public Bindable
	{
	public:
		CubeTexture(Graphics& gfx, const std::string& path, UINT slot = 0);
		virtual void Bind(Graphics& gfx) noxnd override;
	private:
		unsigned int m_Slot;
	protected:
		std::string m_Path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
	};
}