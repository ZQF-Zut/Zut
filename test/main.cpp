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

auto main() -> int
{
    Zut::ZxNative::Sys::InitConsoleEncoding();
    try
    {
        Zut::ZxRecord record;
        Zut::ZxMem jmem("C:/Users/Hex/Downloads/ejson4cpp-master/tests/json/test.json");
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