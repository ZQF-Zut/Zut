#include <Zut/ZxFile.h>
#include <Zut/ZxFs.h>
#include <Zut/ZxStr.h>
#include <Zut/ZxSys.h>
#include <Zut/ZxJson.h>
#include <Zut/ZxBench.h>
#include <format>
#include <iostream>
#include <print>

using namespace Zqf;


static auto BenchJsonParser() -> void
{
	try
	{
		Zut::ZxRecord record;

		Zut::ZxMem jmem("1.json");
		for (size_t i = 0; i < 200; i++)
		{
			record.Beg();
			Zut::ZxJson::JValue jv;
			Zut::ZxJson::JParser{ { jmem.Ptr<char*>(), jmem.SizeBytes<size_t>() } }.Parse(jv);
			record.End();
		}

		record.Log();
	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "std::exception: {}", err.what());
	}
}

static auto BenchJsonDumper() -> void
{
	Zut::ZxJson::JDoc jdoc("1.json");
	auto& jv = jdoc.GetJValue();

	Zut::ZxRecord record;

	for (size_t i = 0; i < 200; i++)
	{
		record.Beg();
		std::string dumpx;
		jv.Dump(dumpx, true, 0);
		std::cout << dumpx;
		record.End();
	}

	record.Log();
}

static auto TestJsonParseRegularEscape() -> bool
{
	constexpr std::string_view str0 = R"JSON("123\n666\r4565\tefwe\"fawfw\\afjasf\bsafasf\fawfasf\fasf\tFDaf\\123")JSON";
	constexpr std::string_view str1 = "123\n666\r4565\tefwe\"fawfw\\afjasf\bsafasf\fawfasf\fasf\tFDaf\\123";
	Zut::ZxJson::JValue jv;
	Zut::ZxJson::JParser{ str0 }.Parse(jv);

	if (jv.Get<std::string_view>() != str1)
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
	return jv.Get<std::string_view>() == str1;
}

static auto TestZxMemIO() -> void
{
	{
		std::array<uint16_t, 2> ee = { 131,66 };
		Zut::ZxMem mem{4};
		mem.Write(std::span{ ee });
		int a = 0;

		mem.SetPos<Zut::MoveWay::Cur>(-4);

		uint16_t x0 = mem.Get<uint16_t>();
		uint16_t x1 = mem.Get<uint16_t>();
	}


	Zut::ZxMem mem(10);
	mem.CurPtr<uint8_t*>();

	uint32_t tmp_val0 = 222;
	double_t tmp_val1 = 111.11;
	std::array<uint32_t, 3> tmp_array{10086, 112233, 666};
	mem << tmp_val0;
	mem << tmp_val1;
	mem << 100;
	mem << 1.22;
	mem << tmp_array;

	size_t tmp_val2 = 777;
	uint8_t tmp_val3 = 0xF;
	uint16_t tmp_val4 = 0xFF;
	mem.Put(tmp_val2).Put(tmp_val3).Put(tmp_val4);

	mem.SetPos<Zut::MoveWay::Beg>();

	assert(tmp_val0 == mem.Get<uint32_t>());
	assert(tmp_val1 == mem.Get<double_t>());
	assert(100 == mem.Get<uint32_t>());
	assert(1.22 == mem.Get<double_t>());
	auto temp_array_get = mem.Get<std::array<uint32_t, 3>>();
	assert(tmp_array == temp_array_get);

	size_t tmp_val2_r{};
	mem >> tmp_val2_r;
	assert(tmp_val2 == tmp_val2_r);
	uint8_t tmp_val3_r{};
	mem >> tmp_val3_r;
	assert(tmp_val3 == tmp_val3_r);
	uint16_t tmp_val4_r{};
	mem >> tmp_val4_r;
	assert(tmp_val4 == tmp_val4_r);

	int a = 0;
}


auto main() -> int
{
	Zut::ZxNative::Sys::InitConsoleEncoding();
	try
	{
		TestZxMemIO();
		// JsonBench();
		// TestZxMem();
	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "std::exception: {}", err.what());
	}

}