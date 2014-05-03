#pragma once

//桌面管理
// CDesktopDlg 对话框
#include "ModuleSocket.h"
#include "Common.h"
#include "Define_Inc.h"

class CDesktopDlg : public CDialog
{
	DECLARE_DYNAMIC(CDesktopDlg)

public:
	CDesktopDlg(CWnd* pParent = NULL, SOCKET sock = NULL, CString strTitle = NULL);   // 标准构造函数
	virtual ~CDesktopDlg();

// 对话框数据
	enum { IDD = IDD_DLG_DESKTOP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
	char* m_pBuffer;
	CString m_strCilpBoard;  //剪贴板内容
	CString m_strTitle;  //窗口标题
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnHideMouse();
	afx_msg void OnBnClickedBtnAllowMouse();
	void SendCommandMsg(IN const DESKTOPINFO_C tagDesktopInfo);
	DWORD SetClipBoardContext(IN const CLIPBOARD_C tagClipBoard);    //显示远程剪贴板的内容
	afx_msg void OnBnClickedBtnHideDesktop();
	afx_msg void OnBnClickedBtnShowDesktop();
	afx_msg void OnBnClickedBtnHideTaskbar();
	afx_msg void OnBnClickedBtnShowTaskbar();
	afx_msg void OnBnClickedBtnViewClip();
	CStringW CStrA2CStrW(const CStringA &cstrSrcA);  // CStringA转CStringW  Unicode环境下
	afx_msg void OnBnClickedBtnSaveClip();
};
