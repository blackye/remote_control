#pragma once

//语音监听对话框类
// CAudioDlg 对话框
#include "ModuleSocket.h"
#include "Define_Inc.h"
#include "../Common/Audio.h"
#include "Common.h"

class CAudioDlg : public CDialog
{
	DECLARE_DYNAMIC(CAudioDlg)

public:
	CAudioDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CAudioDlg();

// 对话框数据
	enum { IDD = IDD_DLG_AUDIO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CAudio m_Audio;
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
	CString m_strIP;
public:
	virtual BOOL OnInitDialog();
	void DlgInit();
	DWORD RecvAudioData(IN AUDIODATA_C tagAudioData);
    BYTE* m_pData;
	HANDLE m_hRecvThread;  //接受线程函数句柄
	HANDLE m_hWorkThread; //发送本地语音的线程函数句柄
	static DWORD WINAPI WorkThreadProc(LPVOID lpParameter);  //发送本地的语音到远程
	static DWORD WINAPI RecvAudioProc(LPVOID lpParameter);  //接受远程语音
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
