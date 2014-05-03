#pragma once

//文件（夹）重命名
// CFileRenDlg 对话框

class CFileRenDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileRenDlg)

public:
	CFileRenDlg(CWnd* pParent = NULL, CString strFileName = _T(""));   // 标准构造函数
	virtual ~CFileRenDlg();

// 对话框数据
	enum { IDD = IDD_DLG_RENAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	bool m_bOk;
	CString GetFileName() {return m_strFileName;}
private:
	CString m_strFileName;  //新文件(夹)名称
	CString m_strDefault;   //旧文件(夹)名称
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
