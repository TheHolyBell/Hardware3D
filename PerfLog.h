#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include "Timer.h"

class PerfLog
{
	struct Entry
	{
		std::string label;
		float time;
		Entry(std::string s, float t)
			: label(std::move(s)), time(t){}
		void WriteTo(std::ostream& out) const noexcept
		{
			if (label.empty())
				out << time * 1000.0f << "ms\n";
			else
				out << "[" << label << "] " << time * 1000.0f << "ms\n";
		}
	};
	static PerfLog& Get_() noexcept
	{
		static PerfLog _Log;
		return _Log;
	}
	PerfLog() noexcept
	{
		m_Entries.reserve(3000);
	}
	~PerfLog()
	{
		Flush_();
	}
	void Start_(const std::string& label = "") noexcept
	{
		m_Entries.emplace_back(label, 0.0f);
		m_Timer.Mark();
	}
	void Mark_(const std::string& label = "") noexcept
	{
		float t = m_Timer.Peek();
		m_Entries.emplace_back(label, t);
	}
	void Flush_()
	{
		std::ofstream file("perf.txt");
		file << std::setprecision(3) << std::fixed;
		for (const auto& e : m_Entries)
			e.WriteTo(file);
	}

public:
	static void Start(const std::string& label = "") noexcept
	{
		Get_().Start_(label);
	}
	static void Mark(const std::string& label = "") noexcept
	{
		Get_().Mark_(label);
	}
private:
	Timer m_Timer;
	std::vector<Entry> m_Entries;
};