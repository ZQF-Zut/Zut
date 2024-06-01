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
	using WalkData_t = WIN32_FIND_DATAW;
}

#endif // ZUT_WIN32


namespace Zqf::Zut
{
	using U8Str_t = std::pair<std::basic_string_view<char>, std::unique_ptr<char[]>>;
	using U16Str_t = std::pair<std::basic_string_view<char16_t>, std::unique_ptr<char16_t[]>>;

	enum class OpenMod
	{
		ReadSafe,
		WriteSafe,
		WriteForce
	};
}
