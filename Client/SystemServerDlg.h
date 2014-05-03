#pragma once


// CSystemServerDlg 对话框

class CSystemServerDlg : public CDialog
{
	DECLARE_DYNAMIC(CSystemServerDlg)

public:
	CSystemServerDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CSystemServerDlg();

// 对话框数据
	enum { IDD = IDD_DLG_SERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	SOCKET m_socket;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
