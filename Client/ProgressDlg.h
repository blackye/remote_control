#pragma once
#include "afxcmn.h"
#include "Common.h"
#include "Define_Inc.h"
#include "ModuleSocket.h"

//进程对话框
// CProgressDlg 对话框

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CProgressDlg();

// 对话框数据
	enum { IDD = IDD_DLG_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CBitmap m_MenuBmp[2];
	CListCtrl m_proListCtrl;  //List控件
private:
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
    CWnd *m_pWndMsg;   //父窗口指针
	bool m_bRefresh;   //是否是刷新列表
public:
	virtual BOOL OnInitDialog();
	void DlgInit();
	void SetProcessInfo(IN PROGRESS_C tagProInfo);  //设置进程信息
	CString GetProcessLevel(IN int proLevel);  //获取进程的优先级
	void UpDataStatusBar(IN CString strLeft = NULL, IN CString strRight = NULL);  //更新状态栏
	void SetRefresh(bool bRefresh)  {m_bRefresh = bRefresh;}  //设置刷新状态
	afx_msg void OnNMRClickListProgress(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListProgress(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnProcTaskkill();
	afx_msg void OnProcRefresh();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
