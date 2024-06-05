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
		size_t m_nPosBytes{};
		size_t m_nSizeBytes{};
		size_t m_nCapacityBytes{};
		std::unique_ptr<uint8_t[]> m_upMemData;

	public:
		ZxMem() {}
		~ZxMem() {}

		ZxMem(size_t nSize)
		{
			this->ReserveBytes(nSize, false);
		}

		ZxMem(const std::string_view msPath, size_t nLoadSize = AUTO_MEM_AUTO_SIZE)
		{
			this->Load(msPath, nLoadSize);
		}

		ZxMem(const ZxMem& rfOBJ)
		{
			this->operator=(rfOBJ);
		}

		ZxMem(ZxMem&& rfOBJ) noexcept
		{
			this->operator=(std::move(rfOBJ));
		}

		auto operator=(const ZxMem& rfOBJ) -> ZxMem&
		{
			assert(this != &rfOBJ);

			m_nPosBytes = rfOBJ.m_nPosBytes;
			m_nSizeBytes = rfOBJ.m_nSizeBytes;
			m_nCapacityBytes = m_nSizeBytes;
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

			m_nPosBytes = rfOBJ.m_nPosBytes;
			m_nSizeBytes = rfOBJ.m_nSizeBytes;
			m_nCapacityBytes = rfOBJ.m_nSizeBytes;
			m_upMemData = std::move(rfOBJ.m_upMemData);

			rfOBJ.m_nPosBytes = 0;
			rfOBJ.m_nSizeBytes = 0;
			rfOBJ.m_nCapacityBytes = 0;

			return *this;
		}

		auto operator+(const ZxMem& rfOBJ) -> ZxMem&
		{
			this->Write(std::span{ rfOBJ });
			return *this;
		}

		auto operator[](const size_t nIndex) noexcept -> uint8_t
		{
			assert(nIndex < m_nSizeBytes);
			return this->Ptr<uint8_t*>()[nIndex];
		}

		auto begin() const noexcept -> uint8_t*
		{
			return this->Ptr<uint8_t*>();
		}

		auto end() const noexcept -> uint8_t*
		{
			return this->Ptr<uint8_t*>() + this->SizeBytes<size_t>();
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
		auto CurPtr() const noexcept -> T
		{
			assert(m_nPosBytes <= m_nSizeBytes);
			if constexpr (std::is_pointer_v<T>)
			{
				return (this->Ptr<uint8_t*>() + m_nPosBytes);
			}
			else
			{
				static_assert(false, "ZxMem::CurPtr<T>(): not pointer type!");
			}
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

		template <Zut::MoveWay eWay = Zut::MoveWay::Beg>
		auto SetPos(size_t nBytes = 0) noexcept -> size_t
		{
			if constexpr (eWay == Zut::MoveWay::Beg)
			{
				m_nPosBytes = nBytes;
			}
			else if constexpr (eWay == Zut::MoveWay::Cur)
			{
				m_nPosBytes += nBytes;
			}
			else if constexpr (eWay == Zut::MoveWay::End)
			{
				m_nPosBytes = m_nSizeBytes;
				m_nPosBytes += nBytes;
			}

			assert(m_nPosBytes <= m_nSizeBytes);
			return m_nPosBytes;
		}

		auto ReserveBytes(size_t nNewSizeBytes, bool isDiscard = false) -> ZxMem&
		{
			if (m_upMemData == nullptr)
			{
				m_nCapacityBytes = nNewSizeBytes;
				m_upMemData = std::make_unique_for_overwrite<uint8_t[]>(nNewSizeBytes);
			}
			else if (m_nCapacityBytes < nNewSizeBytes)
			{
				m_nCapacityBytes = nNewSizeBytes * 2;
				std::unique_ptr<uint8_t[]> tmp = std::make_unique_for_overwrite<uint8_t[]>(m_nCapacityBytes);
				if ((isDiscard == false) && (m_nSizeBytes != 0)) 
				{ 
					::memcpy(tmp.get(), m_upMemData.get(), m_nSizeBytes);
				}
				m_upMemData = std::move(tmp);
			}

			return *this;
		}

		auto ResizeBytes(size_t nNewSizeBytes, bool isDiscard = false) -> ZxMem&
		{
			this->ReserveBytes(nNewSizeBytes, isDiscard);
			m_nSizeBytes = nNewSizeBytes;
			return *this;
		}

		template <class T, size_t S>
		auto Read(const std::span<T, S> spData) noexcept -> void
		{
			if (spData.empty()) { return; }
			std::memcpy(spData.data(), this->CurPtr<uint8_t*>(), spData.size_bytes());
			this->SetPos<Zut::MoveWay::Cur>(spData.size_bytes());
		}

		template <class T, size_t S>
		auto Write(const std::span<T, S> spData) noexcept -> void
		{
			if (spData.empty()) { return; }
			this->ResizeBytes(this->SizeBytes() + spData.size_bytes(), false);
			std::memcpy(this->CurPtr<uint8_t*>(), spData.data(), spData.size_bytes());
			this->SetPos<Zut::MoveWay::Cur>(spData.size_bytes());
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
			ZxFile::SaveDataViaPath(msPath, std::span{ *this }, isForceSave);
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

			ifs.Read(std::span{ this->ResizeBytes(read_size_bytes, true).Ptr<uint8_t*>(), this->SizeBytes() });
			return *this;
		}
	};

}