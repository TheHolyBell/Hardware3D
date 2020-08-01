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

		virtual void Bind(Graphics& gfx) noexcept override;
		virtual std::string GetUID() const noexcept override;

		static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, UINT slot = 0);
		static std::string GenerateUID(const std::string& path, UINT slot = 0);
	private:
		UINT m_Slot;
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
		std::string m_Path;
	};
}