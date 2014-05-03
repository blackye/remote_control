// ChatMsgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ChatMsgDlg.h"
#include "Common.h"

static const int nHotkey  = 2000;
// CChatMsgDlg 对话框

IMPLEMENT_DYNAMIC(CChatMsgDlg, CDialog)

CChatMsgDlg::CChatMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatMsgDlg::IDD, pParent)
{
	m_strChatMsg = _T("");
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CHATMSG);
	m_bForceQuit = false;
}

CChatMsgDlg::~CChatMsgDlg()
{
}

void CChatMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CHAT_RECV, m_chatRecv);
	DDX_Control(pDX, IDC_EDIT_CHAT_SEND, m_chatSend);
}


BEGIN_MESSAGE_MAP(CChatMsgDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_SEND, &CChatMsgDlg::OnBnClickedBtnSend)
	ON_MESSAGE(WM_HOTKEY,&CChatMsgDlg::OnHotKey)
	ON_BN_CLICKED(IDCANCEL, &CChatMsgDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CChatMsgDlg 消息处理程序

BOOL CChatMsgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(m_hIcon, FALSE);
	// TODO:  在此添加额外的初始化
	BOOL bRet = RegisterHotKey(this->GetSafeHwnd(), nHotkey, MOD_CONTROL, VK_RETURN);  //这里注册快捷键失败，原因是窗口句柄
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CChatMsgDlg::OnBnClickedBtnSend()
{
	// TODO: 在此添加控件通知处理程序代码
	SendChatMsg();
}

void CChatMsgDlg::SetSendSocket(IN SOCKET l_socket)
{
	m_socket = l_socket;
}

LRESULT CChatMsgDlg::OnHotKey( WPARAM wParam,LPARAM lParam )
{
	if (nHotkey == wParam)
	{
		SendChatMsg();   //如果按下了 Ctrl + Enter , 则发送当前的消息
		m_chatSend.SetWindowText(_T(""));  //清空
	}
	return 0;
}

void CChatMsgDlg::SendChatMsg()
{
	CString strSendMsg = _T("");  //发送的消息
	GetDlgItemText(IDC_EDIT_CHAT_SEND, strSendMsg);
	if (strSendMsg.IsEmpty())  //发送的是空
	{
		return ;
	}
	CHATMSG_S tagChatMsg;
	memset(&tagChatMsg, 0, sizeof(CHATMSG_S));
	tagChatMsg.bfirstSend = false;
	tagChatMsg.bClose = false;   //用户是否选择关闭对话框了
	wsprintfW(tagChatMsg.szChatMsg, strSendMsg);

	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_CHATMSG;        //发送聊天信息
	memcpy((char*)tagMsgInfo.context, (char*)&tagChatMsg, sizeof(CHATMSG_S));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	if (bSuccess)
	{
		m_strChatMsg += _T("YourSelf: ");
		m_strChatMsg += strSendMsg;
		m_strChatMsg += _T("\r\n\r\n--------------------END---------------------\r\n\r\n");
	}
	m_chatRecv.SetWindowText(m_strChatMsg);
	m_chatSend.SetWindowText(_T(""));  //清空
	m_chatRecv.SetSel(-1);
}

void CChatMsgDlg::RecvChatMsg(IN CHATMSG_S tagChatMsg)
{
	CString strRecvMsg = _T("");
	if (false == tagChatMsg.bClose)
	{
		strRecvMsg.Format(_T("Hacker: %s"), tagChatMsg.szChatMsg);
		m_strChatMsg += strRecvMsg;
		m_strChatMsg += _T("\r\n\r\n--------------------END---------------------\r\n\r\n");
		m_bForceQuit = false;
	}
	else   //接送到关闭聊天的信息
	{
		m_bForceQuit = true;   //被客户端强制退出
		OnCancel();  //关闭
		return;
	}
	m_chatRecv.SetWindowText(m_strChatMsg);
	m_chatRecv.SetSel(-1);
}

void CChatMsgDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	UnregisterHotKey(GetSafeHwnd(), nHotkey);//注销Alt+1键
	OnCancel();
}

void CChatMsgDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	//发送关闭消息
	if (!m_bForceQuit)
	{
		CHATMSG_S tagChatMsg;
		memset(&tagChatMsg, 0, sizeof(CHATMSG_S));
		tagChatMsg.bfirstSend = false;
		tagChatMsg.bClose = true;   //关闭了对话框

		MSGINFO_S tagMsgInfo;
		memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
		tagMsgInfo.Msg_id = CMD_CHATMSG;        //发送聊天信息
		memcpy((char*)tagMsgInfo.context, (char*)&tagChatMsg, sizeof(CHATMSG_S));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	}
    Sleep(100);
	CDialog::OnCancel();
}
