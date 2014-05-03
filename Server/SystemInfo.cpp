#include "StdAfx.h"
#include "SystemInfo.h"
#include "Common.h"
#include <Wininet.h>
#include <windows.h>
#include <atlbase.h>
#include <Vfw.h>

#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"Vfw32.lib")

using namespace std;

CSystemInfo::CSystemInfo(void)
{
}

CSystemInfo::~CSystemInfo(void)
{
}

int CSystemInfo::GetSysVersion()
{
	/*
	在PEB结构中存放着操作系统版本信息的函数，
	ULONG OSMajorVersion; // A4h 偏移地址a4h显示操作系统主版本号
	ULONG OSMinorVersion; // A8h 偏移地址a8h显示操作系统子版本号
	通过读取OSMajorVersion和OSMinorVersion的数据能够确定操作系统的版本。
	FS段寄存器指向当前的TEB结构，在TEB偏移0x30处是PEB指针，通过这个指针可以取得存放在PEB中的信息。
	不同的操作系统，版本信息的存放的位置是不同的，其中
	2000 0X0500   
	XP 0X0501
	2003 0X0502
	VISTA 0X0600
	Win7 0X0601
	这样根据这些信息我们可以通过不调用API函数来获取操作系统的版本信息。
	// _WIN32_WINNT version constants
	//
	Windows 8                                             _WIN32_WINNT_WIN8  (0x0602)
	Windows 7                                             _WIN32_WINNT_WIN7  (0x0601)
	Windows Server 2008                                   _WIN32_WINNT_WS08  (0x0600)
	Windows Vista                                         _WIN32_WINNT_VISTA (0x0600)
	Windows Server 2003 with SP1, Windows XP with SP2     _WIN32_WINNT_WS03  (0x0502)
	Windows Server 2003, Windows XP                       _WIN32_WINNT_WINXP (0x0501)
	*/

	int a=0 , b=0, i=0, j=0;
	int nVersion = 0;
	_asm
	{
		pushad
			mov ebx,fs:[0x18] ; get self pointer from TEB
			mov eax,fs:[0x30] ; get pointer to PEB / database
			mov ebx,[eax+0A8h] ; get OSMinorVersion
			mov eax,[eax+0A4h] ; get OSMajorVersion
			mov j,ebx
			mov i,eax
			popad
	}
	if((i==5)&&(j==0))
	{
		printf("系统版本为 Windows 2000\n");
		nVersion = OS_2000;
	}
	else if((i==5)&&(j==1))
	{ 
		printf("系统版本为 Windows XP\n");
		nVersion = OS_XP;
	}
	else if((i==5)&&(j==2))
	{
		printf("系统版本为 Windows 2003 sever\n");
		nVersion = OS_2003;
	}
	else if((i==6)&&(j==0))
	{
		printf("系统版本为 Windows Vista\n");
		nVersion = OS_Vista;
	}
	else if((i==6)&&(j==1))
	{
		printf("系统版本为 Win7\n");
		nVersion = OS_WIN7;
	}
	else if((i==6)&&(j==2))
	{
		printf("系统版本为 Win8\n");
		nVersion = OS_WIN8;
	}
	else
	{
		printf("未知操作系统版本\n");
		nVersion = OS_UNKNOWN;
	}
	return nVersion;
}

