#include "StdAfx.h"
#include "ModuleSocket.h"
#include "Common.h"

CModuleSocket::CModuleSocket(void)
{
}

CModuleSocket::~CModuleSocket(void)
{
}

UINT32 CModuleSocket::SendCommand(IN const SOCKET socket, 
				                  IN char* szBuf,
				                  IN int bytes,
				                  OUT bool* pbSuccess)
{
	UINT32 result = ERROR_SUCCESS;
	if (INVALID_SOCKET == socket || 0 == bytes || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}
	bool bSuccess = true;
	const char* pCurBuf = szBuf;  //当前已经发送完毕的缓冲区数据
	try
	{
		while (bytes > 0)   //没有发送完毕
		{
			int nTotal = send(socket, (char*)pCurBuf, bytes, 0);
			if (SOCKET_ERROR == nTotal)  //发送失败
			{
				result = WSAGetLastError();
				bSuccess = false;
				throw result;
			}
			else if (0 == nTotal)  //发送完毕
			{
				break;
			}
			bytes -= nTotal;
			pCurBuf += nTotal;
		}
	}
	catch (UINT32)
	{
	}
	*pbSuccess = bSuccess;
	return result;
}

UINT32 CModuleSocket::RecvCommand( IN const SOCKET socket, OUT char* szBuf, OUT int bytes, OUT bool *pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	if (INVALID_SOCKET == socket || NULL == szBuf || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}
	bool bSuccess = true;
    char* pCurBuf = szBuf;  //当前已经发送完毕的缓冲区数据
	try
	{
		while (bytes > 0)   //没有发送完毕
		{
			int nTotal = recv(socket, (char*)pCurBuf, sizeof(bytes), 0);
			if (SOCKET_ERROR == nTotal)  //发送失败  10054 远程主机强迫关闭了一个现有的连接
			{
				result = WSAGetLastError();
				bSuccess = false;
				throw result;
			}
			else if (0 == nTotal)  //发送完毕
			{
				break;
			}
			bytes -= nTotal;
			pCurBuf += nTotal;
		}
	}
	catch (UINT32)
	{
	}
	*pbSuccess = bSuccess;
	return result;
}

void CModuleSocket::Clean()
{
	::WSACleanup();
}