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
		size_t m_uiMaxSize{};
		size_t m_uiMemSize{};
		std::unique_ptr<uint8_t[]> m_upMemData;

	public:
		ZxMem() {}
		~ZxMem() {}

		ZxMem(size_t nSize)
		{
			this->Resize(nSize, false);
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

		ZxMem& operator=(const ZxMem& rfOBJ)
		{
			assert(this != &rfOBJ);

			if (rfOBJ.m_upMemData != nullptr)
			{
				this->m_uiMaxSize = rfOBJ.m_uiMaxSize;
				this->m_uiMemSize = rfOBJ.m_uiMemSize;
				this->m_upMemData = std::make_unique_for_overwrite<uint8_t[]>(rfOBJ.m_uiMemSize);
				::memcpy(m_upMemData.get(), rfOBJ.m_upMemData.get(), m_uiMemSize);
			}
			else
			{
				this->m_uiMaxSize = 0;
				this->m_uiMemSize = 0;
			}

			return *this;
		}

		ZxMem& operator=(ZxMem&& rfOBJ) noexcept
		{
			assert(this != &rfOBJ);

			m_upMemData = std::move(rfOBJ.m_upMemData);

			m_uiMemSize = rfOBJ.m_uiMemSize;
			m_uiMaxSize = rfOBJ.m_uiMaxSize;

			rfOBJ.m_uiMemSize = 0;
			rfOBJ.m_uiMaxSize = 0;

			return *this;
		}

		ZxMem& operator+(const ZxMem& rfOBJ)
		{
			size_t cur_size = this->SizeBytes();
			size_t append_size = rfOBJ.SizeBytes();

			if (append_size)
			{
				this->Resize(cur_size + append_size, true);
				::memcpy(this->Ptr() + cur_size, rfOBJ.Ptr(), append_size);
			}

			return *this;
		}

		ZxMem& Resize(size_t uiNewSize, bool isCopy = false)
		{
			if (m_uiMemSize == 0)
			{
				m_upMemData = std::make_unique_for_overwrite<uint8_t[]>(uiNewSize);
				m_uiMaxSize = uiNewSize;
			}
			else if (uiNewSize > m_uiMaxSize)
			{
				std::unique_ptr<uint8_t[]> tmp = std::make_unique_for_overwrite<uint8_t[]>(uiNewSize);
				if (isCopy) { ::memcpy(tmp.get(), m_upMemData.get(), m_uiMemSize); }
				m_upMemData = std::move(tmp);
				m_uiMaxSize = uiNewSize;
			}

			m_uiMemSize = uiNewSize;

			return *this;
		}

		uint8_t* begin() const noexcept
		{
			return this->Ptr();
		}

		uint8_t* end() const noexcept
		{
			return this->Ptr() + this->SizeBytes();
		}

		uint8_t operator[](size_t nIndex) noexcept
		{
			assert(nIndex < m_uiMemSize);
			return this->Ptr()[nIndex];
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
				static_assert(true, "ZxMem::Ptr: not pointer type!");
			}
		}

		template <class T = size_t>
		auto SizeBytes() const noexcept -> T
		{
			if constexpr (std::is_integral_v<T>)
			{
				return static_cast<T>(m_uiMemSize);
			}
			else
			{
				static_assert(true, "ZxMem::SizeBytes: not integral type!");
			}
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

			size_t read_size{};
			if (const auto file_size_opt = ifs.GetSize())
			{
				const auto file_size = *file_size_opt;
				if (nSize == AUTO_MEM_AUTO_SIZE)
				{
					read_size = static_cast<size_t>(file_size);
				}
				else
				{
					if (nSize <= file_size)
					{
						read_size = nSize;
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

			ifs.Read(std::span{ this->Resize(read_size).Ptr(), this->SizeBytes() });
			return *this;
		}

		template <class T, size_t S>
		auto Copy(std::span<T,S> spData) -> ZxMem&
		{
			this->Resize(spData.size_bytes(), false);
			std::memcpy(this->Ptr<uint8_t*>(), spData.data(), spData.size_bytes());
			return *this;
		}
	};

}