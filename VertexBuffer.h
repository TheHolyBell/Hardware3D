#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include <vector>

namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		template<typename V>
		VertexBuffer(Graphics& gfx, const std::vector<V>& vertices)
			: m_Stride(sizeof(V))
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC bd = {};
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = UINT(sizeof(V) * vertices.size());
			bd.StructureByteStride = sizeof(V);
			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = vertices.data();
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &m_pVertexBuffer));
		}
		VertexBuffer(Graphics& gfx, const Dynamic::VertexBuffer& vbuf)
			: m_Stride(vbuf.GetLayout().Size())
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC _vbDesc = {};
			_vbDesc.Usage = D3D11_USAGE_DEFAULT;
			_vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			_vbDesc.ByteWidth = vbuf.SizeBytes();

			D3D11_SUBRESOURCE_DATA _vinitData = {};
			_vinitData.pSysMem = vbuf.GetData();

			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&_vbDesc, &_vinitData, &m_pVertexBuffer));
		}
		virtual void Bind(Graphics& gfx) noexcept override;
	protected:
		UINT m_Stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	};
}