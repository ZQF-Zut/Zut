#include <Zut/ZxJson.h>
#include <Zut/ZxFile.h>
#include <map>
#include <cassert>
#include <stdexcept>


namespace Zqf::Zut::ZxJson
{
	auto JValue::Dump(std::string& wsText, bool isFormat, size_t nIndent) const ->void
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
			for (const auto& value : *(std::get<std::unique_ptr<JArray_t>>(m_Data)))
			{
				if (isFormat)
				{
					wsText.append(1, '\n');
					wsText.append(nIndent, '\t');
				}

				value.Dump(wsText, isFormat, nIndent);
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
				for (const auto& value : *(std::get<std::unique_ptr<JObject_t>>(m_Data)))
				{
					if (isFormat)
					{
						wsText.append(1, '\n');
						wsText.append(nIndent, '\t');
					}

					wsText.append(1, '\"');
					wsText.append(value.first);
					wsText.append(1, '\"');
					wsText.append(1, ':');

					if (isFormat)
					{
						wsText.append(1, ' ');
					}

					value.second.Dump(wsText, isFormat, nIndent);
					wsText.append(1, ',');
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

}


namespace Zqf::Zut::ZxJson
{
	JParser::JParser(std::span<const char> spJson)
	{
		m_spJson = spJson;
	}

	JParser::~JParser()
	{

	}

	auto Zut::ZxJson::JParser::Parse(JValue& rfJValue) -> bool
	{
		if (m_spJson.empty())
		{
			rfJValue = JNull_t{};
			return true;
		}

		this->ParseValue(rfJValue);
		return this->GetReadBytes() == this->TotalBytes();
	}


	auto JParser::CurPtr() -> const char*
	{
		return m_spJson.data() + m_nReadBytes;
	}

	auto JParser::TotalBytes() const -> size_t
	{
		return m_spJson.size();
	}

	auto JParser::GetReadBytes() const -> size_t
	{
		return m_nReadBytes;
	}

	auto JParser::AddReadBytes(size_t nCount) -> void
	{
		this->m_nReadBytes += nCount;
	}


	auto JParser::SkipWhite() -> char
	{
		while (true)
		{
			char tmp_char = this->CurToken();
			switch (tmp_char)
			{
			case '\t': case '\n': case '\r': case ' ':
				this->AddReadBytes();
				continue;
			}
			return tmp_char;
		}
	}

	auto JParser::NextToken() -> char
	{
		char tmp_char = this->SkipWhite();

		switch (tmp_char)
		{
		case '{': case '}':
		case '[': case ']':
		case '"': case ':':
		case 'n': case 't': case 'f':
		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '-':
			return tmp_char;
		case ',':
			this->AddReadBytes();
			return this->NextToken();
		default:
			throw std::runtime_error("ZxJson::JParser::NextToken(): not find token");
		}
	}

	auto JParser::CurToken() -> char
	{
		return m_spJson[m_nReadBytes];
	}

	auto JParser::ParseArray(JValue& rfJValue) -> void
	{
		assert(this->CurToken() == '[');

		auto& jarry = rfJValue.Sure<JArray_t>();
		this->AddReadBytes();

		while (this->NextToken() != ']')
		{
			JValue value;
			this->ParseValue(value);
			jarry.emplace_back(std::move(value));
		}

		this->AddReadBytes();
		return;
	}

	auto JParser::ParseObject(JValue& rfJValue) -> void
	{
		assert(this->CurToken() == '{');

		auto& obj = rfJValue.Sure<JObject_t>();
		this->AddReadBytes();

		std::string_view key;
		while (true)
		{
			switch (this->NextToken())
			{
			case '"': // parse key
			{
				this->AddReadBytes();

				const char* beg = this->CurPtr();
				const char* end = ::strchr(beg, '"');
				key = { beg, end };

				this->AddReadBytes((end - beg) + 1);
			}
			break;

			case ':': // check key : value
			{
				this->AddReadBytes();
				this->SkipWhite();

				JValue value;
				this->ParseValue(value);

				if (const auto [ite, is_insert_key] = obj.try_emplace({ key.data(), key.size() }); !is_insert_key)
				{
					throw std::runtime_error("ZxJson::JParser::ParseObject: same key!");
				}
				else
				{
					ite->second = std::move(value);
				}
			}
			break;

			case '}': // end of object
			{
				this->AddReadBytes();
				return;
			}
			break;

			default: throw std::runtime_error("ZxJson::JParser::ParseObject: error!");
			}
		}
	}

	auto JParser::ParseNumber(JValue& rfJValue) -> void
	{
		char* end = nullptr;
		const char* beg = this->CurPtr();
		const double num_org = ::strtod(beg, &end);
		this->AddReadBytes(end - beg);

		const int num_int = (int)(num_org);
		(num_org == (double)(num_int)) ? (rfJValue = num_int) : (rfJValue = num_org);
	}

	auto JParser::ParseString(JValue& rfJValue) -> void
	{
		auto fn_decode_unicode = [this](std::string& rfStr)
			{
				auto fn_read_code_point = [this]() -> uint16_t
					{
						auto fn_char2num = [](char cChar) -> uint8_t {
							uint8_t char_v = static_cast<uint8_t>(cChar);
							if (char_v >= 0x61 && char_v <= 0x7a) { return char_v - 0x57; }// a-z
							else if (char_v >= 0x41 && char_v <= 0x5A) { return char_v - 0x37; }// A-Z
							else if (char_v >= 0x30 && char_v <= 0x39) { return char_v - 0x30; }// 0-9
							else { throw std::runtime_error("ZxJson::JParser::DecodeUnicode:: read code point error!"); }
							};

						const char* code_point_str_ptr = this->CurPtr();
						uint8_t hex_0 = fn_char2num(code_point_str_ptr[0]);
						uint8_t hex_1 = fn_char2num(code_point_str_ptr[1]);
						uint8_t hex_2 = fn_char2num(code_point_str_ptr[2]);
						uint8_t hex_3 = fn_char2num(code_point_str_ptr[3]);
						uint16_t code_point = (hex_0 << 12) | (hex_1 << 8) | (hex_2 << 4) | (hex_3 << 0);
						this->AddReadBytes(4);
						return code_point;
					};

				assert(this->CurToken() == 'u');
				this->AddReadBytes();

				uint32_t code_point = fn_read_code_point();
				if (code_point >= 0xD800 && code_point <= 0xDBFF) // check surrogate pair leading range
				{
					// check '\u'
					if (this->CurPtr()[0] == '\\' && this->CurPtr()[1] == 'u')
					{
						this->AddReadBytes(2);

					}
					else
					{
						throw std::runtime_error("ZxJson::JParser::DecodeUnicode: surrogate pair trailing error!");
					}

					// read trailing
					uint16_t code_point_trailing = fn_read_code_point();
					if (code_point_trailing >= 0xDC00 && code_point_trailing <= 0xDFFF) // trailing range
					{
						code_point = (((code_point - 0xD800) << 10) | (code_point_trailing - 0xDC00)) + 0x10000;
					}
					else
					{
						throw std::runtime_error("ZxJson::JParser::DecodeUnicode:: surrogate pair trailing range error!");
					}
				}

				// encode to utf8
				if (code_point <= 0x7F)
				{
					rfStr.append(1, static_cast<char>(code_point & 0x7F));
				}
				else if (code_point <= 0x7FF)
				{
					rfStr.append(1, static_cast<char>(0xC0 | ((code_point >> 0x06) & 0xFF)));
					rfStr.append(1, static_cast<char>(0x80 | (code_point & 0x3F)));
				}
				else if (code_point <= 0xFFFF)
				{
					rfStr.append(1, static_cast <char>(0xE0 | ((code_point >> 0x0C) & 0xFF)));
					rfStr.append(1, static_cast <char>(0x80 | ((code_point >> 0x06) & 0x3F)));
					rfStr.append(1, static_cast <char>(0x80 | (code_point & 0x3F)));
				}
				else if (code_point <= 0x10FFFF)
				{
					rfStr.append(1, static_cast <char>(0xF0 | ((code_point >> 0x12) & 0xFF)));
					rfStr.append(1, static_cast <char>(0x80 | ((code_point >> 0x0C) & 0x3F)));
					rfStr.append(1, static_cast <char>(0x80 | ((code_point >> 0x06) & 0x3F)));
					rfStr.append(1, static_cast <char>(0x80 | (code_point & 0x3F)));
				}
				else
				{
					throw std::runtime_error("ZxJson::JParser::DecodeUnicode:: encode utf8 error!");
				}
			};

		assert(this->CurToken() == '"');
		this->AddReadBytes();

		std::string str;
		while (true)
		{
			char ch = this->CurToken();
			this->AddReadBytes();
			if (ch == '\\')
			{
				switch (this->CurToken())
				{
				case '\\': ch = '\\'; break;
				case '"': ch = '\"'; break;
				case 'b': ch = '\b'; break;
				case 'f': ch = '\f'; break;
				case 'n': ch = '\n'; break;
				case 'r': ch = '\r'; break;
				case 't': ch = '\t'; break;
				case 'u': ch = 'u'; break;
				default: throw std::runtime_error("ZxJson::JParser::ParseString: unknown escape character!");
				}

				if (ch == 'u')
				{
					fn_decode_unicode(str);
				}
				else
				{
					str.append(1, ch);
					this->AddReadBytes();
				}
			}
			else if (ch == '"') // end
			{
				break;
			}
			else
			{
				str.append(1, ch);
			}
		}

		rfJValue = std::move(str);
	}

	auto JParser::ParseTrue(JValue& rfJValue) -> void
	{
		assert(this->CurToken() == 't');
		assert(::strncmp(this->CurPtr(), "true", 4) == 0);
		this->AddReadBytes(4);
		rfJValue = true;
	}

	auto JParser::ParseFalse(JValue& rfJValue) -> void
	{
		assert(this->CurToken() == 'f');
		assert(::strncmp(this->CurPtr(), "false", 5) == 0);
		this->AddReadBytes(5);
		rfJValue = false;
	}

	auto JParser::ParseNull(JValue& rfJValue) -> void
	{
		assert(this->CurToken() == 'n');
		assert(::strncmp(this->CurPtr(), "null", 4) == 0);
		this->AddReadBytes(4);
		rfJValue = JNull_t{};
	}

	auto JParser::ParseValue(JValue& rfJValue) -> void
	{
		switch (this->CurToken())
		{
		case '{': ParseObject(rfJValue); break; // object
		case '[': ParseArray(rfJValue); break; // array
		case 't': ParseTrue(rfJValue); break; // true
		case 'f': ParseFalse(rfJValue); break; // false
		case 'n': ParseNull(rfJValue); break; // null
		case '"': ParseString(rfJValue); break; // string
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '-': ParseNumber(rfJValue); break; // Number
		default: throw std::runtime_error("Zut::ZxJson::JParser::ParseValue(): value error!");
		}
	}


}