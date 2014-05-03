#pragma once
#include "afxwin.h"
#include "Resource.h"
#include "ModuleSocket.h"
// CChatMsgDlg 对话框

class CChatMsgDlg : public CDialog
{
	DECLARE_DYNAMIC(CChatMsgDlg)

public:
	CChatMsgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChatMsgDlg();

// 对话框数据
	enum { IDD = IDD_DLG_CHATMSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;
	SOCKET m_socket;
	CModuleSocket m_moduleSocket;
	CEdit m_chatRecv;
	CEdit m_chatSend;
	afx_msg void OnBnClickedBtnSend();
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);
public:	
	void SendChatMsg();  //发送消息
	void RecvChatMsg(IN CHATMSG_S tagChatMsg);  //接受消息
	void SetSendSocket(IN SOCKET l_socket);  //设置发送的Socket
	afx_msg void OnBnClickedCancel();
	CString m_strChatMsg;
	bool m_bForceQuit;  //是否强制退出
protected:
	virtual void OnCancel();
};
