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
			static_assert(std::is_pointer_v<T>, "ZxView::View_Basic::Ptr: not pointer type!");
			return reinterpret_cast<T>(m_pData);
		}

		template<class T = uint8_t*>
		auto CurPtr() const noexcept -> T
		{
			return reinterpret_cast<T>(this->Ptr<uint8_t*>() + m_nPos);
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

		explicit Reader(const void* pData) : View_Basic(pData)
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
			if constexpr (std::is_integral_v<std::decay_t<decltype(rfData)>>)
			{
				this->Read(std::span{ &rfData,1 });
			}
			else
			{
				this->Read(std::span{ rfData });
			}

			return *this;
		}

		template<class T>
		auto Get() -> T
		{
			T tmp;
			this->operator>>(tmp);
			return tmp;
		}
	};
}
