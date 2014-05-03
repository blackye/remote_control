#pragma once
#include "afxwin.h"
#include "Common.h"
#include "Define_Inc.h"
#include "ModuleSocket.h"

//远程TELNET
// CCmdShell 对话框

class CCmdShell : public CDialog
{
	DECLARE_DYNAMIC(CCmdShell)

public:
	CCmdShell(CWnd* pParent = NULL, SOCKET l_socket = NULL, CString strTitle = _T(""));   // 标准构造函数
	virtual ~CCmdShell();

// 对话框数据
	enum { IDD = IDD_DLG_TELNET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strTitle;
	CFont * m_pfont;
	bool m_bReadOnly;
	CBrush m_Brush;
	CEdit m_cmdRecvCtrl;
	CEdit m_cmdSendCtrl;
	CStatusBar m_cmdStatusBar;  //CMDShell状态栏
	afx_msg void OnBnClickedBtnCmdSend();
	virtual BOOL OnInitDialog();
	void DlgInit();
private:
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
	CString m_strShowMsg;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void CStringToChar(IN CString str, OUT char* w);  //CString转char
	CString CharToCString(char* result);  //char转CString

	void SendCommond();
	void SetReturnInfo(IN COMMOND_C tagCmdInfo);   //将回显信息显示在页面上
protected:
	virtual void PostNcDestroy();
	virtual void OnCancel();
};
