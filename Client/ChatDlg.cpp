// ChatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ChatDlg.h"
#include "BroilerData.h"

// CChatDlg 对话框

static const int nHotkey  = 2000;
 
IMPLEMENT_DYNAMIC(CChatDlg, CDialog)

CChatDlg::CChatDlg(CWnd* pParent /*=NULL*/ , SOCKET sock, CString strIP)
	: CDialog(CChatDlg::IDD, pParent)
{
	m_socket = sock;
	m_strChatMsg = _T("");
	m_strIP = strIP;
	m_bfirstSend = true;
	m_bQuit = false;  //默认服务端是没有退出
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CHATMSG);
}

CChatDlg::~CChatDlg()
{
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CHAT_RECV, m_chatRecv);
	DDX_Control(pDX, IDC_EDIT_CHAT_SEND, m_chatSend);
}


BEGIN_MESSAGE_MAP(CChatDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CChatDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_HOTKEY,&CChatDlg::OnHotKey)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_SEND_MSG, &CChatDlg::OnBnClickedBtnSendMsg)
END_MESSAGE_MAP()


// CChatDlg 消息处理程序

BOOL CChatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetIcon(m_hIcon, FALSE);
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CChatDlg::DlgInit()
{
	RegisterHotKey(this->GetSafeHwnd(), nHotkey, MOD_CONTROL, VK_RETURN);  //注册快捷键
	CString strTitle = _T("信使服务  \\\\");
	strTitle += m_strIP;
	SetWindowText(strTitle);
}

LRESULT CChatDlg::OnHotKey( WPARAM wParam,LPARAM lParam )
{
	if (nHotkey == wParam)
	{
		SendChatMsg();   //如果按下了 Ctrl + Enter , 则发送当前的消息
	}
	return 0;
}

void CChatDlg::OnBnClickedBtnSendMsg()
{
	// TODO: 在此添加控件通知处理程序代码
	SendChatMsg();  //发送消息
}

void CChatDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CChatDlg::SendChatMsg()
{
	CString strSendMsg = _T("");  //发送的消息
	GetDlgItemText(IDC_EDIT_CHAT_SEND, strSendMsg);
	if (strSendMsg.IsEmpty())  //发送的是空
	{
		return ;
	}
	CHATMSG_C tagChatMsg;
	memset(&tagChatMsg, 0, sizeof(CHATMSG_C));
	tagChatMsg.bfirstSend = m_bfirstSend;
	m_bfirstSend = false;
	tagChatMsg.bClose = false;
	wsprintfW(tagChatMsg.szChatMsg, strSendMsg);

	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_CHATMSG;        //发送聊天信息
	memcpy((char*)tagMsgInfo.context, (char*)&tagChatMsg, sizeof(CHATMSG_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	if (bSuccess)
	{
		m_strChatMsg += _T("YourSelf: ");
		m_strChatMsg += strSendMsg;
		m_strChatMsg += _T("\r\n\r\n--------------------END---------------------\r\n\r\n");
		m_bQuit = false;   //不管是否退出，只要发送了消息，我断言对方的窗口一定没退出。
	}
	m_chatRecv.SetWindowText(m_strChatMsg);
	m_chatRecv.SetSel(-1);
	m_chatSend.SetWindowText(_T(""));  //清空
}

void CChatDlg::RecvChatMsg(IN CHATMSG_C tagChatMsg)
{
	CString strRecvMsg = _T("");
	if (false == tagChatMsg.bClose)
	{
		strRecvMsg.Format(_T("小白: %s"), tagChatMsg.szChatMsg);
		m_strChatMsg += strRecvMsg;
		m_bQuit = false;
	}
	else   //服务端关闭聊天了
	{
		CString strWarning = _T("服务端: ");
		strWarning += m_strIP;
		strWarning += _T("关闭了信使服务!");
		m_strChatMsg += strWarning;
		m_bfirstSend = true;  //恢复了第一次发送信息的状态
		m_bQuit = true;  //服务端退出了
	}
	m_strChatMsg += _T("\r\n\r\n--------------------END---------------------\r\n\r\n");
	m_chatRecv.SetWindowText(m_strChatMsg);
	m_chatRecv.SetSel(-1);
}

void CChatDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pChatDlg = NULL;
}

void CChatDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
    if (!m_bQuit)  //如果服务端没有退出，而我此时点击关闭，则将服务端同时也关闭。如果服务端退出了，则不用再发送退出消息
	{
		CHATMSG_C tagChatMsg;
		memset(&tagChatMsg, 0, sizeof(CHATMSG_C));
		tagChatMsg.bfirstSend = false;
		tagChatMsg.bClose = true;

		MSGINFO tagMsgInfo;
		memset(&tagMsgInfo, 0, sizeof(MSGINFO));
		tagMsgInfo.nMsgCmd = CMD_CHATMSG;        //发送聊天信息
		memcpy((char*)tagMsgInfo.context, (char*)&tagChatMsg, sizeof(CHATMSG_C));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
		Sleep(100);
	}
	UnregisterHotKey(GetSafeHwnd(), nHotkey);//注销Ctrl+Enter键 
	DestroyWindow();
	delete this;
}

HBRUSH CChatDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if (IDC_STATIC_CHAT_TIPS == pWnd->GetDlgCtrlID() && CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SetTextColor(RGB(255,0,0));   //字体颜色
		pDC->SetBkMode(TRANSPARENT);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

BOOL CChatDlg::PreTranslateMessage(MSG* pMsg)
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
