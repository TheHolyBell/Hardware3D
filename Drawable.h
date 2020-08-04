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
public:
	Drawable() = default;

	Drawable(const Drawable& rhs) = delete;
	Drawable& operator=(const Drawable& rhs) = delete;

	Drawable(Drawable&& rhs) = delete;
	Drawable& operator=(Drawable&& rhs) = delete;

	virtual ~Drawable() = default;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;

	void Draw(Graphics& gfx) const noxnd;

	template<typename T>
	T* QueryBindable() noexcept
	{
		for (auto& pb : m_Binds)
		{
			if (auto pt = dynamic_cast<T*>(pb.get()))
			{
				return pt;
			}
		}
		return nullptr;
	}
protected:
	void AddBind(std::shared_ptr<Bind::Bindable> pBind) noxnd;
private:
	const Bind::IndexBuffer* m_pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bind::Bindable>> m_Binds;
};