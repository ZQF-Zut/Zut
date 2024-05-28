#pragma once
#include <cstdint>
#include <span>
#include <stdexcept>


namespace Zqf::Zut::ZxView
{
	class View_Basic
	{
	protected:
		size_t m_nPos = 0;
		uint8_t* m_pData = nullptr;

	protected:
		View_Basic()
		{

		}

		View_Basic(const void* pData) : m_pData{ reinterpret_cast<uint8_t*>(const_cast<void*>(pData)) }
		{

		}

	public:
		template<class T = uint8_t*>
		auto Ptr() const noexcept -> T
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return reinterpret_cast<T>(m_pData);
			}
			else
			{
				static_assert(true, "ZxView::View_Basic::Ptr: not pointer type!");
			}
		}

		template<class T = uint8_t*>
		auto CurPtr() const noexcept -> T
		{
			return this->Ptr<T>() + m_nPos;
		}

		auto ResetPos() -> void
		{
			m_nPos = 0;
		}

		auto IncPos(size_t nBytes) -> void
		{
			m_nPos += nBytes;
		}

		auto SetPos(size_t nBytes) -> void
		{
			m_nPos = nBytes;
		}

		auto GetPos() const -> size_t
		{
			return m_nPos;
		}
	};

	class Reader : public View_Basic
	{
	public:
		Reader()
		{

		}

		Reader(const void* pData) : View_Basic(pData)
		{

		}

		template <class T, size_t S>
		auto Read(const std::span<T, S> spData) -> void
		{
			std::memcpy(spData.data(), this->CurPtr(), spData.size_bytes());
			m_nPos += spData.size_bytes();
		}

		template<class T>
		Reader& operator>>(T& rfData)
		{
			using T_decay = std::decay_t<decltype(rfData)>;

			if constexpr (std::is_bounded_array_v<T>)
			{
				this->Read(std::span{ rfData });
			}
			else if constexpr (std::is_integral_v<T_decay>)
			{
				this->Read(std::span{ &rfData,1 });
			}
			else
			{
				static_assert(true, "ZxView::Reader::operator>>: not match type");
			}

			return *this;
		}

		template<class T>
		T Get()
		{
			T tmp;
			this->Read(std::span{ &tmp,1 });
			return tmp;
		}
	};
}
