// SystemServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "SystemServerDlg.h"
#include "SystemManager.h"

// CSystemServerDlg 对话框

IMPLEMENT_DYNAMIC(CSystemServerDlg, CDialog)

CSystemServerDlg::CSystemServerDlg(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CSystemServerDlg::IDD, pParent)
{
	m_socket = sock;
}

CSystemServerDlg::~CSystemServerDlg()
{
}

void CSystemServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSystemServerDlg, CDialog)
END_MESSAGE_MAP()


// CSystemServerDlg 消息处理程序

BOOL CSystemServerDlg::PreTranslateMessage(MSG* pMsg)
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

void CSystemServerDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	DestroyWindow();
	delete this;
}

void CSystemServerDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	((CSystemManager*)this->m_pParentWnd)->m_pSysServerDlg = NULL; 
	CDialog::PostNcDestroy();
}
