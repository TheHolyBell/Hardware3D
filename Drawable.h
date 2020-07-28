#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include <memory>

class Bindable;

class Drawable
{
	template<typename T>
	friend class DrawableBase;
public:
	Drawable() = default;
	
	Drawable(const Drawable& rhs) = delete;

	virtual ~Drawable() = default;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept = 0;
protected:
	template<class T>
	T* QueryBindable() noexcept
	{
		for (auto& pb : m_Binds)
		{
			if (auto pt = dynamic_cast<T*>(pb.get()))
				return pt;
		}
		return nullptr;
	}
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuffer) noexcept;
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	const IndexBuffer* m_pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> m_Binds;
};