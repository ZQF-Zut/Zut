#pragma once
#include <Zut/ZxNative.h>

namespace Zqf::Zut::ZxFs
{
class Walk
{
  private:
    SysID m_hFind;
    char m_aSearchDir[ZUT_MAX_PATH];
    char m_aFindBuffer[ZUT_MAX_PATH];
    std::string_view m_msSearchDir;
    std::string_view m_msFindName;

  public:
    Walk(const std::string_view msWalkDir);
    ~Walk();

  public:
    auto GetPath() -> std::string_view;
    auto GetName() const -> std::string_view;

  public:
    auto NextDir() -> bool;
    auto NextFile() -> bool;
    auto IsSuffix(const std::string_view u8Suffix) const -> bool;
};

auto Suffix(const std::string_view msPath) -> std::string_view;
auto FileName(const std::string_view msPath) -> std::string_view;
using namespace Zut::ZxNative::Fs;
}