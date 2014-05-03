#pragma once
#include "stdafx.h"
#include "Common.h"

class CModuleSocket
{
public:
	CModuleSocket(void);
	~CModuleSocket(void);
    
	UINT32 ConnectServer(IN const char* pIpAddr,OUT SOCKET* pSocket, OUT bool* pbSuccess);  //连接服务器

	UINT32 SendCommand(IN const SOCKET socket, 
		IN char* szBuf,
		IN int bytes,
		OUT bool* pbSuccess);  //发送消息

	UINT32 RecvCommand(IN const SOCKET socket,
		OUT char* szBuf,
		OUT int bytes,
		OUT bool *pbSuccess);   //接受消息

	void Clean();

public:
	UINT  m_nPort;   //连接端口
};
