#include <Zut/ZxBench.h>
#include <print>


namespace Zqf::Zut
{
	auto ZxRecord::Beg() -> void
	{
		m_tpBeg = std::chrono::steady_clock::now();
	}

	auto ZxRecord::End() -> void
	{
		m_tpEnd = std::chrono::steady_clock::now();
		m_vcRecord.emplace_back((m_tpEnd - m_tpBeg));
	}

	auto ZxRecord::Log() -> void
	{
		std::chrono::duration<double, std::milli> cout{};

		for (auto& dur : m_vcRecord)
		{
			cout += dur;
			std::println("{}", dur);
		}

		std::println("Avg:{}", cout / m_vcRecord.size());
	}
} // namespace Rut::RxBench