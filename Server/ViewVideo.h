#pragma once

#include "ModuleSocket.h"
#include "Common.h"
#include "../Common/VideoCap.h"
#include "../Common/zlib.h"
#include "../Common/zconf.h"
#pragma comment(lib, "../Common/zlib1.lib")

class CViewVideo
{
public:
	CViewVideo(void);
	~CViewVideo(void);
	bool VideoInit();  //初始化摄像头数据
//	void SendVideoInfo(); //循环发送视屏数据
	void SendViedoHeaderInfo();   //发送视屏头数据
	void SendVideoData();  //发送视屏数据
	void CleanData();  //清空数据
public:
	SOCKET m_socket;
	CModuleSocket m_moduleSocket;
	CVideoCap m_Cap;
	HANDLE m_hThread;  //线程句柄
//	bool m_bStop;  //是否关闭
	SYSTEMTIME m_dwLastSend;
	BYTE* m_pDataCompress;  //视屏数据块
};
