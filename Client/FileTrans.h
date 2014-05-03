#pragma once
#include "afxcmn.h"
#include "Common.h"
#include "ModuleSocket.h"

// CFileTrans 对话框

class CFileTrans : public CDialog
{
	DECLARE_DYNAMIC(CFileTrans)

public:
	CFileTrans(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CFileTrans();

// 对话框数据
	enum { IDD = IDD_DLG_FILETRANS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	SOCKET m_socket;
	bool m_bBeginWork;   //是否开始工作(传输)
	CFile* m_pFile;     //文件创建
	CModuleSocket m_moduleSocket;
	CWnd *m_pWndMsg;   //父窗口指
    DWORD m_dwStartTime;
public:
	CListCtrl m_listCtrl;
	CImageList m_ImageList;
	CProgressCtrl m_filePro;  //进度条
	void DlgInit();   //对话框的初始化
	void OnAddFileList(IN int nType, IN const CString strSourcePath, IN const CString strDesPath, IN const CString strSize, IN const __int64* puSize);
	virtual BOOL OnInitDialog();
	static DWORD WINAPI BeginToTransFileProc(LPVOID lpParameter);   //开始传输文件
	void TransFile();  //传输文件
	void SetFileData(IN DOWNFILEDATA_C tagDownFileData);
	CString GetFileTranSpeed(DWORD size,DWORD time);
	void SendUploadFileInfo(IN SOCKET sock);  //上传文件时，发送文件数据库
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
