// FolderDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "FolderDlg.h"


// CFolderDlg 对话框

IMPLEMENT_DYNAMIC(CFolderDlg, CDialog)

CFolderDlg::CFolderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFolderDlg::IDD, pParent)
{

}

CFolderDlg::~CFolderDlg()
{
}

void CFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFolderDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFolderDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFolderDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CFolderDlg 消息处理程序

BOOL CFolderDlg::PreTranslateMessage(MSG* pMsg)
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

void CFolderDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bOk = true;
	CString strFolder = _T("");
	GetDlgItemText(IDC_EDIT_FOLDER, strFolder);
	if (_T("") == strFolder)
	{
		::MessageBox(this->m_hWnd, _T("请输入有效的目录名称!"), _T("警告"), MB_OK | MB_ICONWARNING);
		return ;
	}
	else
	{
		m_strFolderName = strFolder;
	}
	OnOK();
}

void CFolderDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bOk = false;
	OnCancel();
}

