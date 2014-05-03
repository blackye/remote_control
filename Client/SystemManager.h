#pragma once
#include "afxcmn.h"
#include "Common.h"
#include "Define_Inc.h"
#include "ModuleSocket.h"
#include "ProgressDlg.h"
#include "SystemServerDlg.h"
#include "ReadPswDlg.h"

//系统管理对话框(包含进程、服务)
// CSystemManager 对话框

class CSystemManager : public CDialog
{
	DECLARE_DYNAMIC(CSystemManager)

public:
	CSystemManager(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CSystemManager();

// 对话框数据
	enum { IDD = IDD_DLG_SYSTEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CImageList m_ImageTab;
	CTabCtrl m_sysTab;
	virtual BOOL OnInitDialog();
	void DlgInit();
	CProgressDlg* GeProgressPoint() { return m_progressDlg; }  //获取进程信息指针
	CSystemServerDlg* GetSysServerPoint() { return m_pSysServerDlg; }  //获取系统服务指针
	CReadPswDlg* GetReadPswPoint()  {return m_pReadPswSlg;}
	CStatusBar m_sysStatusBar;   //状态栏
	CProgressDlg* m_progressDlg;  //进程管理类
	CSystemServerDlg* m_pSysServerDlg;  //系统服务类
	CReadPswDlg*  m_pReadPswSlg;   //帐号密码管理类
private:
	HICON m_hIcon;
	SOCKET m_socket;
	CModuleSocket m_moduleSocket;  
public:
	afx_msg void OnTcnSelchangeTabSystem(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void PostNcDestroy();
	virtual void OnCancel();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
