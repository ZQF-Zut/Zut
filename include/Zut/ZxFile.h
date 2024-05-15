#pragma once
#include <Zut/ZxNative.h>
#include <format>
#include <stdexcept>

namespace Zqf::Zut
{
class ZxFile
{
  private:
    SysID m_hFile{};

  public:
    ZxFile(const std::string_view msPath, const OpenMod eMode)
    {
        if (const auto file_id_opt = Zut::ZxNative::File::Open(msPath, eMode))
        {
            m_hFile = *file_id_opt;
            return;
        }

        throw std::runtime_error(std::format("DxFile: open {} failed!", msPath));
    }

    ~ZxFile()
    {
        this->Close();
    }

    auto Flush() -> bool
    {
        return Zut::ZxNative::File::Flush(m_hFile);
    }

    auto Close() -> bool
    {
        return Zut::ZxNative::File::Close(m_hFile);
    }

    template <class T, size_t S>
    auto Read(const std::span<T, S> spBuffer) -> std::optional<size_t>
    {
        return Zut::ZxNative::File::Read(m_hFile, { reinterpret_cast<uint8_t*>(spBuffer.data()), spBuffer.size_bytes() });
    }

    template <class T, size_t S>
    auto Write(const std::span<T, S> spBuffer) -> std::optional<size_t>
    {
        return Zut::ZxNative::File::Write(m_hFile, { reinterpret_cast<const uint8_t*>(spBuffer.data()), spBuffer.size_bytes() });
    }

    auto SetPtr(const uint64_t nOffset, const MoveWay eWay) -> std::optional<uint64_t>
    {
        return Zut::ZxNative::File::SetPtr(m_hFile, nOffset, eWay);
    }

    auto GetSize() -> std::optional<uint64_t>
    {
        return Zut::ZxNative::File::GetSize(m_hFile);
    }

    template <class T, size_t S>
    static auto SaveDataViaPath(const std::string_view msPath, const std::span<T, S> spData, bool isForceSave) -> void
    {
        ZxNative::Fs::MakeDirs(msPath);
        ZxFile{ msPath, isForceSave ? OpenMod::WriteForce : OpenMod::WriteSafe }.Write(spData);
    }
};

} // namespace Zqf::Zut