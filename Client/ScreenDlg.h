#pragma once

#include "Common.h"
#include "ModuleSocket.h"
// CScreenDlg 对话框

class CScreenDlg : public CDialog
{
	DECLARE_DYNAMIC(CScreenDlg)

public:
	CScreenDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CScreenDlg();

// 对话框数据
	enum { IDD = IDD_DLG_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void GetScreenDataInfo(BMPDATA_C tagBmpData) /*显示图像 */;
private:
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
	BITMAPINFO *m_pBMPINFO;  //存放位图头信息的指针
	BYTE* m_pData;  //存放屏幕数据的指针
	BMPDATA_C m_TagBmpData;

protected:
	virtual void PostNcDestroy();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	void DlgInit();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
