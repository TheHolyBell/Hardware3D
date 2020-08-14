#pragma once
#include <string>
#include <memory>
#include <typeinfo>
#include "Source.h"
#include "RenderGraphCompileException.h"
#include "ChiliUtil.h"
#include "Bindable.h"
#include "BufferResource.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include <sstream>
#include <cctype>
#include <type_traits>

namespace Bind
{
	class Bindable;
}

namespace RenderGraph
{
	class Pass;

	class Sink
	{
	public:
		const std::string& GetRegisteredName() const noexcept;
		const std::string& GetPassName() const noexcept;
		const std::string& GetOutputName() const noexcept;
		void SetTarget(const std::string& passName, const std::string& outputName);
		virtual void Bind(Source& source) = 0;
		virtual void PostLinkValidate() const = 0;
		virtual ~Sink() = default;
	protected:
		Sink(const std::string& registeredName);
	private:
		std::string m_RegisteredName;
		std::string m_PassName;
		std::string m_OutputName;
	};

	template<typename T>
	class DirectBufferSink : public Sink
	{
		static_assert(std::is_base_of_v<Bind::BufferResource, T>, "DirectBufferSink target type must be a BufferResource type");
	public:
		static std::unique_ptr<Sink> Make(std::string registeredName, std::shared_ptr<T>& target)
		{
			return std::make_unique<DirectBufferSink>(std::move(registeredName), target);
		}
		void PostLinkValidate() const override
		{
			if (!m_bLinked)
			{
				throw RGC_EXCEPTION("Unlinked input: " + GetRegisteredName());
			}
		}
		void Bind(Source& source) override
		{
			auto p = std::dynamic_pointer_cast<T>(source.YieldBuffer());
			if (!p)
			{
				std::ostringstream oss;
				oss << "Binding input [" << GetRegisteredName() << "] to output [" << GetPassName() << "." << GetOutputName() << "] "
					<< " { " << typeid(T).name() << " } not compatible with { " << typeid(*source.YieldBuffer().get()).name() << " }";
				throw RGC_EXCEPTION(oss.str());
			}
			m_Target = std::move(p);
			m_bLinked = true;
		}
		DirectBufferSink(std::string registeredName, std::shared_ptr<T>& bind)
			:
			Sink(std::move(registeredName)),
			m_Target(bind)
		{}
	private:
		std::shared_ptr<T>& m_Target;
		bool m_bLinked = false;
	};

	template<typename T>
	class ContainerBindableSink : public Sink
	{
		static_assert(std::is_base_of_v<Bind::Bindable, T>, "DirectBindableSink target type must be a Bindable type");
	public:
		void PostLinkValidate() const override
		{
			if (!m_bLinked)
			{
				throw RGC_EXCEPTION("Unlinked input: " + GetRegisteredName());
			}
		}
		void Bind(Source& source) override
		{
			auto p = std::dynamic_pointer_cast<T>(source.YieldBindable());
			if (!p)
			{
				std::ostringstream oss;
				oss << "Binding input [" << GetRegisteredName() << "] to output [" << GetPassName() << "." << GetOutputName() << "] "
					<< " { " << typeid(T).name() << " } does not match { " << typeid(*source.YieldBindable().get()).name() << " }";
				throw RGC_EXCEPTION(oss.str());
			}
			m_Container[m_Index] = std::move(p);
			m_bLinked = true;
		}
		ContainerBindableSink(std::string registeredName, std::vector<std::shared_ptr<Bind::Bindable>>& container, size_t index)
			:
			Sink(std::move(registeredName)),
			m_Container(container),
			m_Index(index)
		{}
	private:
		std::vector<std::shared_ptr<Bind::Bindable>>& m_Container;
		size_t m_Index;
		bool m_bLinked = false;
	};

	template<class T>
	class DirectBindableSink : public Sink
	{
		static_assert(std::is_base_of_v<Bind::Bindable, T>, "DirectBindableSink target type must be a Bindable type");
	public:
		static std::unique_ptr<Sink> Make(std::string registeredName, std::shared_ptr<T>& target)
		{
			return std::make_unique<DirectBindableSink>(std::move(registeredName), target);
		}
		void PostLinkValidate() const override
		{
			if (!m_bLinked)
			{
				throw RGC_EXCEPTION("Unlinked input: " + GetRegisteredName());
			}
		}
		void Bind(Source& source) override
		{
			auto p = std::dynamic_pointer_cast<T>(source.YieldBindable());
			if (!p)
			{
				std::ostringstream oss;
				oss << "Binding input [" << GetRegisteredName() << "] to output [" << GetPassName() << "." << GetOutputName() << "] "
					<< " { " << typeid(T).name() << " } does not match { " << typeid(*source.YieldBindable().get()).name() << " }";
				throw RGC_EXCEPTION(oss.str());
			}
			m_Target = std::move(p);
			m_bLinked = true;
		}
		DirectBindableSink(std::string registeredName, std::shared_ptr<T>& target)
			:
			Sink(std::move(registeredName)),
			m_Target(target)
		{}
	private:
		std::shared_ptr<T>& m_Target;
		bool m_bLinked = false;
	};
}