#pragma once
#include "Bindable.h"
#include <memory>
#include <string>

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture(Graphics& gfx, const std::string& path, UINT slot = 0);

		virtual void Bind(Graphics& gfx) noxnd override;
		virtual std::string GetUID() const noexcept override;

		static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, UINT slot = 0);
		static std::string GenerateUID(const std::string& path, UINT slot = 0);
		bool HasAlpha() const noexcept;
	private:
		static UINT CalculateNumberOfMipLevels(UINT width, UINT height) noexcept;
	private:
		UINT m_Slot;
	protected:
		bool m_bHasAlpha = false;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
		std::string m_Path;
	};
}