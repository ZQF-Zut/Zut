#pragma once
#include <chrono>


namespace Zqf::Zut
{
	class ZxRecord
	{
	private:
		std::chrono::steady_clock::time_point m_tpBeg;
		std::chrono::steady_clock::time_point m_tpEnd;
		std::vector<std::chrono::duration<double, std::milli>> m_vcRecord;

	public:
		auto Beg() -> void;
		auto End() -> void;
		auto Log() -> void;
	};
}