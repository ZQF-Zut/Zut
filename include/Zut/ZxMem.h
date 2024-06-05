#pragma once
#include <Zut/ZxFile.h>
#include <cassert>
#include <memory>
#include <string>


namespace Zqf::Zut
{
	constexpr size_t AUTO_MEM_AUTO_SIZE = -1;

	class ZxMem
	{
	private:
		size_t m_nPos{};
		size_t m_nSizeBytes{};
		std::unique_ptr<uint8_t[]> m_upMemData;

	public:
		ZxMem()
		{

		}

		~ZxMem()
		{

		}

		ZxMem(size_t nSize)
		{
			this->Resize(nSize, true);
		}

		ZxMem(const ZxMem& rfOBJ)
		{
			this->operator=(rfOBJ);
		}

		ZxMem(ZxMem&& rfOBJ) noexcept
		{
			this->operator=(std::move(rfOBJ));
		}

		ZxMem(const std::string_view msPath, size_t nLoadSize = AUTO_MEM_AUTO_SIZE)
		{
			this->Load(msPath, nLoadSize);
		}

		auto operator=(const ZxMem& rfOBJ) -> ZxMem&
		{
			assert(this != &rfOBJ);

			m_nPos = rfOBJ.m_nPos;
			m_nSizeBytes = rfOBJ.m_nSizeBytes;
			if (rfOBJ.m_upMemData != nullptr)
			{
				m_upMemData = std::make_unique_for_overwrite<uint8_t[]>(rfOBJ.m_nSizeBytes);
				::memcpy(m_upMemData.get(), rfOBJ.m_upMemData.get(), m_nSizeBytes);
			}

			return *this;
		}

		auto operator=(ZxMem&& rfOBJ) noexcept -> ZxMem&
		{
			assert(this != &rfOBJ);

			m_nPos = rfOBJ.m_nPos;
			m_nSizeBytes = rfOBJ.m_nSizeBytes;
			m_upMemData = std::move(rfOBJ.m_upMemData);

			rfOBJ.m_nPos = 0;
			rfOBJ.m_nSizeBytes = 0;

			return *this;
		}

