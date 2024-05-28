#include <Zut/ZxFile.h>
#include <Zut/ZxFs.h>
#include <Zut/ZxStr.h>
#include <Zut/ZxSys.h>
#include <Zut/ZxJson.h>
#include <Zut/ZxBench.h>
#include <Zut/ZxView.h>
#include <format>
#include <iostream>
#include <print>

using namespace Zqf;


static auto JsonBench() -> void
{
	try
	{
		Zut::ZxRecord record;

		Zut::ZxMem jmem("1.json");
		for (size_t i = 0; i < 200; i++)
		{
			record.Beg();
			Zut::ZxJson::JValue jv;
			Zut::ZxJson::JParser{ { jmem.Ptr<char*>(), jmem.Size<size_t>() } }.Parse(jv);
			record.End();
		}

		record.Log();
	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "std::exception: {}", err.what());
	}
}


static auto TestJsonParseRegularEscape() -> bool
{
	constexpr std::string_view str0 = R"JSON("123\n666\r4565\tefwe\"fawfw\\afjasf\bsafasf\fawfasf\fasf\tFDaf\\123")JSON";
	constexpr std::string_view str1 = "123\n666\r4565\tefwe\"fawfw\\afjasf\bsafasf\fawfasf\fasf\tFDaf\\123";
	Zut::ZxJson::JValue jv;
	Zut::ZxJson::JParser{ str0 }.Parse(jv);

	if (jv.Get<std::string>() != str1)
	{
		return false;
	}

	// test unknown escape
	try
	{
		constexpr std::string_view str2 = R"JSON("iurbguiwe\x14141")JSON";
		Zut::ZxJson::JValue jv1;
		Zut::ZxJson::JParser{ str2 }.Parse(jv1);
		return false;
	}
	catch (const std::exception& err)
	{
		if (::strcmp(err.what(),"ZxJson::JParser::ParseString: unknown escape character!"))
		{
			return false;
		}
	}

	return true;
}

static auto TestJsonParseUnicodeEscape() -> bool
{
	constexpr std::string_view str0 = R"JSON("\u5FAE\u79ef\u5206\u57fa\u672c\u5b9a\u7406\uff08Fundamental Theorem of Calculus\uff09\u53c8\u79f0\u5fae\u79ef\u5206\u57fa\u672c\u516c\u5f0f\uff0c\u8bc1\u5b9e\u5fae\u5206\u548c\u79ef\u5206\u4e92\u4e3a\u9006\u8fd0\u7b97")JSON";
	constexpr std::string_view str1 = R"JSON(微积分基本定理（Fundamental Theorem of Calculus）又称微积分基本公式，证实微分和积分互为逆运算)JSON";
	
	Zut::ZxJson::JValue jv;
	Zut::ZxJson::JParser{ str0 }.Parse(jv);
	return jv.Get<std::string>() == str1;
}

auto main() -> int
{
	Zut::ZxNative::Sys::InitConsoleEncoding();
	try
	{
		const std::array<uint16_t, 2> ee = { 131,66 };
		Zut::ZxView::Reader reader = ee.data();
		auto xx = reader.Get<uint16_t>();
		JsonBench();

	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "std::exception: {}", err.what());
	}

}