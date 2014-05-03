#pragma once

#include "Common.h"
#include "ModuleSocket.h"
#include <math.h>
#include <afxwin.h>

//屏幕操作类
class CScreen
{
public:
	CScreen(void);
	~CScreen(void);
public:
	void GetScreen();  //获取屏幕显示信息
	void SendBmpHeaderInfo();  //发送位图头信息
	void SendBmpData();   //发送位图数据
	void SendScreenData();  //发送屏幕数据
	void CleanData();  //清空数据
	SOCKET m_socket;
	HANDLE m_hThread;  //线程句柄
	bool m_bFlag;
private:
	double m_panelsize;
	int m_nHeadTotal;  //位图头的大小
	BITMAPINFO *m_pBMPINFO;
	BYTE* m_pData;  //发送的数据
	UINT m_totalSize;  //位图总大小
	CModuleSocket m_moduleSocket;
};
