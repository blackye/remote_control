#include "StdAfx.h"
#include <iostream>
#include "ModuleSocket.h"
using namespace std;

CModuleSocket::CModuleSocket(void)
{
}

CModuleSocket::~CModuleSocket(void)
{
}

UINT32 CModuleSocket::ConnectServer( IN const char* pIpAddr, OUT SOCKET* pSocket, OUT bool* pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	if (NULL == pIpAddr || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}

	bool bSuccess = true;
	SOCKET socket = INVALID_SOCKET;
	try
	{
		WSADATA wsadata = {0};
		int iRet = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (iRet)
		{
			cout << "socket版本初始信息失败!" << endl;
			result = WSAGetLastError();
			bSuccess = false;
			throw result;
		}
	    socket = ::socket(AF_INET, SOCK_STREAM, 0);
		if (INVALID_SOCKET == socket)
		{
			cout << "socket初始化失败!" << endl;
			result = WSAGetLastError();
			bSuccess = false;
			throw result;
		}
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.S_un.S_addr = inet_addr(pIpAddr);
		serverAddr.sin_port = htons(m_nPort);

		iRet = connect(socket, (sockaddr*)&serverAddr, sizeof(sockaddr_in));
		if (SOCKET_ERROR == iRet)
		{
			cout << "连接服务器端失败!" << endl;
			result = WSAGetLastError();
			bSuccess = false;
			throw result;
		}
		else
		{
			cout << "连接服务器成功!" << endl;
			*pSocket = socket;
		}
	}
	catch (UINT32)
	{
	}
	*pbSuccess = bSuccess;
	return result;
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
	if (INVALID_SOCKET == socket || NULL == pbSuccess)
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
			int nTotal = recv(socket, (char*)pCurBuf, bytes, 0);
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

void CModuleSocket::Clean()
{
	::WSACleanup();
}