#pragma once
#include <Zut/ZxDefine.h>
#include <span>


namespace Zqf::Zut::ZxNative::Sys
{
auto InitConsoleEncoding() -> void;
}

namespace Zqf::Zut::ZxNative::Str
{
// cvt multibyte-string to wide-string without warning log
auto CvtForce(const std::string_view msStr, const CodePage eCodePage) -> WideStr_t;

// cvt wide-string to multibyte-string without warning log
auto CvtForce(const std::wstring_view wsStr, const CodePage eCodePage) -> MbcsStr_t;

// cvt multibyte-string to wide-string use a existing buffer, without warning log
auto CvtForce(const std::string_view msStr, std::span<wchar_t> spBuffer, const CodePage eCodePage) -> std::wstring_view;

// cvt wide-string to multibyte-string use a existing buffer, without warning log
auto CvtForce(const std::wstring_view wsStr, std::span<char> spBuffer, const CodePage eCodePage) -> std::string_view;

// cvt multibyte-string to wide-string with warning log
auto CvtSafe(const std::string_view msStr, const CodePage eCodePage) -> WideStr_t;

// cvt wide-string to multibyte-string with warning log
auto CvtSafe(const std::wstring_view wsStr, const CodePage eCodePage) -> MbcsStr_t;
}


namespace Zqf::Zut::ZxNative::File
{
// open file via utf8 path
auto Open(const std::string_view msPath, const OpenMod eMode) -> std::optional<SysID>;

// close file
auto Close(SysID hFile) -> bool;

// flush file
auto Flush(SysID hFile) -> bool;

// get file size
auto GetSize(SysID hFile) -> std::optional<uint64_t>;

// get current file pointer
auto GetPtr(SysID hFile) -> std::optional<uint64_t>;

// set file pointer
auto SetPtr(SysID hFile, uint64_t nOffset, MoveWay eWay) -> std::optional<uint64_t>;

// read data from file
auto Read(SysID hFile, const std::span<uint8_t> spBuffer) -> std::optional<size_t>;

// write data to file
auto Write(SysID hFile, const std::span<uint8_t> spBuffer) -> std::optional<size_t>;
} // namespace Dut::Native::File


namespace Zqf::Zut::ZxNative::Fs
{
auto WalkCreate(const std::string_view msPath) -> std::optional<SysID>;
auto WalkClose(SysID hFind) -> bool;
auto WalkNextDir(SysID hFind, std::span<char> spBuffer) -> std::optional<std::string_view>;
auto WalkNextFile(SysID hFind, std::span<char> spBuffer) -> std::optional<std::string_view>;

auto SelfDir() -> MbcsStr_t;
auto SelfPath() -> MbcsStr_t;

auto Exist(const std::string_view msPath) -> bool;

auto MakeDir(const std::string_view msPath) -> bool;
auto MakeDirs(const std::string_view msPath) -> bool;

auto RemoveFile(const std::string_view msPath) -> bool;
auto CopyFile(const std::string_view msExistPath, const std::string_view msNewPath, bool isFailIfExists) -> bool;

auto GetFileSize(const std::string_view msPath) -> std::optional<uint64_t>;
}