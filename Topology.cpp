#include "Topology.h"
#include "BindableCodex.h"

namespace Bind
{
	Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology)
		: m_Topology(topology)
	{

	}

	void Topology::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetPrimitiveTopology(m_Topology);
	}
	std::string Topology::GetUID() const noexcept
	{
		return GenerateUID(m_Topology);
	}
	std::shared_ptr<Topology> Topology::Resolve(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		return Codex::Resolve<Topology>(gfx, topology);
	}
	std::string Topology::GenerateUID(D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		using namespace std::string_literals;
		return typeid(Topology).name() + "#"s + std::to_string(topology);
	}
}