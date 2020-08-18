#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include <vector>
#include <memory>

namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		VertexBuffer(Graphics& gfx, const std::string& tag, const Dynamic::VertexBuffer& vbuf);
		VertexBuffer(Graphics& gfx, const Dynamic::VertexBuffer& vbuf);
		
		virtual void Bind(Graphics& gfx) noxnd override;
		virtual std::string GetUID() const noexcept override;
		const Dynamic::VertexLayout& GetLayout() const noexcept;

		static std::shared_ptr<VertexBuffer> Resolve(Graphics& gfx, const std::string& tag,
			const Dynamic::VertexBuffer& vbuf);
		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
	private:
		static std::string GenerateUID_(const std::string& tag);
	protected:
		std::string m_Tag;
		UINT m_Stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		Dynamic::VertexLayout m_Layout;
	};
}