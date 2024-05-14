#pragma once

#ifdef ZUT_WIN32

#include <Wx32/Define.h>
#include <optional>

namespace Zqf::Zut
{

using SysID = HANDLE;

constexpr size_t ZUT_MAX_PATH = MAX_PATH;

enum class MoveWay
{
    Beg = FILE_BEGIN,
    Cur = FILE_CURRENT,
    End = FILE_END,
};

using WalkData_t = WIN32_FIND_DATAW;

enum class STDIOType
{
    Write = static_cast<int>(STD_OUTPUT_HANDLE),
    Read = static_cast<int>(STD_INPUT_HANDLE),
    Error = static_cast<int>(STD_ERROR_HANDLE),
};

} // namespace Zqf::Zut

#endif // ZUT_WIN32

namespace Zqf::Zut
{
using MbcsStr_t = Wx32::MbcsStr_t;
using WideStr_t = Wx32::WideStr_t;
using CodePage = Wx32::CodePage;

enum class OpenMod
{
    ReadSafe,
    WriteSafe,
    WriteForce
};
} // namespace Zqf::Zut
