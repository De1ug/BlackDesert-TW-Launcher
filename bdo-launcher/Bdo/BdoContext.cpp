#include "BdoContext.h"
#include "String.h"
#include <Shlwapi.h>
#include <iphlpapi.h>
#include <thread>
#include <iostream>

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib,"Shlwapi.lib")

Bdo::BdoContext& Bdo::BdoContext::GetInstance()
{
	static BdoContext ctx;
	return ctx;
}

Bdo::BdoContext::BdoContext()
{
}

Bdo::BdoContext::~BdoContext()
{
}

BOOLEAN Bdo::BdoContext::StartGame(const std::string& gamepath, const std::string& email, const std::string& password, uint8_t serverIndex)
{
	if (gamepath.empty() || email.empty() || password.empty())
	{
		std::wcout << L"資料不能為空 ! " << std::endl;
		m_ErrorString = "資料不能為空 ! ";
		return FALSE;
	}


	BOOLEAN res = FALSE;

	m_GamePath = gamepath;
	m_Email = email;
	m_Password = password;

	switch (serverIndex)
	{
	case 0:
		res = Login(email, password);
		break;
	case 1:
		//res = Login_Gamezbd(email, password);
		break;
	default:
		res = FALSE;
		break;
	}

	if (FALSE == res)
		return res;

	res = CreateBdoClient();
	if (FALSE == res)
		return res;

	m_IsPlaying = TRUE;

	return TRUE;
}

