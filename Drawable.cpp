#include "Drawable.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>
#include <iostream>

void Drawable::Draw(Graphics& gfx) const noxnd
{
	for (auto& b : m_Binds)
		b->Bind(gfx);
	for (auto& b : GetStaticBinds())
		b->Bind(gfx);
	gfx.DrawIndexed(m_pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bind::Bindable> bind) noxnd
{
	assert("Must use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(Bind::IndexBuffer));
	m_Binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer> indexBuffer) noexcept
{
	assert("Attempting to add index buffer for a second time" && m_pIndexBuffer == nullptr);
	m_pIndexBuffer = indexBuffer.get();
	m_Binds.push_back(std::move(indexBuffer));
}