	public:
		template <class T = uint8_t*>
		auto Ptr() const noexcept -> T
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return reinterpret_cast<T>(m_upMemData.get());
			}
			else
			{
				static_assert(false, "ZxMem::Ptr<T>(): not pointer type!");
			}
		}

		template<class T = uint8_t*>
		auto PtrCur() const noexcept -> T
		{
			assert(m_nPos <= m_nSizeBytes);
			if constexpr (std::is_pointer_v<T>)
			{
				return reinterpret_cast<T>(this->Ptr<uint8_t*>() + m_nPos);
			}
			else
			{
				static_assert(false, "ZxMem::CurPtr<T>(): not pointer type!");
			}
		}

		template <class T = uint8_t>
		auto Span() const noexcept -> std::span<T>
		{
			return std::span{ this->Ptr<T*>(), this->SizeBytes() / sizeof(T) };
		}

		template <class T = size_t>
		auto Pos() const -> T
		{
			if constexpr (std::is_integral_v<T>)
			{
				return static_cast<T>(m_nPos);
			}
			else
			{
				static_assert(false, "ZxMem::Pos<T>(): not integral type!");
			}
		}

		template <Zut::MoveWay eWay = Zut::MoveWay::Beg>
		auto PosSet(size_t nBytes = 0) -> ZxMem&
		{
			if constexpr (eWay == Zut::MoveWay::Beg)
			{
				m_nPos = nBytes;
			}
			else if constexpr (eWay == Zut::MoveWay::Cur)
			{
				m_nPos += nBytes;
			}
			else if constexpr (eWay == Zut::MoveWay::End)
			{
				m_nPos = m_nSizeBytes;
				m_nPos += nBytes;
			}

			if (m_nPos > m_nSizeBytes)
			{
				throw std::runtime_error("ZxMem::SetPos: out of size!");
			}

			return *this;
		}

		auto PosInc(size_t nBytes) -> ZxMem&
		{
			return this->PosSet<Zut::MoveWay::Cur>(nBytes);
		}

		template <class T = size_t>
		auto SizeBytes() const noexcept -> T
		{
			if constexpr (std::is_integral_v<T>)
			{
				return static_cast<T>(m_nSizeBytes);
			}
			else
			{
				static_assert(false, "ZxMem::SizeBytes<T>(): not integral type!");
			}
		}

		auto Resize(size_t nNewSizeBytes, bool isDiscard = false) -> ZxMem&
		{
			if (m_upMemData == nullptr)
			{
				m_upMemData = std::make_unique_for_overwrite<uint8_t[]>(nNewSizeBytes);
			}
			else if (m_nSizeBytes < nNewSizeBytes)
			{
				auto tmp = std::make_unique_for_overwrite<uint8_t[]>(nNewSizeBytes);
				if (isDiscard == false)
				{
					::memcpy(tmp.get(), m_upMemData.get(), m_nSizeBytes);
				}
				m_upMemData = std::move(tmp);
			}

			m_nSizeBytes = nNewSizeBytes;
			return *this;
		}

		template <class T, size_t S>
		auto Read(const std::span<T, S> spData) -> void
		{
			if (spData.empty()) { return; }
			std::memcpy(spData.data(), this->PtrCur<uint8_t*>(), spData.size_bytes());
			this->PosSet<Zut::MoveWay::Cur>(spData.size_bytes());
		}

		template <class T, size_t S>
		auto Write(const std::span<T, S> spData) -> void
		{
			if (spData.empty()) { return; }
			std::memcpy(this->PtrCur<uint8_t*>(), spData.data(), spData.size_bytes());
			this->PosSet<Zut::MoveWay::Cur>(spData.size_bytes());
		}

		template<class T>
		ZxMem& operator>>(T& rfData)
		{
			using T_decay = std::decay_t<decltype(rfData)>;

			if constexpr (std::is_integral_v<T_decay> || std::is_floating_point_v<T_decay>)
			{
				this->Read(std::span{ &rfData, 1 });
			}
			else
			{
				this->Read(std::span{ rfData });
			}

			return *this;
		}

		template<class T>
		ZxMem& operator<<(T&& rfData)
		{
			using T_decay = std::decay_t<decltype(rfData)>;

			if constexpr (std::is_integral_v<T_decay> || std::is_floating_point_v<T_decay>)
			{
				this->Write(std::span{ &rfData, 1 });
			}
			else
			{
				this->Write(std::span{ rfData });
			}

			return *this;
		}

		template <class T>
		auto Get() -> T
		{
			T tmp;
			this->operator>>(tmp);
			return tmp;
		}

		template <class T>
		auto Put(const T& rfData) -> ZxMem&
		{
			return this->operator<<(rfData);
		}

	public:
		auto Save(const std::string_view msPath, bool isForceSave) const -> const ZxMem&
		{
			ZxFile::SaveDataViaPath(msPath, this->Span<uint8_t>(), isForceSave);
			return *this;
		}

		auto Load(const std::string_view msPath, size_t nSize = AUTO_MEM_AUTO_SIZE) -> ZxMem&
		{
			ZxFile ifs{ msPath, OpenMod::ReadSafe };

			size_t read_size_bytes{};
			if (const auto file_size_opt = ifs.GetSize())
			{
				const auto file_size = *file_size_opt;
				if (nSize == AUTO_MEM_AUTO_SIZE)
				{
					read_size_bytes = static_cast<size_t>(file_size);
				}
				else
				{
					if (nSize <= file_size)
					{
						read_size_bytes = nSize;
					}
					else
					{
						throw std::runtime_error(std::format("ZxMem::Load: read size larger than file size!, msPath: {}", msPath));
					}
				}
			}
			else
			{
				throw std::runtime_error(std::format("ZxMem::Load: get file size error!, msPath: {}", msPath));
			}

			ifs.Read(std::span{ this->Resize(read_size_bytes, true).Span<uint8_t>() });
			return *this;
		}
	};

}