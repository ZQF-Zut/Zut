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

            throw std::runtime_error(std::format("ZxFile: open file failed! -> msPath: {}", msPath));
        }

        ~ZxFile()
        {
            this->Close();
        }

        auto Flush() const -> bool
        {
            return Zut::ZxNative::File::Flush(m_hFile);
        }

        auto Close() const -> bool
        {
            return Zut::ZxNative::File::Close(m_hFile);
        }

        template <class T, size_t S>
        auto Read(const std::span<T, S> spBuffer) const -> std::optional<size_t>
        {
            return Zut::ZxNative::File::Read(m_hFile, { reinterpret_cast<uint8_t*>(spBuffer.data()), spBuffer.size_bytes() });
        }

        template <class T, size_t S>
        auto Write(const std::span<T, S> spBuffer) const -> std::optional<size_t>
        {
            return Zut::ZxNative::File::Write(m_hFile, { reinterpret_cast<const uint8_t*>(spBuffer.data()), spBuffer.size_bytes() });
        }

        auto SetPtr(const uint64_t nOffset, const MoveWay eWay) const -> std::optional<uint64_t>
        {
            return Zut::ZxNative::File::SetPtr(m_hFile, nOffset, eWay);
        }

        auto GetSize() const -> std::optional<uint64_t>
        {
            return Zut::ZxNative::File::GetSize(m_hFile);
        }

        template<class T>
        ZxFile& operator>>(T& rfData)
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
        ZxFile& operator<<(T&& rfData)
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
        auto Put(const T& rfData) -> ZxFile&
        {
            return this->operator<<(rfData);
        }

        template <class T, size_t S>
        static auto SaveDataViaPath(const std::string_view msPath, const std::span<T, S> spData, bool isForceSave) -> void
        {
            const auto pos = msPath.rfind('/');
            if (pos != std::string_view::npos)
            {
                std::string_view dir = msPath.substr(0, pos + 1);
                ZxNative::Fs::MakeDirs(dir);
            }

            ZxFile{ msPath, isForceSave ? OpenMod::WriteForce : OpenMod::WriteSafe }.Write(spData);
        }
    };

}
