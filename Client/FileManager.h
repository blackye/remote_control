#pragma once

#include "Resource.h"
#include "afxcmn.h"
#include "FileView.h"
#include "FileTrans.h"
// CFileManager 对话框

class CFileManager : public CDialog
{
	DECLARE_DYNAMIC(CFileManager)

public:
	CFileManager(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CFileManager();

// 对话框数据
	enum { IDD = IDD_DLG_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	HICON m_hIcon;
	SOCKET m_socket;  //通信socket
	CRect m_rect;
	
public:
	void InitDlg();   //初始化
	CFileView* m_pFileView;
	CFileTrans* m_pFileTrans;
	CTabCtrl m_tabCtrl;  //选项卡
	CStatusBar m_fileStatusBar;  //文件状态栏
	CImageList m_ImageTab;   //选项卡图标列表
	virtual BOOL OnInitDialog();
	CFileView* GetFileViewPoint() {return this->m_pFileView;}  //得到文件视图对话框的指针
	CFileTrans* GetFileTransPoint() {return this->m_pFileTrans;}  //得到文件传输对话框的指针
	afx_msg void OnTcnSelchangeTabFile(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
