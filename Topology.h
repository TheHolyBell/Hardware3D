#pragma once
#include "Bindable.h"
#include <memory>

namespace Bind
{
	class Topology : public Bindable
	{
	public:
		Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology);
		
		virtual void Bind(Graphics& gfx) noxnd override;
		virtual std::string GetUID() const noexcept override;

		static std::shared_ptr<Topology> Resolve(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		static std::string GenerateUID(D3D11_PRIMITIVE_TOPOLOGY topology);
	protected:
		D3D11_PRIMITIVE_TOPOLOGY m_Topology;
	};
}