#pragma once
#include <string>
#include <memory>
#include "RenderGraphCompileException.h"

namespace Bind
{
	class Bindable;
	class BufferResource;
}

namespace RenderGraph
{
	class Source
	{
	public:
		const std::string& GetName() const noexcept;
		virtual void PostLinkValidate() const = 0;

		virtual std::shared_ptr<Bind::Bindable> YieldBindable();
		virtual std::shared_ptr<Bind::BufferResource> YieldBuffer();
		virtual ~Source() = default;
	protected:
		Source(const std::string& name);
	private:
		std::string m_Name;
	};

	template<typename T>
	class DirectBufferSource : public Source
	{
	public:
		static std::unique_ptr<DirectBufferSource> Make(const std::string& name, std::shared_ptr<T>& buffer)
		{
			return std::make_unique<DirectBufferSource>(name, buffer);
		}

		DirectBufferSource(const std::string& name, std::shared_ptr<T>& buffer)
			: Source(name), m_Buffer(buffer){}

		virtual void PostLinkValidate() const override{}
		virtual std::shared_ptr<Bind::BufferResource> YieldBuffer() override
		{
			if (m_bLinked)
				throw RGC_EXCEPTION("Mutable output bound twice: " + GetName());
			m_bLinked = true;
			return m_Buffer;
		}
	private:
		std::shared_ptr<T>& m_Buffer;
		bool m_bLinked = false;
	};

	template<typename T>
	class DirectBindableSource : public Source
	{
	public:
		static std::unique_ptr<DirectBindableSource> Make(const std::string& name, std::shared_ptr<T>& buffer)
		{
			return std::make_unique<DirectBindableSource>(name, buffer);
		}
		DirectBindableSource(const std::string& name, std::shared_ptr<T>& bind)
			: Source(name), m_Bind(bind)
		{}

		virtual void PostLinkValidate() const override{}
		virtual std::shared_ptr<Bind::Bindable> YieldBindable() override
		{
			return m_Bind;
		}
	private:
		std::shared_ptr<T>& m_Bind;
	};
}