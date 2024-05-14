#include <Zut/ZxJson.h>
#include <Zut/ZxFile.h>

#include <cassert>
#include <map>
#include <stdexcept>

namespace Zqf::Zut::ZxJson
{
auto JValue::Dump(std::string& wsText, bool isFormat, bool isOrder, size_t nIndent) const ->void
{
    if (std::holds_alternative<JNull_t>(m_Data))
    {
        wsText.append("null");
    }
    else if (std::holds_alternative<JBool_t>(m_Data))
    {
        wsText.append((std::get<JBool_t>(m_Data)) ? "true" : "false");
    }
    else if (std::holds_alternative<JInt_t>(m_Data))
    {
        wsText.append(std::to_string(std::get<JInt_t>(m_Data)));
    }
    else if (std::holds_alternative<JDouble_t>(m_Data))
    {
        wsText.append(std::to_string(std::get<JDouble_t>(m_Data)));
    }
    else if (std::holds_alternative<std::unique_ptr<JString_t>>(m_Data))
    {
        wsText.append(1, '\"');
        for (auto ch : *(std::get<std::unique_ptr<JString_t>>(m_Data)))
        {
            switch (ch)
            {
            case '\n': ch = 'n'; break;
            case '\r': ch = 'r'; break;
            case '\b': ch = 'b'; break;
            case '\t': ch = 't'; break;
            case '\f': ch = 'f'; break;
            case '\"': ch = '"'; break;
            case '\\': ch = '\\'; break;
            default: {
                wsText.append(1, ch);
                continue;
            }
            }

            wsText.append(1, '\\');
            wsText.append(1, ch);
        }
        wsText.append(1, '\"');
    }
    else if (std::holds_alternative<std::unique_ptr<JArray_t>>(m_Data))
    {
        wsText.append(1, '[');

        nIndent++;
        for (auto& value : *(std::get<std::unique_ptr<JArray_t>>(m_Data)))
        {
            if (isFormat)
            {
                wsText.append(1, '\n');
                wsText.append(nIndent, '\t');
            }

            value.Dump(wsText, isFormat, isOrder, nIndent);
            wsText.append(1, ',');
        }
        nIndent--;

        // if not null array
        if (wsText.back() == ',')
        {
            wsText.pop_back();
            if (isFormat)
            {
                wsText.append(1, '\n');
                wsText.append(nIndent, '\t');
            }
        }

        wsText.append(1, ']');
    }
    else if (std::holds_alternative<std::unique_ptr<JObject_t>>(m_Data))
    {
        wsText.append(1, '{');

        nIndent++;
        {
            const auto& unorder_obj = *(std::get<std::unique_ptr<JObject_t>>(m_Data));

            auto fn_format_obj = [&wsText, &isFormat, &isOrder, &nIndent](const std::pair<std::string, JValue>& rfKV) {
                if (isFormat)
                {
                    wsText.append(1, '\n');
                    wsText.append(nIndent, '\t');
                }
                wsText.append(1, '\"');
                wsText.append(rfKV.first);
                wsText.append(1, '\"');
                wsText.append(1, ':');
                if (isFormat)
                {
                    wsText.append(1, ' ');
                }
                rfKV.second.Dump(wsText, isFormat, isOrder, nIndent);
                wsText.append(1, ',');
            };

            if (isOrder)
            {
                std::map<std::string, JValue> order_obj;
                for (auto& value : unorder_obj) { order_obj.insert(value); }
                for (const auto& value : order_obj) { fn_format_obj(value); }
            }
            else
            {
                for (const auto& value : unorder_obj) 
                { 
                    fn_format_obj(value);
                }
            }
        }
        nIndent--;

        // if not null object
        if (wsText.back() == ',')
        {
            wsText.pop_back();
            if (isFormat)
            {
                wsText.append(1, '\n');
                wsText.append(nIndent, '\t');
            }
        }

        wsText.append(1, '}');
    }
}

} // namespace Zqf::Zut::ZxJson

