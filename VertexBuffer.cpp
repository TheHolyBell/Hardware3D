#include "VertexBuffer.h"

void Bind::VertexBuffer::Bind(Graphics& gfx) noexcept
{
	UINT offset = 0;
	GetContext(gfx)->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &m_Stride, &offset);
}
