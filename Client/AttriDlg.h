#pragma once

//文件或文件夹的属性对话框
// CAttriDlg 对话框

#include "Common.h"
#include "afxwin.h"

class CAttriDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttriDlg)

public:
	CAttriDlg(CWnd* pParent = NULL, ATTRIBUTE_C* tagArrtibute = NULL);   // 标准构造函数
	virtual ~CAttriDlg();

// 对话框数据
	enum { IDD = IDD_DLG_ATTRIBUTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	void DlgInit();
	ATTRIBUTE_C m_tagArrtibute;
public:
	CStatic m_picCtrl;
	CButton m_ReadOnlyCheck;
	CButton m_HideCheck;
	CButton m_ArchiveCheck;
	CButton m_SystemCheck;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
