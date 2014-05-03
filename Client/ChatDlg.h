#pragma once
#include "afxwin.h"
#include "Common.h"
#include "ModuleSocket.h"
#include "Define_Inc.h"

//聊天信使服务
// CChatDlg 对话框

class CChatDlg : public CDialog
{
	DECLARE_DYNAMIC(CChatDlg)

public:
	CChatDlg(CWnd* pParent = NULL, SOCKET sock = NULL,  CString strIP = NULL);   // 标准构造函数
	virtual ~CChatDlg();

// 对话框数据
	enum { IDD = IDD_DLG_CHAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;
	CEdit m_chatRecv;
	CEdit m_chatSend;
	SOCKET m_socket;
    CModuleSocket m_moduleSocket;
	bool m_bfirstSend;  //是否第一次发送

public:
	virtual BOOL OnInitDialog();
	void DlgInit();
	void SendChatMsg();   //发送信息
	void RecvChatMsg(IN CHATMSG_C tagChatMsg);  //接受信息

protected:
	virtual void PostNcDestroy();
	virtual void OnCancel();
	afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);
public:
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CString m_strChatMsg;  //聊天记录
	CString m_strIP;   //对方的IP
	bool m_bQuit;     //服务端是否退出了
	afx_msg void OnBnClickedBtnSendMsg();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
