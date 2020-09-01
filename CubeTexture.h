#pragma once
#include "Bindable.h"

namespace Bind
{
	class OutputOnlyDepthStencil;

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

	class DepthCubeTexture : public Bindable
	{
	public:
		DepthCubeTexture(Graphics& gfx, UINT size, UINT slot = 0);

		virtual void Bind(Graphics& gfx) noxnd override;
		OutputOnlyDepthStencil& GetDepthBuffer(size_t index) const;
	private:
		unsigned int m_Slot;
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
		std::vector<std::unique_ptr<OutputOnlyDepthStencil>> m_DepthBuffers;
	};
}