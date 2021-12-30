#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <map>

#include "3rd_party/libcurl/include/curl.h"
#include "3rd_party/jsoncpp/include/json/json.h"
#include "3rd_party/zlib/include/zlib.h"


using std::string;
using std::wstring;
using std::vector;
using std::map;


class HTTPRequest
{
public:
	HTTPRequest();
	~HTTPRequest();

public:
	// 啟用Cookie但不儲存
	void sc_enableCookie();
	// 設置cookie文件
	void sc_setCookieFile(const char* sFilename);
	// 設置COOKIES
	void sc_setCookie(const char* sName, const char* sValue);
	// 清除COOKIES
	void sc_cleanExCookie();
	// 獲取指定Cookie值
	std::string sc_getCookie(const char* sCookieName);
	// 獲取所有的Cookies
	std::string sc_getCookie();
	// 刪除Cookies文件
	void sc_delCookieFile(void);

	void sc_setGzip();

	void sc_setHttpVersion(int version);


	// 清空Http頭
	void sc_cleanHeader();
	// 添加Http頭
	void sc_appendHeader(const char* sHeader);

	// GET  sFileName == NULL ? 不接受返回數據
	CURLcode sc_get(const char* sURL, const wchar_t* sFileName = NULL, bool bCaseRepHdr = false);
	// GET
	CURLcode sc_get(const char* sURL, std::string& sRet, bool bCaseRepHdr = false);
	// POST  sFileName == NULL ? 不接受返回數據
	CURLcode sc_post(const char* sURL, const char* sData, const wchar_t* sFileName = NULL, bool bCaseRepHdr = false);
	// POST
	CURLcode sc_post(const char* sURL, const char* sData, std::string& sRet, bool bCaseRepHdr = false);
	// POST PIC
	CURLcode sc_post(const char* sURL, const char* sFieldName, const char* sFileName, std::string& sRet, bool bCaseRepHdr = false);
	// PUT
	CURLcode sc_put(const char* sURL, const char* sData, std::string& sRet, bool bCaseRepHdr = false);


	// 設置超時時間
	void sc_setTimeOut(int nTimeOutMillisecond);
	// 設置代理
	void sc_setProxy(const char* sProxy, char* sPass = NULL, curl_proxytype ntype = CURLPROXY_HTTP);

	// 讀取文件到string
	static bool sc_readTextFile(const wchar_t* sFileName, string& sRet);
	// 獲取時間戳
	static std::string sc_getTimeStamp(int nOffest = 0);
	// 寫日誌文件
	static bool sc_writeLog(const wchar_t* sFile, const char* sText);
	// 截取中間字符串
	static int sc_getMidString(string src, string& dst, const char* sLeft, const char* sRight, int nStart);
	// 字符串分割函數
	static int sc_split(string str, string pattern, vector<string>& xRet);
	// 替換
	static void sc_replace(string& str, const char* sRp, const char* sTo);

	long sc_getResponseCode();

	// 轉碼
	static void sc_utf8ToAnsi(string& str);
	static void sc_ansiToUtf8(string& str);
	static void sc_ansiToUnicode(const string& str, wstring& sUnicode);
	static void sc_unicodeToAnsi(const wstring& sUnicode, string& str);
	static void sc_utf8ToUnicode(const string& sUtf8, wstring& sUnicode);
	static void sc_unicodeToUtf8(const wstring& sUnicode, string& sUtf8);

	// URL編碼/解碼
	static bool sc_urlEncodeUtf8(string& strUtf8Data, bool bUpperCase = true);
	static bool sc_urlEncodeAnsi(string& strAnsiData, bool bUpperCase = true);
	static bool sc_urlDecodeUtf8(string& strUtf8Data);
	static bool sc_urlDecodeAnsi(string& strAnsiData);

	// BASE 64
	static string sc_base64Decode(const char* s, int len);
	static string sc_base64Encode(const char* b, int len);

private:
	void* m_hCurl = nullptr;
	char* m_szCookie = nullptr;
	struct curl_slist* m_header = nullptr;
	map<string, string>* m_pCookieEx;
	static size_t _callBackWriteFile(void* buffer, size_t size, size_t nmemb, void* usrFp);
	static size_t _callBackWriteString(void* buffer, size_t size, size_t nmemb, void* usrFp);
	static size_t _callBackNotCase(void* buffer, size_t size, size_t nmemb, void* usrFp);

	static string _base64DecodeRaw(const char* b, const char* e);
	static string _base64EncodeRaw(const char* b, const char* e);
	void _checkExCookies();
};