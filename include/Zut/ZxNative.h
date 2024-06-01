#pragma once
#include <Zut/ZxDefine.h>
#include <span>


namespace Zqf::Zut::ZxNative::Sys
{
	auto InitConsoleEncoding() -> void;
}


namespace Zqf::Zut::ZxNative::Str
{
	// cvt multibyte-string to u16-string with warning log
	auto CvtSafe(const std::string_view msStr, const CodePage eCodePage) -> U16Str_t;

	// cvt u16-string to multibyte-string with warning log
	auto CvtSafe(const std::u16string_view u16Str, const CodePage eCodePage) -> U8Str_t;

	// cvt multibyte-string to u16-string without warning log
	auto CvtForce(const std::string_view msStr, const CodePage eCodePage) -> U16Str_t;

	// cvt u16-string to multibyte-string without warning log
	auto CvtForce(const std::u16string_view u16Str, const CodePage eCodePage) -> U8Str_t;

	// cvt multibyte-string to u16-string use a existing buffer, without warning log
	auto CvtForce(const std::string_view msStr, std::span<char16_t> spBuffer, const CodePage eCodePage) -> std::u16string_view;

	// cvt u16-string to multibyte-string use a existing buffer, without warning log
	auto CvtForce(const std::u16string_view u16Str, std::span<char> spBuffer, const CodePage eCodePage) -> std::string_view;

	// compare substrings without case sensitivity
	auto Cmpni(const std::string_view msStr0, const std::string_view msStr1, size_t nMaxCount) -> size_t;
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
	auto Write(SysID hFile, const std::span<const uint8_t> spBuffer) -> std::optional<size_t>;
}


namespace Zqf::Zut::ZxNative::Fs
{
	auto WalkCreate(const std::string_view msPath) -> std::optional<SysID>;
	auto WalkClose(SysID hFind) -> bool;
	auto WalkNextDir(SysID hFind, std::span<char> spBuffer) -> std::optional<std::string_view>;
	auto WalkNextFile(SysID hFind, std::span<char> spBuffer) -> std::optional<std::string_view>;

	auto SelfDir() -> U8Str_t;
	auto SelfPath() -> U8Str_t;

	auto Exist(const std::string_view msPath) -> bool;

	auto MakeDir(const std::string_view msPath) -> bool;
	auto MakeDirs(const std::string_view msPath) -> bool;

	auto RemoveFile(const std::string_view msPath) -> bool;
	auto CopyFile(const std::string_view msExistPath, const std::string_view msNewPath, bool isFailIfExists) -> bool;

	auto GetFileSize(const std::string_view msPath) -> std::optional<uint64_t>;
}