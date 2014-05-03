#pragma once

//创建目录
// CFolderDlg 对话框

class CFolderDlg : public CDialog
{
	DECLARE_DYNAMIC(CFolderDlg)

public:
	CFolderDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFolderDlg();

// 对话框数据
	enum { IDD = IDD_DLG_FOLDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CString m_strFolderName;  //文件目录名
public:
	bool m_bOk;
	CString GetFolderName() {return m_strFolderName;}
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