namespace Zqf::Zut::ZxJson
{
JParser::JParser(std::span<char> spJson)
{
    m_spJson = spJson;
}

JParser::~JParser()
{

}

auto Zut::ZxJson::JParser::Parse(JValue& rfJValue) -> bool
{
    this->ParseValue(rfJValue);
    return this->GeReadCCH() == this->GetJsonCCH();
}

void JParser::AddReadCCH(size_t nCount)
{
    this->m_nReadCCH += nCount;
}

char JParser::GetCurChar()
{
    return m_spJson[m_nReadCCH];
}

char* JParser::GetCurPtr()
{
    return m_spJson.data() + m_nReadCCH;
}

size_t JParser::GeReadCCH() const
{
    return m_nReadCCH;
}

size_t JParser::GetJsonCCH() const
{
    return m_spJson.size();
}

char JParser::SkipWhite()
{
    while (true)
    {
        char tmp_char = this->GetCurChar();
        switch (tmp_char)
        {
        case '\t':
        case '\n':
        case '\r':
        case ' ':
            this->AddReadCCH();
            continue;
        }
        return tmp_char;
    }
}

char JParser::GetToken()
{
    char tmp_char = this->SkipWhite();

    switch (tmp_char)
    {
    case '{':
    case '}':
    case '[':
    case ']':
    case '"':
    case ':':
    case 'n':
    case 't':
    case 'f':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
        return tmp_char;
    case ',':
        this->AddReadCCH();
        return this->GetToken();
    default:
        throw std::runtime_error("Not Find Token");
    }
}

void JParser::ParseArray(JValue& rfJValue)
{
    assert(this->GetCurChar() == '[');

    auto& jarry = rfJValue.Sure<JArray_t>();
    this->AddReadCCH();

    while (this->GetToken() != ']')
    {
        JValue value;
        this->ParseValue(value);
        jarry.emplace_back(std::move(value));
    }

    this->AddReadCCH();
    return;
}

void JParser::ParseObject(JValue& rfJValue)
{
    assert(this->GetCurChar() == '{');

    auto& obj = rfJValue.Sure<JObject_t>();
    this->AddReadCCH();

    std::string_view key;
    while (true)
    {
        switch (this->GetToken())
        {
        case '"': // parse key
        {
            this->AddReadCCH();

            char* beg = this->GetCurPtr();
            char* end = ::strchr(beg, '"');
            key = { beg, end };

            this->AddReadCCH((end - beg) + 1);
        }
        break;

        case ':': // check key : value
        {
            this->AddReadCCH();
            this->SkipWhite();

            JValue value;
            this->ParseValue(value);
            obj[std::string(key)] = std::move(value);
        }
        break;

        case '}': // end of object
        {
            this->AddReadCCH();
            return;
        }
        break;

        default: {
            throw std::runtime_error("ParseObject Error!");
        }
        }
    }
}

void JParser::ParseNumber(JValue& rfJValue)
{
    char* end = nullptr;
    char* beg = this->GetCurPtr();
    double num_org = ::strtod(beg, &end);

    this->AddReadCCH(end - beg);

    int num_int = (int)(num_org);
    double num_loss = (double)(num_int);

    num_org == num_loss ? (rfJValue = (int)num_int) : (rfJValue = num_org);
}

void JParser::ParseString(JValue& rfJValue)
{
    auto fn_make_unicode = [](const char* wpStr) -> uint32_t {
        auto fn_char2num = [](char cChar) -> uint8_t {
            if ((uint16_t)cChar >= 0x41) // A-Z
            {
                return (uint8_t)(cChar - 0x37);
            }
            else // 0-9
            {
                return (uint8_t)(cChar - 0x30);
            }
        };

        uint8_t hex_0 = fn_char2num(wpStr[0]);
        uint8_t hex_1 = fn_char2num(wpStr[1]);
        uint8_t hex_2 = fn_char2num(wpStr[2]);
        uint8_t hex_3 = fn_char2num(wpStr[3]);

        return (hex_0 << 12) | (hex_1 << 8) | (hex_2 << 4) | (hex_3 << 0);
    };

    assert(this->GetCurChar() == '"');

    this->AddReadCCH();

    std::string text;
    while (true)
    {
        char ch = this->GetCurChar();

        if (ch == '\\') // control characters
        {
            this->AddReadCCH();
            char ctrl_char = this->GetCurChar();

            switch (ctrl_char)
            {
            case '\\':
                ctrl_char = '\\';
                break;
            case '"':
                ctrl_char = '\"';
                break;
            case 'b':
                ctrl_char = L'\b';
                break;
            case 'f':
                ctrl_char = '\f';
                break;
            case 'n':
                ctrl_char = '\n';
                break;
            case 'r':
                ctrl_char = '\r';
                break;
            case 't':
                ctrl_char = '\t';
                break;
            case 'u': {
                this->AddReadCCH();
                ctrl_char = (char)fn_make_unicode(this->GetCurPtr());
                this->AddReadCCH(3);
            }
            break;

            default:
                throw std::runtime_error("Unknow Control Characters");
            }

            ch = ctrl_char;
        }
        else if (ch == '"') // end
        {
            this->AddReadCCH(); // skip " char
            break;
        }

        text.append(1, ch);
        this->AddReadCCH(); // netx char
    }

    rfJValue = std::move(text);
}

void JParser::ParseTrue(JValue& rfJValue)
{
    assert(this->GetCurChar() == 't');

    this->AddReadCCH(4);
    rfJValue = true;
}

void JParser::ParseFalse(JValue& rfJValue)
{
    assert(this->GetCurChar() == 'f');

    this->AddReadCCH(5);
    rfJValue = false;
}

void JParser::ParseNull(JValue& rfJValue)
{
    assert(this->GetCurChar() == 'n');

    this->AddReadCCH(4);
    rfJValue = JValue();
}

void JParser::ParseValue(JValue& rfJValue)
{
    switch (this->GetCurChar())
    {
    case '{':
        ParseObject(rfJValue);
        break; // object
    case '[':
        ParseArray(rfJValue);
        break; // array
    case 't':
        ParseTrue(rfJValue);
        break; // true
    case 'f':
        ParseFalse(rfJValue);
        break; // false
    case 'n':
        ParseNull(rfJValue);
        break; // null
    case '"':
        ParseString(rfJValue);
        break; // string
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
        ParseNumber(rfJValue);
        break; // Number
    default:
        throw std::runtime_error("Json Parse Value Error!");
    }
}


} // namespace Zqf::Zut::ZxJson