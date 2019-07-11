#include "stdafx.h"
#include "ADBUtils.h"

DWORD __stdcall ThreadAdbshellProc(void *pVoid)
{
	if (pVoid != NULL) {
		ADBUtils *pShell = (ADBUtils *)pVoid;
		pShell->Loop();
	}
	return 0;
}
ADBUtils::ADBUtils(char* adb)
{
	///wchar_t test[] = &adb;
	//adbPath = adb;
	adbPath = adb;
}


ADBUtils::~ADBUtils()
{
}

wchar_t * char2wchar(const char* cchar)
{
	//char *CStr = "string to convert";
	size_t len = strlen(cchar) + 1;
	size_t converted = 0;
	wchar_t *WStr;
	WStr = (wchar_t*)malloc(len * sizeof(wchar_t));
	mbstowcs_s(&converted, WStr, len, cchar, _TRUNCATE);
	return WStr;

	/*wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;*/
}
char * wchar2char(const wchar_t* wchar)
{
	//wchar_t *WStr = L"string to convert";
	size_t len = wcslen(wchar) + 1;
	size_t converted = 0;
	char *CStr;
	CStr = (char*)malloc(len * sizeof(char));
	wcstombs_s(&converted, CStr, len, wchar, _TRUNCATE);
	return CStr;
	/*char * m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wchar), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;*/
}

BOOL ADBUtils::Start()
{
	m_strOutput.Empty();
	SECURITY_ATTRIBUTES sat;
	STARTUPINFO startupinfo;
	PROCESS_INFORMATION pinfo;

	sat.nLength = sizeof(SECURITY_ATTRIBUTES);
	sat.bInheritHandle = TRUE;
	sat.lpSecurityDescriptor = NULL;
	if (CreatePipe(&m_hReadPipe, &m_hWritePipe, &sat, NULL) == FALSE) {
		return FALSE;
	}
	if (CreatePipe(&m_hReadPipe2, &m_hWritePipe2, &sat, NULL) == FALSE) {
		return FALSE;
	}
	startupinfo.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&startupinfo);
	startupinfo.hStdError = m_hWritePipe;
	startupinfo.hStdOutput = m_hWritePipe;
	startupinfo.hStdInput = m_hReadPipe2;
	startupinfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupinfo.wShowWindow = SW_HIDE;
	wchar_t commandLine1[] = L"cmd ";
	wchar_t *adb = char2wchar(adbPath);
	wchar_t test2[] = L"adb shell screencap -p /sdcard/0101.png";
	//wchar_t test2[] = L"adb devices";
	if (CreateProcess(adb, test2, NULL, NULL, TRUE, 0, NULL, NULL, &startupinfo, &pinfo) == FALSE) {
		//DWORD dwError = GetLastError();
		return FALSE;
	}
	CloseHandle(m_hWritePipe);
	CloseHandle(m_hReadPipe2);
	CloseHandle(pinfo.hThread);
	CloseHandle(pinfo.hProcess);
	//m_hProcess = pinfo.hProcess;

	DWORD dwThread = FALSE;
	hThread = CreateThread(NULL, 0, ThreadAdbshellProc, this, 0, &dwThread);//这个参数你根据你的需要决定要不要
	if (hThread == NULL) {
		return FALSE;
	}

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;
}

//这里输入的是在adb shell状态下的批处理命令，如果有多条请以\n分隔
BOOL ADBUtils::RunCmd(const CString&strCmdline)
{
	BOOL bSuccess = TRUE;
	CString strOneCmd;
	int nPos1 = 0;
	int nPos2 = 0;
	DWORD dwWrite = 0;

	if (strCmdline.GetLength() < 2) {
		return FALSE;
	}
	//先等待线程函数准备好
	WaitForSingleObject(m_hEvent, INFINITE);

	while (TRUE) {
		nPos1 = nPos2;
		nPos2 = strCmdline.Find('\n', nPos1);
		if (nPos2 == -1) {
			nPos2 = strCmdline.GetLength();
		}
		strOneCmd = strCmdline.Mid(nPos1, nPos2 - nPos1).Trim();

		//命令长度至少为2
		if (strOneCmd.GetLength() >= 2) {
			strOneCmd += "\r\n";
			//执行
			bSuccess = WriteFile(m_hWritePipe2, strOneCmd, strOneCmd.GetLength(), &dwWrite, NULL);
		}

		++nPos2;
		if (nPos2 >= strCmdline.GetLength()) {
			break;
		}
	}

	return bSuccess;
}

