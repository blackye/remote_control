#pragma once

//远程命令广播
// CRemoteCmd 对话框
#include "Common.h"
#include "Define_Inc.h"
#include "ModuleSocket.h"
#include "afxwin.h"

class CRemoteCmdDlg : public CDialog
{
	DECLARE_DYNAMIC(CRemoteCmdDlg)

public:
	CRemoteCmdDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CRemoteCmdDlg();

// 对话框数据
	enum { IDD = IDD_DLG_BROADCAST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
	UINT GetIconType();  //获取弹窗的图标
	UINT GetBtnType();  //获取按钮类型
	HICON m_hIcon;
public:
	CStatusBar m_StatusBar;  //状态栏
	CButton m_checkRun;
	virtual BOOL OnInitDialog();
	void DlgInit();
	afx_msg void OnBnClickedBtnOpenWebsite();
	afx_msg void OnBnClickedBtnDownWebsite();
	afx_msg void OnBnClickedBtnSendWnd();
	afx_msg void OnBnClickedBtnPreviewWnd();
	void SendCmdMsg(IN BROADCAST_C tagBroadCast);  //发动消息命令
	void UpDateStatusBar(IN const wchar_t* pRetContent);  //更新状态栏

	CComboBox m_ComboxIcon;
	CComboBox m_ComboxButton;
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
