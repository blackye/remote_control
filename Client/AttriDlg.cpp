// AttriDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "AttriDlg.h"


// CAttriDlg 对话框

IMPLEMENT_DYNAMIC(CAttriDlg, CDialog)

CAttriDlg::CAttriDlg(CWnd* pParent /*=NULL*/ , ATTRIBUTE_C* tagArrtibute)
	: CDialog(CAttriDlg::IDD, pParent)
{
	m_tagArrtibute = *tagArrtibute;
}

CAttriDlg::~CAttriDlg()
{
}

void CAttriDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_ATTR, m_picCtrl);
	DDX_Control(pDX, IDC_CHECK_READONLY, m_ReadOnlyCheck);
	DDX_Control(pDX, IDC_CHECK_HIDE, m_HideCheck);
	DDX_Control(pDX, IDC_CHECK_ARCHIVE, m_ArchiveCheck);
	DDX_Control(pDX, IDC_CHECK_SYSTEM, m_SystemCheck);
}


BEGIN_MESSAGE_MAP(CAttriDlg, CDialog)
END_MESSAGE_MAP()


// CAttriDlg 消息处理程序
BOOL CAttriDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CAttriDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//对话框初始化
void CAttriDlg::DlgInit()
{
	CString strFilePath = _T("");   //所在路径
	strFilePath.Format(_T("%s"), m_tagArrtibute.szFilePath);
	GetDlgItem(IDC_STATIC_LOACTION)->SetWindowText(strFilePath);   //位置
	CString strFileName = _T("");  //文件名
	strFileName.Format(_T("%s"), m_tagArrtibute.szFileName);
	GetDlgItem(IDC_ATTR_FILE_NAME)->SetWindowText(strFileName);   //名称
	CString strTitle = _T(" 属性");
	strTitle = strFileName + strTitle;
	this->SetWindowText(strTitle);  //设置对话框的标题

	switch (m_tagArrtibute.nType)
	{
	case FILE_ATTR:    //文件
		{
		
			GetDlgItem(IDC_STATIC_TYPE)->SetWindowText(_T("文件"));
			SHFILEINFO shInfo; //mfc文件信息结构体
			memset((char*)&shInfo, 0, sizeof(shInfo));

			//利用文件扩展名来决定图标显示，比如传过来的是a.exe，则找到客户端exe文件图标显示的句柄.
			if (SHGetFileInfo(strFileName, 0, &shInfo, sizeof(shInfo), SHGFI_ICON |SHGFI_USEFILEATTRIBUTES))
			{
				m_picCtrl.SetIcon(shInfo.hIcon);
				SetIcon(shInfo.hIcon, FALSE);   //设置对话框的图标
			}
			else   //文件图标句柄获取失败! 则采用资源引入的文件图标
			{
				m_picCtrl.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_FILE));
			}
		}
		break;
	case FOLDER_ATTR:  //文件夹
		{
			GetDlgItem(IDC_STATIC_TYPE)->SetWindowText(_T("文件夹"));    //类型
			m_picCtrl.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_FOLDER));
		}
		break;
	default:
		break;
	}

	//设置时间
	wchar_t szDateTime[MAX_PATH] = {0};
    wsprintfW(szDateTime,                                          
		      _T("%02d年%02d月%02d日, %02d:%02d:%02d"),
		       m_tagArrtibute.sysCreateTime.wYear,
			    m_tagArrtibute.sysCreateTime.wMonth,         
			    m_tagArrtibute.sysCreateTime.wDay, 
			    m_tagArrtibute.sysCreateTime.wHour, 
			    m_tagArrtibute.sysCreateTime.wMinute, 
			    m_tagArrtibute.sysCreateTime.wSecond);
	GetDlgItem(IDC_STATIC_CREATE_TIME)->SetWindowText(szDateTime);   //创建时间
	memset(szDateTime, 0, sizeof(szDateTime));
	wsprintfW(szDateTime,
		_T("%02d年%02d月%02d日, %02d:%02d:%02d"),
		m_tagArrtibute.sysVisitTime.wYear,
		m_tagArrtibute.sysVisitTime.wMonth,         
		m_tagArrtibute.sysVisitTime.wDay, 
		m_tagArrtibute.sysVisitTime.wHour, 
		m_tagArrtibute.sysVisitTime.wMinute, 
		m_tagArrtibute.sysVisitTime.wSecond);
	GetDlgItem(IDC_STATIC_VISIT_TIME)->SetWindowText(szDateTime);   //访问时间
	memset(szDateTime, 0, sizeof(szDateTime));
	wsprintfW(szDateTime,
		_T("%02d年%02d月%02d日, %02d:%02d:%02d"),
		m_tagArrtibute.sysModifyTime.wYear,
		m_tagArrtibute.sysModifyTime.wMonth,         
		m_tagArrtibute.sysModifyTime.wDay, 
		m_tagArrtibute.sysModifyTime.wHour, 
		m_tagArrtibute.sysModifyTime.wMinute, 
		m_tagArrtibute.sysModifyTime.wSecond);
	GetDlgItem(IDC_STATIC_MODI_TIME)->SetWindowText(szDateTime);   //最后修改时间

	//设置文件(夹)属性
	m_ReadOnlyCheck.SetCheck(m_tagArrtibute.bReadOnly);
	m_HideCheck.SetCheck(m_tagArrtibute.bHide);
	m_ArchiveCheck.SetCheck(m_tagArrtibute.bArchive);
	m_SystemCheck.SetCheck(m_tagArrtibute.bSystem);
}