//退出shell命令状态，关闭进程。不能通过TerminateProcess方式结束，否则会有读取不全的情况
BOOL ADBUtils::Stop()
{
	//CString str("");
	RunCmd(CString("exit\nexit"));
	WaitForSingleObject(hThread, INFINITE);
	return TRUE;
	//if ( m_hProcess!=NULL ) {
	//	TerminateProcess(m_hProcess, -1);
	//	CloseHandle(m_hProcess);
	//	m_hProcess = NULL;
	//}

	//return TRUE;
}

//读取输出结果,调用前请务必调用Stop先结束
CString ADBUtils::GetOutput()
{
	WaitForSingleObject(hThread, INFINITE);

	//两个exit要去掉
	int nPos1 = 0;
	//int nPos2 = 0;

	//m_strOutput.Find('d');
	wchar_t *txt = char2wchar("# exit");
	nPos1 = m_strOutput.Find(txt);
	if (nPos1 == -1) {
		wchar_t *txt2 = char2wchar("$ exit");
		nPos1 = m_strOutput.Find(txt2);
	}
	if (nPos1 != -1) {
		m_strOutput = m_strOutput.Left(nPos1);
		nPos1 = m_strOutput.ReverseFind('\n');
		m_strOutput = m_strOutput.Left(nPos1);
	}

	return m_strOutput;
}

BOOL ADBUtils::Loop()
{
	CHAR buffer[1024] = { 0 };
	DWORD dwRead = 0;
	while (TRUE || isOpenInfo) {
		RtlZeroMemory(buffer, _countof(buffer));
		SetEvent(m_hEvent);
		if (ReadFile(m_hReadPipe, buffer, _countof(buffer), &dwRead, NULL) == FALSE) {
			break;
		}
		else {
			m_strOutput += buffer;
			//if (StrStrA(buffer, "# exit") != NULL) {
				break;
			//}
		}
	}
	m_hReadPipe = NULL;
	if (m_hReadPipe != NULL) {
		CloseHandle(m_hReadPipe);
	}
	m_hWritePipe2 = NULL;
	if (m_hWritePipe2 != NULL) {
		CloseHandle(m_hWritePipe2);
	}
	if (m_hEvent != 0 ){
		CloseHandle(m_hEvent);
	}
	m_hEvent = NULL;
	if (hThread != NULL) {
		CloseHandle(hThread);
	}
	hThread = NULL;

	return TRUE;
}

