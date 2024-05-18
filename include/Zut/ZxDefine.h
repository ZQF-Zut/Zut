#pragma once

#ifdef ZUT_WIN32

#include <Wx32/Define.h>
#include <optional>

namespace Zqf::Zut
{
	constexpr size_t ZUT_MAX_PATH = MAX_PATH;

	using SysID = Wx32::SysID;
	using CodePage = Wx32::CodePage;
	using MoveWay = Wx32::MoveWayEnum;
	using STDHandleType = Wx32::STDHandleEnum;
	using MbcsStr_t = Wx32::MbcsStr_t;
	using WideStr_t = Wx32::WideStr_t;
	using WalkData_t = WIN32_FIND_DATAW;
} // namespace Zqf::Zut

#endif // ZUT_WIN32

namespace Zqf::Zut
{
	enum class OpenMod
	{
		ReadSafe,
		WriteSafe,
		WriteForce
	};
} // namespace Zqf::Zut
