#if !defined(AFX_MSGWND_H__1DC7047D_8675_4C80_B335_54F78F3BBD76__INCLUDED_)
#define AFX_MSGWND_H__1DC7047D_8675_4C80_B335_54F78F3BBD76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgNoticeWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgNoticeWnd window

class CMsgNoticeWnd : public CWnd
{
	// Construction
public:
	CMsgNoticeWnd(CString strCaption, CString strOnlineMsg);

	// Attributes
protected:
	CBitmap m_Bitmap;
	BITMAP bmBitmap;
	BOOL m_bFlag;


	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgNoticeWnd)
	//}}AFX_VIRTUAL

	// Implementation
public:	
	LPTSTR m_strMessage;
	LPTSTR m_strCaption;
	HCURSOR m_hCursor;   //光标形状
	virtual ~CMsgNoticeWnd();
	void CreateMsgWindow();
	void SetPromptMessage(LPCTSTR lpszMsg);
	void SetPromptCaption(LPCTSTR lpszCaption);
	// Generated message map functions
protected:
	//{{AFX_MSG(CMsgNoticeWnd)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);  //设置光标
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGWND_H__1DC7047D_8675_4C80_B335_54F78F3BBD76__INCLUDED_)
