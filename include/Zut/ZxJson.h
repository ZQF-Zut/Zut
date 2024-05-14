#pragma once
#include <Zut/ZxMem.h>
#include <memory>
#include <span>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>


namespace Zqf::Zut::ZxJson
{
class JValue;
using JNull_t = std::monostate;
using JBool_t = bool;
using JInt_t = uint64_t;
using JFloat_t = float;
using JDouble_t = double;
using JString_t = std::string;
using JString_View_t = std::string_view;
using JArray_t = std::vector<JValue>;
using JObject_t = std::unordered_map<std::string, JValue>;

class JValue
{
  private:
    using JDataType = std::variant<JNull_t, JBool_t, JInt_t, JDouble_t, std::unique_ptr<JString_t>, std::unique_ptr<JArray_t>, std::unique_ptr<JObject_t>>;
    JDataType m_Data;

  public:
    JValue()
    {
        m_Data = JNull_t{};
    }

    template <class T>
    JValue(const T& rfData)
    {
        using T_decay = std::decay_t<decltype(rfData)>;

        if constexpr (std::is_same_v<T_decay, JNull_t>)
        {
            m_Data = JNull_t{};
        }
        else if constexpr (std::is_same_v<T_decay, JFloat_t> || std::is_same_v<T_decay, JDouble_t> || std::is_same_v<T_decay, JBool_t>)
        {
            m_Data = rfData;
        }
        else if constexpr (std::is_integral_v<T_decay>)
        {
            m_Data = static_cast<JInt_t>(rfData);
        }
        else if constexpr (std::is_bounded_array_v<T>)
        {
            m_Data = std::make_unique<JString_t>(rfData);
        }
        else if constexpr (std::is_same_v<T_decay, JString_View_t>)
        {
            m_Data = std::make_unique<JString_t>(rfData.data(), rfData.size());
        }
        else if constexpr (std::is_same_v<T_decay, JString_t> || std::is_same_v<T_decay, JArray_t> || std::is_same_v<T_decay, JObject_t>)
        {
            m_Data = std::make_unique<T_decay>(rfData);
        }
        else
        {
            throw std::runtime_error(std::format("ZxJson::JValue: error type {}", typeid(T_decay).name()));
        }
    }

    template <class T>
    JValue(T&& rfData) noexcept
    {
        using T_decay = std::decay_t<decltype(rfData)>;

        if constexpr (std::is_same_v<T_decay, JNull_t>)
        {
            m_Data = JNull_t{};
        }
        else if constexpr (std::is_same_v<T_decay, JFloat_t> || std::is_same_v<T_decay, JDouble_t> || std::is_same_v<T_decay, JBool_t>)
        {
            m_Data = rfData;
        }
        else if constexpr (std::is_integral_v<T_decay>)
        {
            m_Data = static_cast<JInt_t>(rfData);
        }
        else if constexpr (std::is_same_v<T_decay, JString_t> || std::is_same_v<T_decay, JArray_t> || std::is_same_v<T_decay, JObject_t>)
        {
            m_Data = std::make_unique<T_decay>(std::move(rfData));
        }
        else
        {
            assert(false); // type of failure to capture
        }
    }

    JValue& Copy(const JValue& rfJar)
    {
        std::visit(
            [this](auto&& data) {
                using T = std::decay_t<decltype(data)>;
                if constexpr (std::is_same_v<T, std::unique_ptr<JString_t>> || std::is_same_v<T, std::unique_ptr<JArray_t>> || std::is_same_v<T, std::unique_ptr<JObject_t>>)
                {
                    (*this) = *data;
                }
                else if constexpr (std::is_same_v<T, JNull_t> || std::is_same_v<T, JBool_t> || std::is_same_v<T,JInt_t> || std::is_same_v<T, JDouble_t>)
                {
                    (*this) = data;
                }
                else
                {
                    throw std::runtime_error(std::format("ZxJson::JValue::Copy: error type {}", typeid(T).name()));
                }
            },
            rfJar.m_Data);

        return *this;
    }

    JValue(const JValue& rfJar)
    {
        this->Copy(rfJar);
    }

    JValue(JValue&& rfJar) noexcept
    {
        this->m_Data = std::move(rfJar.m_Data);
    }

    JValue& operator=(const JValue& rfJar)
    {
        return this->Copy(rfJar);
    };

