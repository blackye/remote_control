// ProgressDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ProgressDlg.h"
#include "SystemManager.h"

// CProgressDlg 对话框
typedef struct
{
	TCHAR *title;
	int nWidth; 
}COLUMN_PRO;   //进程List列表信息

COLUMN_PRO g_Column[] = 
{
	{_T("进程名称"),	    102	},
	{_T("进程ID"),			 82	},
	{_T("线程总数"),	     82	},
	{_T("进程优先级"),		 82	},
	{_T("进程路径"),		210	}
};

static int g_Pro_Len = 0;  //列长度
static int g_Pro_Count = sizeof(g_Column) / sizeof(COLUMN_PRO) ;   //list控件列表数目

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CProgressDlg::IDD, pParent)
{
	m_socket = sock;
	m_pWndMsg = pParent;
	m_bRefresh = false;
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROGRESS, m_proListCtrl);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROGRESS, &CProgressDlg::OnNMRClickListProgress)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROGRESS, &CProgressDlg::OnLvnItemchangedListProgress)
	ON_COMMAND(ID_PROC_TASKKILL, &CProgressDlg::OnProcTaskkill)
	ON_COMMAND(ID_PROC_REFRESH, &CProgressDlg::OnProcRefresh)
END_MESSAGE_MAP()


// CProgressDlg 消息处理程序
BOOL CProgressDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CProgressDlg::DlgInit()
{
	for (int Index = 0 ; Index < g_Pro_Count ; Index++)  //插入列
	{
		m_proListCtrl.InsertColumn(Index, g_Column[Index].title);
		m_proListCtrl.SetColumnWidth(Index, g_Column[Index].nWidth);
		g_Pro_Len += g_Column[Index].nWidth;
	}
	m_proListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_MenuBmp[0].LoadBitmap(IDB_BMP_TASKKILL);
	m_MenuBmp[1].LoadBitmap(IDB_BMP_REFRESH);

	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_PROGRESS;   //进程管理查看
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	if (!bSuccess)
	{
		::MessageBox(this->m_hWnd, _T("查看进程信息失败!"), _T("提示"), MB_OK | MB_ICONWARNING);
	}
}

void CProgressDlg::SetProcessInfo( IN PROGRESS_C tagProInfo )
{
	CString strProName = _T("");          //进程名
	CString strPid     = _T("");          //进程的PID
	CString strThreadCount = _T("");      //进程下的所有线程数
	CString strLevel   = _T("");          //进程的优先级
	CString strProPath = _T("");          //进程的路径

	//显示到界面上
	strProName.Format(_T("%s"), tagProInfo.szProName);
	strPid.Format(_T("%d"), tagProInfo.nPid);
	strThreadCount.Format(_T("%d"), tagProInfo.nThreadCount);
	strLevel = GetProcessLevel(tagProInfo.nLevel);
	strProPath.Format(_T("%s"), tagProInfo.szProPath);

	if (m_bRefresh)
	{
		m_proListCtrl.DeleteAllItems();   //如果是刷新，则删除所有项
		m_bRefresh = false;
	}
	int nIndex = m_proListCtrl.GetItemCount();
	m_proListCtrl.InsertItem(nIndex, strProName);
	m_proListCtrl.SetItemText(nIndex, 1, strPid);
	m_proListCtrl.SetItemText(nIndex, 2, strThreadCount);
	m_proListCtrl.SetItemText(nIndex, 3, strLevel);
	m_proListCtrl.SetItemText(nIndex, 4, strProPath);
	m_proListCtrl.SetItemData(nIndex, (DWORD_PTR)tagProInfo.nPid);

	//更新状态栏
	CString strCount = _T("");
	strCount.Format(_T("数量:%d"), nIndex);
	CString strKillInfo = _T("");
	if (1 == tagProInfo.nTag)
	{
		strKillInfo = _T("进程结束成功!");
	}
	else if (2 == tagProInfo.nTag)
	{
		strKillInfo = _T("进程结束失败!");
	}
	UpDataStatusBar(strKillInfo, strCount);
}

