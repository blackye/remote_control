// FileRenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "FileRenDlg.h"


// CFileRenDlg 对话框

IMPLEMENT_DYNAMIC(CFileRenDlg, CDialog)

CFileRenDlg::CFileRenDlg(CWnd* pParent /*=NULL*/, CString strFileName)
	: CDialog(CFileRenDlg::IDD, pParent)
{
	m_strDefault = strFileName;
}

CFileRenDlg::~CFileRenDlg()
{
}

void CFileRenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFileRenDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFileRenDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFileRenDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CFileRenDlg 消息处理程序
BOOL CFileRenDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CFileRenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	GetDlgItem(IDC_EDIT_FILE_RENAME)->SetWindowText(m_strDefault);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFileRenDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bOk = true;
	CString strFolder = _T("");
	GetDlgItemText(IDC_EDIT_FILE_RENAME, strFolder);
	if (_T("") == strFolder)
	{
		::MessageBox(this->m_hWnd, _T("请输入有效的名称!"), _T("警告"), MB_OK | MB_ICONWARNING);
		return ;
	}
	else
	{
		m_strFileName = strFolder;
	}
	OnOK();
}

void CFileRenDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bOk = false;
	OnCancel();
}

