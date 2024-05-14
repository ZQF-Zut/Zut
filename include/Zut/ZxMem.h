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
        this->LoadData(msPath, nLoadSize);
    }

    ZxMem(const ZxMem& buffer)
    {
        this->Copy(buffer);
    }

    ZxMem(ZxMem&& buffer) noexcept
    {
        this->Move(std::move(buffer));
    }

    ZxMem& operator=(const ZxMem& rfAutoMem)
    {
        return this->Copy(rfAutoMem);
    }

    ZxMem& operator=(ZxMem&& rfAutoMem) noexcept
    {
        return this->Move(std::move(rfAutoMem));
    }

    ZxMem& operator+(const ZxMem& rfAutoMem)
    {
        return this->Append(rfAutoMem);
        ;
    }

    ZxMem& Copy(const ZxMem& rfObj)
    {
        assert(this != &rfObj);

        if (rfObj.m_upMemData != nullptr)
        {
            this->m_uiMaxSize = rfObj.m_uiMaxSize;
            this->m_uiMemSize = rfObj.m_uiMemSize;
            this->m_upMemData = std::make_unique_for_overwrite<uint8_t[]>(rfObj.m_uiMemSize);
            ::memcpy(m_upMemData.get(), rfObj.m_upMemData.get(), m_uiMemSize);
        }
        else
        {
            this->m_uiMaxSize = 0;
            this->m_uiMemSize = 0;
        }

        return *this;
    }

    ZxMem& Move(ZxMem&& buffer) noexcept
    {
        assert(this != &buffer);

        m_upMemData = std::move(buffer.m_upMemData);

        m_uiMemSize = buffer.m_uiMemSize;
        m_uiMaxSize = buffer.m_uiMaxSize;

        buffer.m_uiMemSize = 0;
        buffer.m_uiMaxSize = 0;

        return *this;
    }

    ZxMem& Append(const ZxMem& rfMem)
    {
        size_t cur_size = this->Size();
        size_t append_size = rfMem.Size();

        if (append_size)
        {
            this->Resize(cur_size + append_size, true);
            ::memcpy(this->Ptr() + cur_size, rfMem.Ptr(), append_size);
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
        return this->Ptr() + this->Size();
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
            static_assert(true, "DxMem::Ptr: not pointer type!");
        }
    }

    template <class T = size_t>
    auto Size() const noexcept -> T
    {
        if constexpr (std::is_integral_v<T>)
        {
            return static_cast<T>(m_uiMemSize);
        }
        else
        {
            static_assert(true, "DxMem::Size: not integral type!");
        }
    }

  public:
    ZxMem& SaveData(const std::string_view msPath, bool isForceSave)
    {
        ZxFile::SaveDataViaPath(msPath, std::span{ *this }, isForceSave);
        return *this;
    }

    ZxMem& LoadData(const std::string_view msPath, size_t nSize = AUTO_MEM_AUTO_SIZE)
    {
        ZxFile ifs{ msPath, OpenMod::ReadSafe };
        size_t read_size = AUTO_MEM_AUTO_SIZE ? static_cast<size_t>(ifs.GetSize().value()) : nSize;
        ifs.Read(std::span{ this->Resize(read_size).Ptr(), this->Size() });
        return *this;
    }
};

} // namespace Dut