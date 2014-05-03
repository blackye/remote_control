#pragma once

#include "Common.h"
#include "ModuleSocket.h"

class CCmdShell
{
public:
	CCmdShell(void);
	~CCmdShell(void);
public:
	void SetCmdSocket(SOCKET sock);
	void Cmd_Init();  //初始化，创建管道
	void Cmd_Send();   //读取通道数据并发送信息
    void Cmd_Recv(IN char* pRecvBuf);  //接受命令并写入通道中
private:
	STARTUPINFO m_si;     //创建进程需要的结构体
	HANDLE m_hReadPipe;   //读通道
	HANDLE m_hWritePipe;  //写通道
    HANDLE m_hReadFile;   //读文件
	HANDLE m_hWriteFile;  //写文件
	PROCESS_INFORMATION m_pi;   //进程的结构体
	SOCKET m_socket;
	CModuleSocket m_moduleSocket;
};
