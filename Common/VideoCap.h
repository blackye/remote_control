#if !defined(AFX_VIDEOCAP_H__)
#define AFX_VIDEOCAP_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include <windows.h>
#include <vfw.h>
#pragma comment(lib,"Vfw32.lib")
#pragma once

class CVideoCap
{
public:
	CVideoCap();
	virtual ~CVideoCap();

public:
	LPBITMAPINFO m_lpbmi;
	static BOOL IsWebCam();
	bool Initialize();
	LPVOID GetDIB();

protected:
	static LRESULT CALLBACK capErrorCallback(HWND hWnd,	int nID, LPCTSTR lpsz);
	static LRESULT CALLBACK FrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr);

private:
	LPVOID m_lpDIB;
	HWND m_hWnd;
	HWND m_hWndCap;
	bool m_bIsCapture;
	static bool m_bIsConnected;
};

#endif // !defined(AFX_VIDEOCAP_H__)