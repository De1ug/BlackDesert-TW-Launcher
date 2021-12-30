#pragma once
#include <WinSock2.h>
#include <HttpRequest/HttpRequest.h>
#include <thread>

namespace Bdo
{
	class BdoContext
	{
	public:
		static BdoContext& GetInstance();

	public:
		BOOLEAN StartGame(const std::string& gamepath, const std::string& email, const std::string& password, uint8_t serverIndex);
		std::string GetErrorString() { return m_ErrorString; }
		VOID SetAutoStart(BOOLEAN IsAutoStart) { m_IsAutoStart = IsAutoStart; }
		BOOLEAN IsAutoStart() { return m_IsAutoStart; }

	private:
		BdoContext();
		~BdoContext();
		BOOLEAN Login(const std::string& email, const std::string& password);
		BOOLEAN Login_Gamezbd(const std::string& email, const std::string& password);
		BOOLEAN CreateBdoClient(BOOLEAN doInjection = FALSE, const std::string& dllpath = "");
		std::string GetLoginToken(const std::string& webdata);
		std::string GetProcessToken();
		std::string GetMacByGetAdaptersAddresses();
		VOID GameThread();

	private:
		HTTPRequest m_HttpRequest;
		PROCESS_INFORMATION m_ProcessInformation = { 0 };
		STARTUPINFO m_StartUpInfo = { 0 };
		BOOLEAN m_IsPlaying = FALSE;
		BOOLEAN m_IsAutoStart = FALSE;
		std::string m_Email = "";
		std::string m_Password = "";
		std::string m_ErrorString = "";
		std::string m_WebData = "";
		std::string m_GamePath = "";
		std::thread m_GameThread;
	};

	
}