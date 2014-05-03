#pragma once
#include "afxwin.h"
#include "Common.h"
#include "Define_Inc.h"
#include "ModuleSocket.h"

// CReadPswDlg 对话框

class CReadPswDlg : public CDialog
{
	DECLARE_DYNAMIC(CReadPswDlg)

public:
	CReadPswDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CReadPswDlg();

// 对话框数据
	enum { IDD = IDD_DLG_READPSW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editCtrl;
	afx_msg void OnBnClickedBtnGetpsw();
private:
	SOCKET m_socket;
	CModuleSocket m_moduleSocket;
	CWnd *m_pWndMsg;   //父窗口指针
	CString m_strContent;
public:
	virtual BOOL OnInitDialog();
	void DlgInit();
	void SetReadPswInfo(IN READPSWDATA_C tagRPDATA);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
