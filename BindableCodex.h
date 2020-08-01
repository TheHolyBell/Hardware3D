#pragma once

#include "Bindable.h"
#include "ConditionalNoexcept.h"
#include <type_traits>
#include <memory>
#include <unordered_map>

namespace Bind
{
	class Codex
	{
	public:
		template<class T, typename...Params>
		static std::shared_ptr<T> Resolve(Graphics& gfx, Params&&...p) noxnd
		{
			//static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
			return Get().Resolve_<T>(gfx, std::forward<Params>(p)...);
		}
	private:
		template<class T, typename...Params>
		std::shared_ptr<T> Resolve_(Graphics& gfx, Params&&...p) noxnd
		{
			const auto key = T::GenerateUID(std::forward<Params>(p)...);
			const auto i = m_Binds.find(key);
			if (i == m_Binds.end())
			{
				auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
				m_Binds[key] = bind;
				return bind;
			}
			else
			{
				return std::static_pointer_cast<T>(i->second);
			}
		}
		static Codex& Get()
		{
			static Codex _Codex;
			return _Codex;
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<Bindable>> m_Binds;
	};
}