BOOL ADBUtils::connent() {
	if (adbPath == NULL) {
		return FALSE;
	}
	wchar_t *adb = char2wchar(adbPath);
	return execute(adb,L"adb devices");
	//return execute(adb, L"adb devices >d:\\1.txt");
}
BOOL ADBUtils::execute(wchar_t adb[], wchar_t parameter[]) {
	if (adb == NULL || parameter == NULL) {
		return false;
	}
	m_strOutput.Empty();
	SECURITY_ATTRIBUTES sat;
	STARTUPINFO startupinfo;
	PROCESS_INFORMATION pinfo;

	sat.nLength = sizeof(SECURITY_ATTRIBUTES);
	sat.bInheritHandle = TRUE;
	sat.lpSecurityDescriptor = NULL;
	if (CreatePipe(&m_hReadPipe, &m_hWritePipe, &sat, NULL) == FALSE) {
		return FALSE;
	}
	if (CreatePipe(&m_hReadPipe2, &m_hWritePipe2, &sat, NULL) == FALSE) {
		return FALSE;
	}
	startupinfo.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&startupinfo);
	startupinfo.hStdError = m_hWritePipe;
	startupinfo.hStdOutput = m_hWritePipe;
	startupinfo.hStdInput = m_hReadPipe2;
	startupinfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupinfo.wShowWindow = SW_HIDE;
	//wchar_t commandLine1[] = L"cmd ";
	//wchar_t *adb = char2wchar(adbPath);
	wchar_t test2[] = L"adb shell screencap -p /sdcard/0101.png";
	//wchar_t test2[] = L"adb devices";
	if (CreateProcess(adb, parameter, NULL, NULL, TRUE, 0, NULL, NULL, &startupinfo, &pinfo) == FALSE) {
		//DWORD dwError = GetLastError();
		return FALSE;
	}
	CloseHandle(m_hWritePipe);
	CloseHandle(m_hReadPipe2);
	CloseHandle(pinfo.hThread);
	CloseHandle(pinfo.hProcess);
	//m_hProcess = pinfo.hProcess;

	DWORD dwThread = FALSE;
	hThread = CreateThread(NULL, 0, ThreadAdbshellProc, this, 0, &dwThread);//这个参数你根据你的需要决定要不要
	if (hThread == NULL) {
		return FALSE;
	}

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;


	//return TRUE;
}
CString ADBUtils::GetFrist(CString str) {
	int nPos1 = str.Find('\n');
	int leng = str.GetLength();
	int i = leng - nPos1-1;
	//CString str2 = str.Right(nPos1);
	CString str2 = str.Right(i);
	CString CEnd("device");
	wchar_t *cc = CEnd.AllocSysString();
	int i2 = str2.ReverseFind(*cc);
	if (i2 > 0) {
		int tPos = str2.Find('\t');
		phontDevice = str2.Left(tPos);
		//getchar();
	}

	//printf("");

	return phontDevice;
}
CString ADBUtils::GetPhoto() {
	if (phontDevice.IsEmpty()|| adbPath ==NULL) {//没有连接的手机
		return NULL;
	}
	//wchar_t *adb = char2wchar(adbPath);
	////adb exec-out screencap -p > sc.png
	//CString paream;
	//paream.Format(_T("adb -s %s exec-out screencap -p > %s"), CStringW(phontDevice), CStringW("d:\\2019\\0101.png"));
	//BOOL b = execute(adb, paream.AllocSysString());
	//if (b) {
	//	return CString("d:\\2019\\0101.png");
	//}



	//adb shell screencap -p /sdcard/0101.png
	//从手机截屏copy出来
	CString str;
	///str.Format(L"adb %s %s %s %s", "qq", CStringW("ww"), CStringW("qweq"), CStringW("uu"));
	str.Format(_T("adb -s %s shell screencap -p %s "), CStringW(phontDevice), CStringW("/sdcard/0101.png"));
	wchar_t *adb = char2wchar(adbPath);
	wchar_t *paream = str.AllocSysString();
	BOOL b = execute(adb, paream);
	if (!b) {
		return NULL;
	}
	//Stop();
	//adb -s 925cdad5 exec-out screencap -p > d:\\2019\\0101.png
	//adb pull /sdcard/0101.png d:\\2019\0101.png
	str.Format(_T("adb -s %s pull %s %s "), CStringW(phontDevice), CStringW("/sdcard/0101.png"), CStringW("d:\\2019\\0101.png"));
	//保存截图到手机成功后
	paream = str.AllocSysString();
	b = execute(adb, paream);
	//CString ss = GetOutput();
	if (b) {
		//CString photo;
		//photo.Format(L"%s", CStringW("d:\\2019\\0101.png"));
		//return photo;
		return CString("d:\\2019\\0101.png");
	}

	return NULL;


}