CString CProgressDlg::GetProcessLevel(IN int proLevel)
{
	//进程优先级
	CString strLevel = _T("");
	switch (proLevel)
	{
	case UNKNOWN_LEVEL:
		{
			strLevel = _T("未知");
		}
		break;
	case NORMAL:
		{
			strLevel = _T("普通");
		}
		break;
	case IDLE:
		{
			strLevel = _T("低");
		}
		break;
	case REALTIME:
		{
			strLevel = _T("实时");
		}
		break;
	case HIGH:
		{
			strLevel = _T("高");
		}
		break;
	case ABOVENORMAL:
		{
			strLevel = _T("高于标准");
		}
		break;
	case BELOWNORMAL:
		{
			strLevel = _T("低于标准");
		}
		break;
	}
	return strLevel;
}

//更新状态栏
void CProgressDlg::UpDataStatusBar(IN CString strLeft, IN CString strRight)
{
	::SendMessage( ((CSystemManager*)this->m_pWndMsg)->m_sysStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)strLeft.GetBuffer(0));
	::SendMessage( ((CSystemManager*)this->m_pWndMsg)->m_sysStatusBar, SB_SETTEXT, (WPARAM)1, (LPARAM)strRight.GetBuffer(1));	
}

void CProgressDlg::OnNMRClickListProgress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int nItem = pNMItemActivate->iItem;  //选中项的索引值
	if (-1 == nItem)
	{
		return ;
	}
	CMenu processMenu;   //操作目录
	processMenu.LoadMenu(IDR_MENU_PROCESS);
	CMenu *pSubMenu = processMenu.GetSubMenu(0);
	pSubMenu->SetMenuItemBitmaps(ID_PROC_TASKKILL, MF_BYCOMMAND, &m_MenuBmp[0], &m_MenuBmp[0]);  //图标
	pSubMenu->SetMenuItemBitmaps(ID_PROC_REFRESH, MF_BYCOMMAND, &m_MenuBmp[1], &m_MenuBmp[1]); 

	CPoint point;
	GetCursorPos(&point);  //获取光标位置
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	*pResult = 0;
}

void CProgressDlg::OnLvnItemchangedListProgress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString strShowMsg = _T("进程名称: ");
	int nItem = pNMLV->iItem;
	if (-1 == nItem)
	{
		return;
	}
	CString strProName = m_proListCtrl.GetItemText(nItem, 0);  //进程名
	CString strProPid  = m_proListCtrl.GetItemText(nItem, 1);  //进程的PID
	strShowMsg = strShowMsg + strProName + _T("  ID:") + strProPid;
	::SendMessage( ((CSystemManager*)this->m_pWndMsg)->m_sysStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)strShowMsg.GetBuffer(0));
	*pResult = 0;
}

//关闭指定的进程
void CProgressDlg::OnProcTaskkill()
{
	// TODO: 在此添加命令处理程序代码
	POSITION pos;
	pos = m_proListCtrl.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		return ;
	}
	int nItem = m_proListCtrl.GetNextSelectedItem(pos);
	PROGRESS_C tagProInfo;
	memset(&tagProInfo, 0, sizeof(PROGRESS_C));
	tagProInfo.nPid = m_proListCtrl.GetItemData(nItem);  //获取PID值
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_PROC_TASKKILL;  //结束进程
	memcpy((char*)tagMsgInfo.context, (char*)&tagProInfo, sizeof(PROGRESS_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	m_bRefresh = true;
}

//刷新进程列表
void CProgressDlg::OnProcRefresh()
{
	// TODO: 在此添加命令处理程序代码
	m_bRefresh = true;
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_PROGRESS;  //发送刷新命令
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

void CProgressDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	DestroyWindow();
	delete this;
}

void CProgressDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	((CSystemManager*)this->m_pParentWnd)->m_progressDlg = NULL; 
	CDialog::PostNcDestroy();
}
