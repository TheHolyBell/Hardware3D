#pragma once
#include "Bindable.h"
#include <vector>
#include <memory>

namespace Bind
{
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
		IndexBuffer(Graphics& gfx, std::string tag, const std::vector<unsigned short>& indices);

		virtual void Bind(Graphics& gfx) noexcept override;
		virtual std::string GetUID() const noexcept override;
		UINT GetCount() const noexcept;

		static std::shared_ptr<IndexBuffer> Resolve(Graphics& gfx, const std::string& tag,
			const std::vector<unsigned short>& indices);

		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
	private:
		static std::string GenerateUID_(const std::string& tag);
	protected:
		std::string m_Tag;
		UINT m_Count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	};
}