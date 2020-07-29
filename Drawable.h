#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include <memory>
#include "ConditionalNoexcept.h"

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
	template<typename T>
	friend class DrawableBase;
public:
	Drawable() = default;
	
	Drawable(const Drawable& rhs) = delete;

	virtual ~Drawable() = default;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noxnd;
	virtual void Update(float dt) noexcept{}
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
	void AddBind(std::unique_ptr<Bind::Bindable> bind) noxnd;
	void AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer> indexBuffer) noexcept;
private:
	virtual const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	const Bind::IndexBuffer* m_pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bind::Bindable>> m_Binds;
};