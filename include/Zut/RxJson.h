#pragma once
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <unordered_map>


namespace Rut::RxJson
{
	class JValue;

	using JBool = bool;
	using JInt = int;
	using JDouble = double;
	using JString = std::wstring;
	using JArray = std::vector<JValue>;
	using JObject = std::unordered_map<std::wstring, JValue>;

	enum JValueType
	{
		JVALUE_NUL,
		JVALUE_BOL,
		JVALUE_INT,
		JVALUE_DBL,
		JVALUE_STR,
		JVALUE_ARY,
		JVALUE_OBJ
	};

	union JValueUnion
	{
		JBool Bool;
		JInt Int;
		JDouble Double;
		JString* StrPtr;
		JArray* AryPtr;
		JObject* ObjPtr;
	};

	class JValue
	{
	private:
		JValueType m_Type;
		JValueUnion m_Value;

	public:
		JValue();
		JValue(int iValue);
		JValue(bool bValue);
		JValue(double dValue);
		JValue(const wchar_t* sValue);
		JValue(std::wstring_view sValue);
		JValue(const std::wstring& sValue);
		JValue(std::wstring&& sValue) noexcept;
		JValue(const JArray& aValue);
		JValue(JArray&& aValue) noexcept;
		JValue(const JObject& oValue);
		JValue(JObject&& oValue) noexcept;
		JValue(const JValue& rfJValue);
		JValue(JValue&& rfJValue) noexcept;
		~JValue();

		JValue& Copy(const JValue& rfJValue);
		JValue& Move(JValue& rfJValue) noexcept;
		JValue& operator = (const JValue& rfJValue);
		JValue& operator = (JValue&& rfJValue) noexcept;

		JValueType GetType() const noexcept;

		JArray& ToAry();
		JObject& ToOBJ();
		template<class T> auto Get();

		void Dump(std::wstring& wsText, bool isFormat = true, bool isOrder = false, size_t nIndent = 0) const;
	};

	template<class T> auto JValue::Get()
	{
		if constexpr (std::is_integral_v<T>)
		{
			if constexpr (std::is_same_v<T, bool>)
			{
				if (m_Type == JVALUE_BOL) { return m_Value.Bool; }
				throw std::runtime_error("Error Json Type!");
			}
			else
			{
				if (m_Type == JVALUE_INT) { return (T)m_Value.Int; }
				throw std::runtime_error("Error Json Type!");
			}
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			if (m_Type == JVALUE_DBL) { return m_Value.Double; }
			throw std::runtime_error("Error Json Type!");
		}
		else if constexpr (std::is_same_v<T, const wchar_t*>)
		{
			if (m_Type == JVALUE_STR) { return *(m_Value.StrPtr)->data(); }
			throw std::runtime_error("Error Json Type!");
		}
		else if constexpr (std::is_same_v<T, std::wstring_view>)
		{
			if (m_Type == JVALUE_STR) { return *(m_Value.StrPtr)->data(); }
			throw std::runtime_error("Error Json Type!");
		}
		else
		{
			throw std::runtime_error("RxJson:: convert type error!");
		}
	}
}

namespace Rut::RxJson
{
	class Parser
	{
	private:
		std::unique_ptr<wchar_t[]> m_uqJson;
		size_t m_nJsonCCH;
		size_t m_nReadCCH;

	public:
		Parser();
		Parser(std::wstring_view wsJson, JValue& rfJValue);
		~Parser();

	private:
		wchar_t SkipWhite();
		wchar_t GetToken();

		void AddReadCCH(size_t nCount = 1);
		wchar_t GetCurChar();
		wchar_t* GetCurPtr();
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

	public:
      bool Load(const std::string_view phJson, JValue& rfJValue);
	static void Save(const JValue& rfJVaue, const std::filesystem::path& phJson, bool isFormat = false, bool isOrder = false);
	};
}
