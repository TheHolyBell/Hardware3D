#include "Drawable.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>

using namespace Bind;

void Drawable::Draw(Graphics& gfx) const noxnd
{
	for (const auto& pb : m_Binds)
		pb->Bind(gfx);
	gfx.DrawIndexed(m_pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<Bindable> pBind) noxnd
{
	// Special case for index buffer
	if (typeid(*pBind) == typeid(IndexBuffer))
	{
		assert(m_pIndexBuffer == nullptr && "Binding multiple index buffer is not allowed");
		m_pIndexBuffer = &static_cast<IndexBuffer&>(*pBind);
	}
	m_Binds.push_back(std::move(pBind));
}