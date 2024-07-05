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
            Zut::ZxJson::JParser{ jmem.Span<char>() }.Parse(jv);
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

static auto TestJsonParser() -> void
{
    auto x = Zut::ZxJson::Load("1.json");
    Zut::ZxJson::JDoc jdoc("1.json");
}

static auto TestJsonValue() -> void
{
    Zut::ZxJson::JValue jv;
    jv = 1;
    assert(jv.Get<size_t>() == 1);
    jv = 1.2;
    assert(jv.Get<double>() == 1.2);
    jv = "123";
    assert(jv.Get<std::string_view>() == "123");
    jv = std::string("5666");
    assert(jv.Get<std::string>() == "5666");
    assert(jv.Get<std::string&>() == "5666");

    Zut::ZxJson::JValue jv1 = std::move(jv);
    assert(jv.Check<Zut::ZxJson::JNull_t>());
    assert(jv1.Get<std::string>() == "5666");
    assert(jv1.Get<std::string&>() == "5666");


    Zut::ZxJson::JArray_t jarr;
    jarr.emplace_back(1);
    jarr.emplace_back("12313");
    jarr.emplace_back(13.41);
    jarr.emplace_back(false);

    assert(jarr.size() == 4);
    assert(jarr[0].Get<size_t>() == 1);
    assert(jarr[1].Get<std::string_view>() == "12313");
    assert(jarr[2].Get<double>() == 13.41);
    assert(jarr[3].Get<bool>() == false);

    Zut::ZxJson::JObject_t jobj;
    jobj["532532"] = 1;
    jobj["666"] = 3.11;
    jobj["777"] = "121431";
    jobj["ftftfr"] = false;

    assert(jobj["532532"].Get<size_t>() == 1);
    assert(jobj["666"].Get<double>() == 3.11);
    assert(jobj["777"].Get<std::string_view>() == "121431");
    assert(jobj["ftftfr"].Get<bool>() == false);

    Zut::ZxJson::JArray_t jarr1 = std::move(jarr);
    assert(jarr.empty());

    jv = std::move(jarr1);
    assert(jarr1.empty());

    Zut::ZxJson::JObject_t jobj1 = std::move(jobj);
    jobj1["arrayx"] = std::move(jv);
    assert(jv.Check<Zut::ZxJson::JNull_t>());

    assert(jobj1["arrayx"][0].Get<size_t>() == 1);

    auto& jarr2 = jobj1["arrayx"].Get<Zut::ZxJson::JArray_t&>();
    assert(jarr2.size() == 4);

    jv = std::move(jobj1);
    assert(jv["532532"].Get<size_t>() == 1);

    int a = 0;

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
        if (::strcmp(err.what(), "ZxJson::JParser::ParseString: unknown escape character!"))
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
        Zut::ZxMem mem{ 4 };
        mem.Write(std::span{ ee });
        int a = 0;

        mem.PosSet<Zut::MoveWay::Cur>(-4);

        uint16_t x0 = mem.Get<uint16_t>();
        uint16_t x1 = mem.Get<uint16_t>();
    }


    Zut::ZxMem mem(100);

    uint32_t tmp_val0 = 222;
    double_t tmp_val1 = 111.11;
    std::array<uint32_t, 3> tmp_array{ 10086, 112233, 666 };
    mem << tmp_val0;
    mem << tmp_val1;
    mem << 100;
    mem << 1.22;
    mem << tmp_array;

    size_t tmp_val2 = 777;
    uint8_t tmp_val3 = 0xF;
    uint16_t tmp_val4 = 0xFF;
    mem.Put(tmp_val2).Put(tmp_val3).Put(tmp_val4);

    mem.PosSet<Zut::MoveWay::Beg>();

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

static auto TestZxFile()
{
    {
        Zut::ZxFile ofs("1.bin", Zut::OpenMod::WriteForce);
        ofs << 32;
        ofs << 54;
    }

    Zut::ZxFile ifs("1.bin", Zut::OpenMod::ReadSafe);
    auto my_val_0 = ifs.Get<uint32_t>();
    auto my_val_1 = ifs.Get<uint32_t>();

}


auto main() -> int
{
    Zut::ZxNative::Sys::InitConsoleEncoding();
    try
    {
        TestZxFile();
        //TestZxMemIO();
        //TestJsonParser();
        //BenchJsonParser();
        // TestZxMem();
    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }

}
