// MsgNoticeWnd.cpp : 实现文件

#include "stdafx.h"
#include "MsgNoticeWnd.h"
#include "resource.h"
#include "Define_Inc.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgNoticeWnd

CMsgNoticeWnd::CMsgNoticeWnd(CString strCaption, CString strOnlineMsg)
{
	m_Bitmap.LoadBitmap(MAKEINTRESOURCE(IDB_BMP_NOTICE)); //Load Bitmap
	m_Bitmap.GetBitmap(&bmBitmap);         //Get Bitmap Info
	m_bFlag = true;
 	m_strMessage = new TCHAR[strOnlineMsg.GetLength() +1];
 	lstrcpy(m_strMessage, strOnlineMsg); 
	m_strCaption = new TCHAR[strCaption.GetLength() +1];
	lstrcpy(m_strCaption, strCaption);
}

CMsgNoticeWnd::~CMsgNoticeWnd()
{
	if (NULL != m_strMessage)
	{
		delete[] m_strMessage;
	}
	if (NULL != m_strCaption)
	{
		delete[] m_strCaption; 
	}
}


BEGIN_MESSAGE_MAP(CMsgNoticeWnd, CWnd)
	//{{AFX_MSG_MAP(CMsgNoticeWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	//}}AFX_MSG_MAP
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMsgNoticeWnd message handlers
void CMsgNoticeWnd::CreateMsgWindow()
{
    m_hCursor = LoadCursor(NULL,IDC_HAND);
	CreateEx(0 ,AfxRegisterWndClass(0,
		                           ::LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND)),(HBRUSH)(COLOR_DESKTOP+1),NULL),								  
								    _T(""),
									WS_POPUP|WS_EX_TOPMOST,
									0,
									0,
									0,//bmBitmap.bmWidth,  //Bitmap Width = Splash Window Width
									0,//bmBitmap.bmHeight, //Bitmap Height = Splash Window Height
									NULL,//AfxGetMainWnd()->GetSafeHwnd(),
									NULL,
									NULL);
	SetCursor(m_hCursor);
	SetTimer(ID_TIMER_POP_WINDOW,20,NULL);
}

void CMsgNoticeWnd::SetPromptMessage(LPCTSTR lpszMsg)
{
	lstrcpy(m_strMessage,lpszMsg);
}

void CMsgNoticeWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CDC dcMemory;
	CRect rect;
	GetClientRect(&rect);
	dcMemory.CreateCompatibleDC(NULL);
	dcMemory.SelectObject(&m_Bitmap);
	dc.StretchBlt(0,
		          0,
		          rect.right-rect.left,//bmBitmap.bmWidth,
		          rect.bottom-rect.top,//bmBitmap.bmHeight,    
		          &dcMemory, 
		          0,
		          0,
				  bmBitmap.bmWidth,    
				  bmBitmap.bmHeight,
				  SRCCOPY);	
	CFont font;
	font.CreatePointFont(90, _T("Impact"));
	dc.SelectObject(&font);
	dc.SetTextColor(RGB(0, 64, 128));
	dc.SetBkMode(TRANSPARENT);
	dc.TextOut(30, 5, m_strCaption);
	rect.top = 30;
	dc.DrawText(m_strMessage,-1, &rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	// Do not call CWnd::OnPaint() for painting messages
}

void CMsgNoticeWnd::OnTimer(UINT nIDEvent) 
{	
	static int nHeight=0;
	int cy=GetSystemMetrics(SM_CYSCREEN);
	int cx=GetSystemMetrics(SM_CXSCREEN);
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);
	int y=rect.bottom-rect.top;
	int x=rect.right-rect.left;
	x=x-WIN_WIDTH;

	switch(nIDEvent)
	{
	case ID_TIMER_POP_WINDOW:
		if(nHeight<=WIN_HEIGHT)
		{
			//++nHeight;
			nHeight += 3;
			MoveWindow(x,
				y-nHeight,
				WIN_WIDTH,
				WIN_HEIGHT);
			Invalidate(FALSE);
		}
		else
		{
			KillTimer(ID_TIMER_POP_WINDOW);
			SetTimer(ID_TIMER_DISPLAY_DELAY,4000,NULL);
		}
		break;
	case ID_TIMER_CLOSE_WINDOW:
		if(nHeight >0 )
		{
			nHeight -= 3;
			MoveWindow(x,
				y-nHeight,
				WIN_WIDTH,
				nHeight);
		}
		else
		{
			KillTimer(ID_TIMER_CLOSE_WINDOW);
			SendMessage(WM_CLOSE);
		}
		break;
	case ID_TIMER_DISPLAY_DELAY:
		KillTimer(ID_TIMER_DISPLAY_DELAY);
		SetTimer(ID_TIMER_CLOSE_WINDOW,20,NULL);
		break;
	}

	CWnd::OnTimer(nIDEvent);
}

int CMsgNoticeWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CMsgNoticeWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	TRACKMOUSEEVENT    tme;   
	tme.cbSize=sizeof(TRACKMOUSEEVENT);   
	tme.dwFlags=TME_HOVER    |    TME_LEAVE;   
	tme.dwHoverTime=HOVER_DEFAULT;   
	tme.hwndTrack=m_hWnd;   
	if(!_TrackMouseEvent(&tme))   
	{
		m_bFlag = false;   //鼠标事件捕捉失败
	}
	KillTimer(ID_TIMER_DISPLAY_DELAY);
	CWnd::OnMouseMove(nFlags, point);
}

void CMsgNoticeWnd::OnMouseLeave()
{
	if (m_bFlag)
	{
		KillTimer(ID_TIMER_POP_WINDOW);
		SetTimer(ID_TIMER_DISPLAY_DELAY,20,NULL);
	}
	CWnd::OnMouseLeave();
}

void CMsgNoticeWnd::SetPromptCaption(LPCTSTR lpszCaption)
{
	lstrcpy(m_strCaption,lpszCaption);
}

BOOL CMsgNoticeWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_hCursor)
	{
	   SetCursor(m_hCursor);
	   return TRUE;
	}
	HCURSOR hCursor = LoadCursor(NULL ,IDC_HAND);
	::SetCursor(hCursor);
	return FALSE;
}