BOOLEAN Bdo::BdoContext::Login(const std::string& email, const std::string& password)
{
	if (email.empty() || password.empty())
	{
		std::wcout << L"資料不能為空 ! " << std::endl;
		m_ErrorString = "資料不能為空 ! ";
		return FALSE;
	}

	CURLcode res = CURLE_OK;
	std::string strLoginToken = "";
	Json::Reader reader;
	Json::Value root;

	m_HttpRequest.sc_cleanHeader();
	m_HttpRequest.sc_cleanExCookie();
	m_HttpRequest.sc_setTimeOut(20000);
	m_HttpRequest.sc_setHttpVersion(CURL_HTTP_VERSION_NONE);
	m_HttpRequest.sc_enableCookie();
	//m_HttpRequest.sc_setGzip();
	m_HttpRequest.sc_appendHeader("Connection: keep-alive");
	m_HttpRequest.sc_appendHeader("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
	m_HttpRequest.sc_appendHeader("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36");
	m_HttpRequest.sc_appendHeader("Host: launcher.tw.playblackdesert.com");
	m_HttpRequest.sc_appendHeader("Sec-Fetch-Mode: navigate");
	m_HttpRequest.sc_appendHeader("Upgrade-Insecure-Requests: 1");
	m_HttpRequest.sc_appendHeader("Sec-Fetch-Dest: document");
	m_HttpRequest.sc_appendHeader("Sec-Fetch-Site: none");
	m_HttpRequest.sc_appendHeader("Upgrade-Insecure-Requests: 1");
	m_HttpRequest.sc_appendHeader("Accept-Encoding: deflate");
	m_HttpRequest.sc_appendHeader("Accept-Language: zh-TW,zh;q=0.9");

	res = m_HttpRequest.sc_get("https://launcher.tw.playblackdesert.com/Member/Login", m_WebData);
	if (res != CURLE_OK)
	{
		if (CURLE_OPERATION_TIMEDOUT == res)
		{
			MessageBox(nullptr, L"連接黑沙伺服器失敗", nullptr, MB_OK);
			std::wcout << L"連接黑沙伺服器失敗" << std::endl;
			m_ErrorString = "連接黑沙伺服器失敗";
			return FALSE;
		}

		MessageBox(nullptr, std::wstring(L"請檢查網路連線 ! err: " + std::to_wstring(res)).c_str(), nullptr, MB_OK);
		std::wcout << L"請檢查網路連線 ! " << std::endl;
		m_ErrorString = "請檢查網路連線 ! ";
		return FALSE;
	}

	m_HttpRequest.sc_utf8ToAnsi(m_WebData);
	strLoginToken = GetLoginToken(m_WebData);
	if (strLoginToken.empty())
	{
		std::wcout << L"請檢查網路連線 ! " << std::endl;
		m_ErrorString = "請檢查網路連線 ! ";
		return FALSE;
	}

	m_HttpRequest.sc_cleanHeader();

	m_HttpRequest.sc_appendHeader("Connection: keep-alive");
	m_HttpRequest.sc_appendHeader("Accept: * / *");
	m_HttpRequest.sc_appendHeader("Origin: https://launcher.tw.playblackdesert.com");
	m_HttpRequest.sc_appendHeader("X-Requested-With: XMLHttpRequest");
	m_HttpRequest.sc_appendHeader("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36");
	m_HttpRequest.sc_appendHeader("Content-Type: application/x-www-form-urlencoded;  charset=UTF-8");
	m_HttpRequest.sc_appendHeader("Sec-Fetch-Site: same-origin");
	m_HttpRequest.sc_appendHeader("Sec-Fetch-Mode: cors");
	m_HttpRequest.sc_appendHeader("Referer: https://launcher.tw.playblackdesert.com/Member/Login");
	m_HttpRequest.sc_appendHeader("Accept-Encoding: deflate");
	m_HttpRequest.sc_appendHeader("Accept-Language: zh-TW,zh;q=0.9,en-US;q=0.8,en;q=0.7");

	res = m_HttpRequest.sc_post(
		"https://launcher.tw.playblackdesert.com/Member/Login/LoginCheck",
		std::string("__RequestVerificationToken=" + strLoginToken + "&_email=" + email + "&_password=" + password + "&X-Requested-With=XMLHttpRequest").c_str(),
		m_WebData);

	if (res != CURLE_OK)
	{
		if (CURLE_OPERATION_TIMEDOUT == res)
		{
			std::wcout << L"連接黑沙伺服器失敗" << std::endl;
			m_ErrorString = "連接黑沙伺服器失敗";
			return FALSE;
		}

		std::wcout << L"請檢查網路連線 ! " << std::endl;
		m_ErrorString = "請檢查網路連線 ! ";
		return FALSE;
	}

	if (reader.parse(m_WebData, root))
	{
		int resCode = root["resultCode"].asInt();
		if (0 != resCode)
		{
			m_ErrorString = root["resultMsg"].asString();
			return FALSE;
		}
	}

	MessageBox(nullptr, L"success", nullptr, MB_OK);
	std::cout << strLoginToken << std::endl;

	return TRUE;
}

BOOLEAN Bdo::BdoContext::CreateBdoClient(BOOLEAN doInjection, const std::string& dllpath)
{
	WCHAR FileDir[MAX_PATH] = { 0 };
	std::string ProcessToken = "";
	std::string ProcessCommand = "";

	memset(&m_StartUpInfo, 0, sizeof(STARTUPINFO));
	memset(&m_ProcessInformation, 0, sizeof(PROCESS_INFORMATION));

	m_StartUpInfo.cb = sizeof(STARTUPINFO);
	m_StartUpInfo.dwFlags = STARTF_USESHOWWINDOW;
	m_StartUpInfo.wShowWindow = SW_SHOW;

	std::wstring dir = Utility::String::StringToWString(m_GamePath);
	std::copy(dir.begin(), dir.end(), FileDir);
	if (0 == wcslen(FileDir) || FALSE == PathRemoveFileSpecW(FileDir))
	{
		m_ErrorString = "路徑錯誤 ! Path";
		return FALSE;
	}

	ProcessToken = GetProcessToken();
	if (ProcessToken.empty())
	{
		m_ErrorString = "獲取令牌失敗 ! GetProcessToken";
		return FALSE;
	}

	ProcessCommand = m_GamePath + " " + ProcessToken;

	if (0 == CreateProcess(nullptr, (LPWSTR)Utility::String::StringToWString(ProcessCommand).c_str(), nullptr, nullptr, FALSE, 0, nullptr, FileDir, &m_StartUpInfo, &m_ProcessInformation))
	{
		m_ErrorString = "啟動遊戲失敗 CreateProcess";
		return FALSE;
	}

	return TRUE;
}

std::string Bdo::BdoContext::GetLoginToken(const std::string& webdata)
{
	if (webdata.empty())
		return std::string();

	std::string token = "";
	std::string strFindToken = "__RequestVerificationToken\" type=\"hidden\" value=\"";
	size_t posStart = webdata.find(strFindToken);
	size_t posEnd = 0;
	if (posStart != webdata.npos)
	{
		posEnd = webdata.find("\" />", posStart);
		if (posEnd == webdata.npos)
			return std::string();
	}


	token.assign(webdata, posStart + strFindToken.size(), posEnd - posStart - strFindToken.size());

	return token;
}

std::string Bdo::BdoContext::GetProcessToken()
{
	CURLcode res = CURLE_OK;
	Json::Reader reader;
	Json::Value root;
	std::string ProcessToken = "";

	res = m_HttpRequest.sc_post("https://launcher.jp.playblackdesert.com/Default/AuthenticateAccount", std::string("macAddr=" + GetMacByGetAdaptersAddresses()).c_str(), m_WebData);
	if (res != CURLE_OK)
		return std::string();

	MessageBoxA(nullptr, m_WebData.c_str(), nullptr, MB_OK);

	if (reader.parse(m_WebData, root))
		ProcessToken = root["resultMsg"].asString();

	return ProcessToken;
}

std::string Bdo::BdoContext::GetMacByGetAdaptersAddresses()
{
	std::string res = "";
	ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
	PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	if (pAddresses == NULL)
		return std::string();
	// Make an initial call to GetAdaptersAddresses to get the necessary size into the ulOutBufLen variable
	if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
		if (pAddresses == NULL)
			return std::string();
	}

	if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == NO_ERROR)
	{
		// If successful, output some information from the data we received
		for (PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
		{
			if (pCurrAddresses->PhysicalAddressLength != 6)
				continue;
			char acMAC[32];
			sprintf_s(acMAC, "%02X-%02X-%02X-%02X-%02X-%02X",
				int(pCurrAddresses->PhysicalAddress[0]),
				int(pCurrAddresses->PhysicalAddress[1]),
				int(pCurrAddresses->PhysicalAddress[2]),
				int(pCurrAddresses->PhysicalAddress[3]),
				int(pCurrAddresses->PhysicalAddress[4]),
				int(pCurrAddresses->PhysicalAddress[5]));
			res = acMAC;
			break;
		}
	}

	free(pAddresses);
	return res;
}