void CSystemInfo::GetInternetIP(OUT char* pWANIP, OUT bool* pbSuccess )
{
	bool bSuccess = true;
	HINTERNET hInternet=NULL; 
	HINTERNET hConnect=NULL; 
	char szText[512]= {0}; 
    TCHAR szAccept[] = _T("Accept: */*\r\n\r\n"); 
    TCHAR szUrl[] =_T("http://iframe.ip138.com/ic.asp"); 
	DWORD dwReadLen=0;
	hInternet=InternetOpen(_T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)"),0,NULL,NULL,0); 
	if (!hInternet) 
	{
		bSuccess = false;
		goto __Error;
	}
	hConnect=InternetOpenUrl(hInternet,szUrl,szAccept,wcslen(szAccept),INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD,0); 
	if (!hConnect) 
	{
		bSuccess = false;
		goto __Error;
	}
	InternetReadFile(hConnect, szText,512, &dwReadLen);
	if (hInternet) 
	{
		InternetCloseHandle(hInternet);
	}
	if (hConnect) 
	{
		InternetCloseHandle(hConnect);
	}
	char* pStart = strstr(szText,"[");  // [110.123.43.16]
	if (NULL == pStart) 
	{
		bSuccess = false;
		goto __Error;
	}
	pStart++;
	char* pEnd = strstr(szText, "]");
	if (NULL == pEnd)
	{
		bSuccess = false;
		goto __Error;
	}
	strncpy(pWANIP, pStart, pEnd - pStart);

__Error:
	*pbSuccess = bSuccess;
}

void CSystemInfo::GetHostName(OUT char* hostName, OUT char* szLocalIP, OUT bool* pbSuccess)
{
	char host_name[255] = {0};
	bool bSuccess = true;
	int iRet = ::gethostname(host_name, sizeof(host_name));
	if (SOCKET_ERROR == iRet)
	{
		printf("Get chicken hostName failed ! Error Code:%d\n", WSAGetLastError());
		bSuccess = false;
		return ;
	}
	else
	{
		memcpy(hostName, host_name, sizeof(host_name));  //获取本机名
		struct hostent *phe = gethostbyname(host_name);
		if (!phe)
		{
			printf("Yow! Bad host lookup.");
			bSuccess = false;
		}
		else
		{
			//循环得出本地机器所有IP地址
			WSADATA wsaData = {0};
			char name[255] = {0};
			PHOSTENT hostinfo;
			char *pLocalIP = NULL;
			if (0 == WSAStartup(MAKEWORD(2, 2),&wsaData))
			{
				if (0 == gethostname(name,sizeof(name)))
				{
					if(NULL !=(hostinfo=gethostbyname(name)))
					{
						pLocalIP = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);
						memcpy(szLocalIP, pLocalIP, IPADDR_SIZE);
					}
				}
				WSACleanup();
			}
		}
	}
	*pbSuccess = bSuccess;
}

//获取CPU的核心数
DWORD CSystemInfo::GetProcessorCoreCount()
{
#if (_WIN32_WINNT < 0x0600) // [zyl910] 低版本的Windows SDK没有定义 RelationProcessorPackage 等常量
#define RelationProcessorPackage 3
#define RelationGroup 4
#endif
	typedef BOOL (WINAPI *LPFN_GLPI)(
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
		PDWORD);

	LPFN_GLPI glpi = (LPFN_GLPI) GetProcAddress(GetModuleHandle(TEXT("kernel32")), "GetLogicalProcessorInformation");

	if (NULL == glpi)
		return 0;

	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	DWORD returnLength = 0;
	DWORD processorCoreCount = 0;
	while (true)
	{
		DWORD rc = glpi(buffer, &returnLength);
		if (FALSE == rc)
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if (buffer)
				{
					free(buffer);
				}
				buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
				if (NULL == buffer)
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			break;
		}
	}
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = buffer;
	DWORD byteOffset = 0;
	while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
	{
		switch (ptr->Relationship)
		{
		case RelationProcessorCore:
			++processorCoreCount;
			break;

		default:
			break;
		}
		byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		++ptr;
	}
	free(buffer);
	return processorCoreCount;
}

//获取CPU的主频信息
DWORD CSystemInfo::GetCPULevel()
{
	DWORD dwMHz = 0; 
	CString strPath = _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");//注册表子键路径
	CRegKey regkey;//定义注册表类对象
	LONG lResult;//LONG型变量－反应结果
	lResult=regkey.Open(HKEY_LOCAL_MACHINE, LPCTSTR(strPath), KEY_ALL_ACCESS); //打开注册表键 
	if (ERROR_SUCCESS != lResult)
	{
		printf("无法获取到CPU主频信息\n");
		return dwMHz;
	}
	lResult = regkey.QueryDWORDValue(_T("~MHz"), dwMHz);//获取VendorIdentifier字段值
	if (ERROR_SUCCESS != lResult)
	{
		dwMHz = 0;
	}
	return dwMHz;
}

