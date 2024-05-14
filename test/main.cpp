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
    //for (Zut::ZxFs::Walk walk("D:/VM/WindowsXP_JP/"); walk.NextFile();)
    //{
    //    if (!walk.IsSuffix(".scoreboard")) { continue; }
    //    std::println("路径: {}", walk.GetPath());
    //    std::println("名称: {}", walk.GetName());
    //}

    //std::println("file_name: {}", Zut::ZxFs::FileName("D:/VM/WindowsXP_JP/1.jpg"));

    try
    {
        //Zut::ZxSys::InitConsoleEncoding();

        Rut::RxBench::Record record;
        
        for (size_t i = 0; i < 1; i++)
        {
            record.Beg();
            Zut::ZxJson::JDoc doc;
            auto& jv = doc.GetJObject();
            Zut::ZxJson::JArray_t jarr;
            jarr.push_back("123");
            jarr.push_back(123);
            jarr.push_back(31.313);
            jv["hash"] = std::move(jarr);
            auto& arrx = jv["hash"].Get<Zut::ZxJson::JArray_t>();
            arrx.push_back(31.31);
            doc.Save("33.json", true, false);
            record.End();
        }
        
        record.Log();

        
        //std::println("当前程序目录：{}", Zut::ZxFs::SelfDir().first);
        //std::println("当前程序路径：{}", Zut::ZxFs::SelfPath().first);
        //Zut::ZxFs::MakeDirs("这是/我的/一个/个人/路径/123.jpg/");
    }
    catch (const std::runtime_error& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}