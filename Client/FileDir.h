#pragma once

#include "Config.h"

// CFileDir 对话框

class CFileDir : public CDialog
{
	DECLARE_DYNAMIC(CFileDir)

public:
	CFileDir(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFileDir();

// 对话框数据
	enum { IDD = IDD_DLG_DIRLOCATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnBrowser();
	void GetSelectDirectoryPath( const IN CString strDlgTitle, OUT CString& strPath );
	void CheckFolderIsExist(IN const CString strPath, OUT bool* pbExist);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	bool m_bOk;
	CString GetLocationPath() {return m_strLoactionPath;}
private:
	CString m_strLoactionPath;  //下载保存的路径
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