//是否有摄像头
BOOL CSystemInfo::IsWebCam()
{
	BOOL  bRet = false;
	TCHAR lpszName[100], lpszVer[50];
	for (int i = 0; i < 10 && !bRet; i++)
	{
		bRet = capGetDriverDescription(i, lpszName, sizeof(lpszName),
			lpszVer, sizeof(lpszVer));
	}
	return bRet;
}

//获取磁盘的总大小
DWORD CSystemInfo::GetTotalDiskInfo()
{
	DWORD dwTotal = 0;
	for (wchar_t i = 'B'; i <= 'Z'; i++)  //为什么不从A开始，应该A有可能是软盘，而这不是我们所关心的对象
	{
		unsigned __int64 i32FreeBytesToCaller = 0;
		unsigned __int64 i32TotalBytes = 0;
		unsigned __int64 i32FreeBytes = 0;
		TCHAR szTemp[] = {i, ':', '\0'};
		UINT uType = GetDriveType(szTemp);  //获取磁盘的类型
		switch (uType)
		{
		case DRIVE_FIXED:  //固定硬盘
			{
				BOOL bSuccess = GetDiskFreeSpaceEx(szTemp,
					(PULARGE_INTEGER)&i32FreeBytesToCaller,
					(PULARGE_INTEGER)&i32TotalBytes,
					(PULARGE_INTEGER)&i32FreeBytes);
				if (bSuccess)  //获取到磁盘空间信息
				{
					dwTotal += (double)(i32TotalBytes/1024.0/1024/1024);
				}
				else
				{
					dwTotal += 0.0;
				}		
			}
			break;
		default:          //其他
			continue;
		}
	}
	return dwTotal;
}

UINT32 CSystemInfo::SendSysInfo( IN SOCKET sock, OUT bool* pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	if (INVALID_SOCKET == sock || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}
	bool bSuccess = true;
	SYSTEMINFO_S sysInfo;
	memset(&sysInfo, 0, sizeof(SYSTEMINFO_S));
	sysInfo.OSVer = GetSysVersion();  //获取操作系统版本
	GetInternetIP(sysInfo.szWANIP, &bSuccess);   //获取公网IP
	if (!bSuccess)  //如果获取公网IP失败
	{
		char* szWANIP = "0.0.0.0"; 
		memcpy(sysInfo.szWANIP, szWANIP, sizeof(sysInfo.szWANIP));
	}
	GetHostName(sysInfo.hostName, sysInfo.szLocalIP, &bSuccess);  //获取计算机名和本地IP
	sysInfo.Cam = false;
	DWORD dwCPUCount = GetProcessorCoreCount();   //CPU核心数
	DWORD dwCPULevel = GetCPULevel();  //CPU主频
	CString strCPUInfo = _T("");
	strCPUInfo.Format(_T("%d*%dMHz"), dwCPUCount, dwCPULevel);
	wsprintfW(sysInfo.szCPUInfo, strCPUInfo);
	sysInfo.Cam = (IsWebCam() ? true : false);  //是否有摄像头
	sysInfo.dwDiskSize = GetTotalDiskInfo();  //磁盘空间大小

	MSGINFO_S msgCMD;
	memset(&msgCMD, 0, sizeof(MSGINFO_S));
	msgCMD.Msg_id = CMD_SYSINFO;
	memcpy((char*)msgCMD.context, (char*)&sysInfo, CONTEXT_BUF_SIZE);
	m_moduleSocket.SendCommand(sock, (char*)&msgCMD, sizeof(MSGINFO_S), &bSuccess);
	return result;
}

