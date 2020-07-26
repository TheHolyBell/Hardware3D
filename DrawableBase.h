#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

template<typename T>
class DrawableBase : public Drawable
{
public:
	static bool IsStaticInitialized() noexcept
	{
		return !s_StaticBinds.empty();
	}
	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		s_StaticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && m_pIndexBuffer == nullptr);
		m_pIndexBuffer = ibuf.get();
		s_StaticBinds.push_back(std::move(ibuf));
	}
	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && m_pIndexBuffer == nullptr);
		for (const auto& b : s_StaticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				m_pIndexBuffer = p;
				return;
			}
		}
		assert("Failed to find index buffer in static binds" && m_pIndexBuffer != nullptr);
	}
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return s_StaticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bindable>> s_StaticBinds;
};

template<typename T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::s_StaticBinds;