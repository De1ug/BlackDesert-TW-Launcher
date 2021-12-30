// curl-test.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <vector>
#include <Bdo/BdoContext.h>
#include <Bdo/String.h>


int main()
{
	BOOLEAN res = FALSE;

	res = Bdo::BdoContext::GetInstance().StartGame("C:\\BlackDesert TW\\bin64\\BlackDesert64.exe", "ac", "pw", 1);
	if (FALSE == res)
		std::wcout << Utility::String::utf8ToWstring(Bdo::BdoContext::GetInstance().GetErrorString()) << std::endl;
	if (FALSE == res)
		std::cout << Bdo::BdoContext::GetInstance().GetErrorString() << std::endl;

	getchar();

	return 0;
}