    JValue& operator=(JValue&& rfJar) noexcept
    {
        this->m_Data = std::move(rfJar.m_Data);
        return *this;
    };

    template <class T>
    auto Get() -> T
    {
        if constexpr (std::is_same_v<T, JNull_t>)
        {
            return JNull_t{};
        }
        else if constexpr (std::is_same_v<T, JBool_t>)
        {
            return std::get<JBool_t>(this->m_Data);
        }
        else if constexpr (std::is_integral_v<T>)
        {
            return static_cast<T>(std::get<JInt_t>(this->m_Data));
        }
        else if constexpr (std::is_same_v<T, JFloat_t> || std::is_same_v<T, JDouble_t>)
        {
            return static_cast<T>(std::get<JDouble_t>(this->m_Data));
        }
        else if constexpr (std::is_same_v<T, JString_View_t>)
        {
            return *std::get<std::unique_ptr<JString_t>>(this->m_Data);
        }
        else if constexpr (std::is_same_v<T, JString_t> || std::is_same_v<T, JString_t&> || std::is_same_v<T, JArray_t> || std::is_same_v<T, JArray_t&> || std::is_same_v<T, JObject_t> || std::is_same_v<T, JObject_t&>)
        {
            return *std::get<std::unique_ptr<std::remove_cvref_t<T>>>(this->m_Data);
        }
        else
        {
            throw std::runtime_error("ZxJson::JValue::Get: error get type!");
        }
    }

    auto ToAry() -> JArray_t&
    {
        if (std::holds_alternative<JNull_t>(this->m_Data))
        {
            this->m_Data = std::make_unique<JArray_t>();
        }
        else if (!std::holds_alternative<std::unique_ptr<JArray_t>>(this->m_Data))
        {
            throw std::runtime_error("ZxJson::JValue::ToAry: error!");
        }

        return *std::get<std::unique_ptr<JArray_t>>(this->m_Data);
    }

    auto ToObj() -> JObject_t&
    {
        if (std::holds_alternative<JNull_t>(this->m_Data))
        {
            this->m_Data = std::make_unique<JObject_t>();
        }
        else if (!std::holds_alternative<std::unique_ptr<JObject_t>>(this->m_Data))
        {
            throw std::runtime_error("ZxJson::JValue::ToObj: error!");
        }

        return *std::get<std::unique_ptr<JObject_t>>(this->m_Data);
    }

    auto Dump(std::string& wsText, bool isFormat, bool isOrder, size_t nIndent) const -> void;
};
} // Zqf::Zut::ZxJson

namespace Zqf::Zut::ZxJson
{
class JParser
{
  private:
    size_t m_nReadCCH{};
    std::span<char> m_spJson;

  public:
    JParser(std::span<char> spJson);
    ~JParser();
    auto Parse(JValue& rfJValue) -> bool;

  private:
    char SkipWhite();
    char GetToken();

    void AddReadCCH(size_t nCount = 1);
    char GetCurChar();
    char* GetCurPtr();
    size_t GeReadCCH() const;
    size_t GetJsonCCH() const;

    void ParseArray(JValue& rfJValue);
    void ParseObject(JValue& rfJValue);
    void ParseNumber(JValue& rfJValue);
    void ParseString(JValue& rfJValue);
    void ParseTrue(JValue& rfJValue);
    void ParseFalse(JValue& rfJValue);
    void ParseNull(JValue& rfJValue);

    void ParseValue(JValue& rfJValue);
};
} // Zqf::Zut::ZxJson


namespace Zqf::Zut::ZxJson
{
class JDoc
{
  private:
    JValue m_JValue;
    
  public:
    JDoc()
    {

    };

    JDoc(const std::string_view msPath)
    {
        this->Load(msPath);
    }

    ~JDoc()
    {

    };

    auto Get() -> JValue&
    {
        return m_JValue;
    }

    auto Load(const std::string_view msPath) -> bool
    {
        ZxMem m_JMem(msPath);
        return JParser{ { m_JMem.Ptr<char*>(), m_JMem.Size<size_t>() } }.Parse(m_JValue);
    }

    auto Save(const std::string_view msPath, bool isFormat, bool isOrder) -> void
    {
        std::string text;
        m_JValue.Dump(text, isFormat, isOrder, 0);
        ZxFile::SaveDataViaPath(msPath, std::span{ text }, true);
    }
};
} // namespace Zqf::Zut::ZxJson