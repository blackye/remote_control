#pragma once

//语音监听类
#include "Common.h"
#include "ModuleSocket.h"
#include "../Common/Audio.h"

class CAudioManager
{
public:
	CAudioManager(void);
	~CAudioManager(void);
	void SendRecordBuffer();   //发送语音信息
	void RecvRecordBuffer();   //接受远程的语音信息
	bool Initialize();  //初始化

public:
	SOCKET m_socket;
    bool m_bWorking;
	HANDLE m_hSendThread;
	HANDLE m_hRecvThread;
private:
	CModuleSocket m_moduleSocket;
	CAudio	*m_lpAudio;
	static DWORD WINAPI SendAudioDataProc(LPVOID lpParameter);
	static DWORD WINAPI RecvAudioDataProc(LPVOID lpParameter);
};
