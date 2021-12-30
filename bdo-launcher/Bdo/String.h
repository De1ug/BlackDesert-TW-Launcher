#pragma once
#include <cstdlib>
#include <string>
#include <locale>
#include <codecvt>
#include <windows.h>

namespace Utility
{
	class String
	{

	public:
		static std::string GetRandomString(int length)
		{
			std::string result;
			result.resize(length);
			const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
			for (int i = 0; i < length; i++)
				result[i] = alphanum[rand() % sizeof(alphanum) - 1];
			return result;
		}

		static std::string utf8_encode(const std::wstring& wstr)
		{
			if (wstr.empty()) return std::string();
			int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
			std::string strTo(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
			return strTo;
		}

		static std::wstring utf8_decode(const std::string& str)
		{
			if (str.empty()) return std::wstring();
			int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
			std::wstring wstrTo(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
			return wstrTo;
		}

		static std::wstring utf82unicode(const std::string& str)
		{
			if (str.empty()) return std::wstring();
			int widesize = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
			if (widesize == ERROR_NO_UNICODE_TRANSLATION)
			{
				throw std::exception("Invalid UTF-8 sequence.");
			}
			if (widesize == 0)
			{
				throw std::exception("Error in conversion.");
			}

			std::vector<wchar_t> resultstring(widesize);

			int convresult = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &resultstring[0], widesize);

			if (convresult != widesize)
			{
				throw std::exception("La falla!");
			}

			return std::wstring(&resultstring[0]);
		}

		static std::string WideByte2Acsi(const std::wstring& str)
		{
			if (str.empty()) return std::string();
			int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
			if (asciisize == ERROR_NO_UNICODE_TRANSLATION)
			{
				throw std::exception("Invalid UTF-8 sequence.");
			}
			if (asciisize == 0)
			{
				throw std::exception("Error in conversion.");
			}
			std::vector<char> resultstring(asciisize);
			int convresult = ::WideCharToMultiByte(CP_OEMCP, 0, str.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);

			if (convresult != asciisize)
			{
				throw std::exception("La falla!");
			}

			return std::string(&resultstring[0]);
		}

		static std::string utf82ASCII(const std::string& str)
		{
			if (str.empty()) return std::string();
			std::string strRet("");
			std::wstring wstr = utf82unicode(str);
			strRet = WideByte2Acsi(wstr);
			return strRet;
		}

		static std::wstring StringToWString(const std::string& str)
		{
			std::wstring wstr;
			size_t size;
			wstr.resize(str.length());
			mbstowcs_s(&size, &wstr[0], wstr.size() + 1, str.c_str(), str.size());
			return wstr;
		}

		static std::string WStringToString(const std::wstring& wstr)
		{
			std::string str;
			size_t size;
			str.resize(wstr.length());
			wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
			return str;
		}

		static std::string wstringToUtf8(const std::wstring& str)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t> > strCnv;

			return strCnv.to_bytes(str);
		}


		static std::wstring utf8ToWstring(const std::string& str)
		{
			std::wstring_convert< std::codecvt_utf8<wchar_t> > strCnv;

			return strCnv.from_bytes(str);
		}

	};
}
