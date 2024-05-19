#include <iostream>
#include <print>
#include <ranges>
#include <Zut/ZxBench.h>
#include <Zut/ZxJson.h>

using namespace Zqf;


auto main() -> int
{
	try
	{
		Zut::ZxNative::Sys::InitConsoleEncoding();

		Zut::ZxRecord record;
		for (auto i : std::views::iota(0,10))
		{
			record.Beg();
			Zut::ZxJson::JDoc doc("1.json");
			record.End();
		}
		record.Log();
	}
	catch (const std::exception& err)
	{
		std::println(std::cerr, "std::exception: {}", err.what());
	}
}