#pragma once

#include "../Common/XPictureBox.h"
#include "Common.h"
#include "Define_Inc.h"
#include "ModuleSocket.h"
#include "../Common/AviFile.h"
#include "../Common/zlib.h"
#include "../Common/zconf.h"
#pragma comment(lib, "../Common/zlib1.lib")
// CVideoDlg 对话框

class CVideoDlg : public CDialog
{
	DECLARE_DYNAMIC(CVideoDlg)

public:
	CVideoDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CVideoDlg();

// 对话框数据
	enum { IDD = IDD_DLG_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CStatusBarCtrl    m_wndStatusBar;
	BOOL VideoStart();
	void VideoStop();
	XPictureBox m_PicBox;

	DECLARE_MESSAGE_MAP()
private:
	SOCKET m_socket;
	HANDLE m_hRecvVideoThread;
	CModuleSocket m_moduleSocket;
    LPBITMAPINFO m_pBmpheader;
	BYTE* m_pCompress;  //存放数据的指针(压缩后的)
	BYTE* m_pUnCompress;  //存放数据的指针(未压缩的)
	CAviFile* m_pAviFile;
public:
	virtual BOOL OnInitDialog();
	void DlgInit();
	DWORD RecvVideo(IN VIDEODATA_C tagVideoData);
	void SendVideoStartMsg();
	HBITMAP GetBitmapFromData(LPBITMAPINFO lpBmpInfo, BYTE* pDibData);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedBtnRecords();
	afx_msg void OnBnClickedBtnRecorde();
	void StatusTextOut(int iPane,LPCTSTR ptzFormat, ...);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
