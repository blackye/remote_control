#pragma once

//自定义socket操作类
class CModuleSocket
{
public:
	CModuleSocket(void);
	~CModuleSocket(void);

	UINT32 SendCommand(IN const SOCKET socket, 
		               IN char* szBuf,
					   IN int bytes,
					   OUT bool* pbSuccess);  //发送消息

	UINT32 RecvCommand(IN const SOCKET socket,
		               OUT char* szBuf,
					   OUT int bytes,
					   OUT bool *pbSuccess);   //接受消息

	void Clean();
};
