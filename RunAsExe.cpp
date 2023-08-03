#include <stdio.h>
#include <windows.h>
#include <userenv.h>
#include <clocale>
#include <tchar.h>




void DisplayError(LPCWSTR pszAPI)
{
	LPVOID lpvMessageBuffer = NULL;

	int a = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED),
		(LPTSTR)&lpvMessageBuffer,
		0,
		NULL);
	setlocale(LC_ALL, "");
	//... now display this string

	wprintf(L"ERROR: API        = %s.\n", pszAPI);
	wprintf(L"       error code = %d.\n", GetLastError());
	wprintf(L"       message    = %Ls.\n", (LPWSTR)lpvMessageBuffer);

	//
	// Free the buffer allocated by the system
	//
	LocalFree(lpvMessageBuffer);
	ExitProcess(GetLastError());
}

int wmain(int argc,wchar_t* args[])
{
	setlocale(LC_ALL, "chs");
	if (argc<4)
	{
		wprintf(L"命令行参数：输入用户名 域名(domain) 密码 需要管理员运行的程序完整路径\n");

		wprintf(L"example: RunAsExe.exe \"username\" \"domain\" \"password\" \"ProgramPath\"\n");
		
	}
	
	//DWORD dwSessionId;
	HANDLE hToken = NULL;

	TOKEN_PRIVILEGES tp;
	PROCESS_INFORMATION pi;
	STARTUPINFOW si;

	// Initialize structures.
	ZeroMemory(&tp, sizeof(tp));
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);



	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_QUERY| TOKEN_ADJUST_PRIVILEGES,
		&hToken))
	{

		DisplayError(L"OpenProcessToken error");
	}



	// Look up the LUID for the TCB Name privilege.
	//SE_SHUTDOWN_NAME , SE_TCB_NAME,
	if (!LookupPrivilegeValue(NULL, SE_TCB_NAME, 	
		&tp.Privileges[0].Luid)) 
	{
		DisplayError(L"LookupPrivilegeValue error");
	}


	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes =
		SE_PRIVILEGE_ENABLED;//SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, 0)) 
	{
		DisplayError(L"AdjustTokenPrivileges error");
	}


	LPCWSTR lpszUsername = args[1];
	LPCWSTR lpszDomain = args[2];
	LPCWSTR lpszPassword = args[3]; 
	LPCWSTR lpszCmd = args[4];
#pragma message("编译")
	if (LogonUser(lpszUsername, lpszDomain, lpszPassword,
		LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hToken) == 0)
	{
		DisplayError(L"LogonUser error");
	}
	else
	{
		STARTUPINFO sInfo;
		PROCESS_INFORMATION ProcessInfo;
		memset(&ProcessInfo, 0, sizeof(PROCESS_INFORMATION));
		memset(&sInfo, 0, sizeof(STARTUPINFO));
		sInfo.cb = sizeof(STARTUPINFO);
		sInfo.dwX = CW_USEDEFAULT;
		sInfo.dwY = CW_USEDEFAULT;
		sInfo.dwXSize = CW_USEDEFAULT;
		sInfo.dwYSize = CW_USEDEFAULT;
		
		LPCWSTR  cmd = L"cmd /k start \" \" mshta vbscript:createobject(\"shell.application\").shellexecute(\"";
		
		LPCWSTR cmdEnd = L"\",\"::\",,\"runas\",1)(window.close)&exit";
		
		int len1 = wcslen(cmd);
		int len2 = wcslen(cmdEnd);
		int cmdLen = wcslen(lpszCmd);

		
		int totalLen = len1 + len2 + cmdLen + 1;
		//wchar_t* result = new wchar_t[totalLen];
		wchar_t* result = (wchar_t*)malloc(totalLen * sizeof(wchar_t));
		if (result) {
			wcscpy_s(result, totalLen, cmd);

			wcscat_s(result, totalLen, lpszCmd);

			wcscat_s(result, totalLen, cmdEnd);
			bool bRet = CreateProcessWithLogonW(
				lpszUsername,
				lpszDomain,
				lpszPassword,
				LOGON_WITH_PROFILE,
				NULL,//L"c:\\windows\\system32\\cmd.exe" L"C:\\Users\\Public\\Downloads\\sogou_pinyin_102a.exe"
				//_tcsdup(L"c:\\windows\\system32\\cmd.exe"),//lpCommandLine
				_tcsdup(result),
				CREATE_NEW_CONSOLE,
				NULL, //lpEnvironment,
				NULL, // lpCurrentDirectory,
				&sInfo,
				&ProcessInfo);

			if (bRet == 0) {
				DisplayError(L"CreateProcessWithLogonW error");
			}
		}
		
		//wprintf(L"输出中文\n");
		//wcscpy_s(result, totalLen, cmdEnd);
		//打印命令信息
		//wprintf(L"%s \n", result);
		//printf("total %d ,%d, %d, %d\n", totalLen,len1, len2, cmdLen);		//mshta vbscript:createobject(\"shell.application\").shellexecute(\"cmd\",\"::\",,\"runas\",1)(window.close)&exit
	
	
		CloseHandle(ProcessInfo.hProcess);
		CloseHandle(ProcessInfo.hThread);
		
		
		free(result);
		printf("执行成功。");

	}
	return 0;
}