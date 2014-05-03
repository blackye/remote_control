#pragma once

#include "ModuleSocket.h"
#include "Common.h"
#include <tlhelp32.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib")

//任务管理器(进程和服务、帐号密码等其他任务)

class CTaskManager
{
public:
	CTaskManager(void);
	~CTaskManager(void);
	void SendProgressList(IN SOCKET sock, IN int nTag);  //发送当前进程列表信息
	UINT32 GetSysProgressInfo(IN SOCKET sock, IN int nTag);
	BOOL GetProcessFilePathByPId(IN const DWORD dwProcessId, OUT CString & cstrPath );  //根据进程的ID，获取进程所在的路径
	PROGRESS_LEVEL GetProcessLevel(IN const DWORD dwProcessId);
	bool EnableDebugPrivilege();  //给枚举的进程增加SE_DEBUG_NAME权限 , 可以枚举出更多进程路径
	void TaskkillProc(IN PROGRESS_S tagProInfo, IN SOCKET sock);  ////根据进程的PID来结束指定进程
	UINT32 KillPorcessByPID(IN const DWORD dwProcessPid, OUT bool* pbSuccess );
private:
	CModuleSocket m_moduleSocket;
};
