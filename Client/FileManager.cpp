// FileManager.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "FileManager.h"
#include "BroilerData.h"

// CFileManager 对话框

static UINT idArray[] = {ID_RES_FILE1,     //状态栏
                         ID_RES_FILE2};


IMPLEMENT_DYNAMIC(CFileManager, CDialog)

CFileManager::CFileManager(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CFileManager::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_FILE_MANAGER);
	m_socket = sock;
}

CFileManager::~CFileManager()
{
}

void CFileManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_FILE, m_tabCtrl);
}


BEGIN_MESSAGE_MAP(CFileManager, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_FILE, &CFileManager::OnTcnSelchangeTabFile)
END_MESSAGE_MAP()


// CFileManager 消息处理程序

BOOL CFileManager::PreTranslateMessage(MSG* pMsg)
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

BOOL CFileManager::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
	// TODO:  在此添加额外的初始化
    InitDlg();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFileManager::InitDlg()
{
	GetClientRect(&m_rect);
	m_ImageTab.Create(16, 16, ILC_COLOR24|ILC_MASK, 1, 1);
	HICON hIcon[2];
	hIcon[0] = (HICON)(AfxGetApp()->LoadIcon(IDI_ICON_FILEVIEW));
	hIcon[1] = (HICON)(AfxGetApp()->LoadIcon(IDI_ICON_FILETRANS));
    for(int i = 0; i < 2; i++)
	{
		m_ImageTab.Add(hIcon[i]);
	}
	m_tabCtrl.SetImageList(&m_ImageTab);
	m_tabCtrl.InsertItem(0, _T("文件查看"), 0);
	m_tabCtrl.InsertItem(1, _T("文件传输"), 1);

	m_pFileView = new CFileView(this, m_socket);
	m_pFileView->Create(IDD_DLG_FILEVIEW, &m_tabCtrl);
	CRect rect;
	m_tabCtrl.GetClientRect(rect);  //获取Tab的区域大小
	rect.DeflateRect(0, 20, 0, 0);  //缩小
	m_pFileView->MoveWindow(rect);

	m_pFileTrans = new CFileTrans(this, m_socket);
	m_pFileTrans->Create(IDD_DLG_FILETRANS, &m_tabCtrl);
	m_tabCtrl.GetClientRect(rect);  //获取Tab的区域大小
	rect.DeflateRect(0, 20, 0, 0);  //缩小
	m_pFileTrans->MoveWindow(rect);

	m_pFileTrans->ShowWindow(SW_SHOW);

	m_fileStatusBar.Create(this);
	m_fileStatusBar.SetIndicators(idArray, sizeof(idArray) / sizeof(UINT));
	m_fileStatusBar.SetPaneInfo(0, idArray[0], SBPS_NORMAL, 200);
	m_fileStatusBar.SetPaneInfo(1, idArray[1], SBPS_NORMAL, 200);
	m_fileStatusBar.SetPaneText(0 , _T("就绪"));
	m_fileStatusBar.SetPaneText(1 , _T(""));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);  //显示工具栏和状态栏（这句话一定要加上, 否则工具栏不显示）

	m_pFileView->ShowWindow(SW_SHOW);
	m_pFileTrans->ShowWindow(SW_HIDE);
}

void CFileManager::OnTcnSelchangeTabFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
    int nIndex = m_tabCtrl.GetCurFocus();   //得到当前索引
	switch (nIndex)
	{
	case 0:
		{
			m_pFileView->ShowWindow(SW_SHOW);
			m_pFileTrans->ShowWindow(SW_HIDE);
		}
		break;
	case 1:
		{
			m_pFileView->ShowWindow(SW_HIDE);
			m_pFileTrans->ShowWindow(SW_SHOW);
		}
		break;
	default:
		break;
	}
	*pResult = 0;
}

void CFileManager::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	DestroyWindow();
	delete this;
}

void CFileManager::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pFileDlg = NULL;   //为什么要先置为NULL ，才能Delete
//	delete ((CBroilerData*)this->m_pParentWnd)->m_pFileDlg;   //原因： 在调用了DestoryWindow()后，内部已经将内存空间删除掉了
}
