#pragma once
#include "afxcmn.h"
#include "ModuleSocket.h"
#include "Common.h"
#include "FileDir.h"
//文件管理视图对话框
// CFileView 对话框

static CString GetRealSize(__int64 nSize);   //文件大小格式化

class CFileView : public CDialog
{
	DECLARE_DYNAMIC(CFileView)

public:
	CFileView(CWnd* pParent = NULL, SOCKET sock = NULL);   // 标准构造函数
	virtual ~CFileView();

// 对话框数据
	enum { IDD = IDD_DLG_FILEVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CRect m_rect;
	SOCKET m_socket;
	CModuleSocket m_moduleSocket;
	int m_nDirCount;  //文件夹数量
	CWnd *m_pWndMsg;   //父窗口指针
    CBitmap m_MenuBmp[8];   //菜单项图标
	bool m_bCopy;    //是否出现复制
public:
	CTreeCtrl m_treeFile;  //树控件
	CListCtrl m_listCtrl;  //列表控件
	CImageList m_ImageTree;   //树控件图像列表
	CImageList m_ImageList;   //list控件图像列表
	HTREEITEM m_hItem;   //树控件当前选择项的句柄
	HTREEITEM m_Root_tree;  //树控件根节点
	virtual BOOL OnInitDialog();
	void InitDlg();  //界面的初始化
	void SetDiskInfo(DRIVER_C tagDisk);  //设置磁盘信息
	void SetFileListInfo( IN FILEINFO_C tagFileInfo );  //设置文件列表信息
	void DelChildItem(IN HTREEITEM hItem);  //删除文件夹下的子项
	CString GetCurFilePath(IN HTREEITEM hCurrentItem);  //获取当前选择项的路径
	HTREEITEM GetCurhItemByPath(IN CString strFolderName);
    void UpDataStatusBar(HTREEITEM hItem);  //更新状态栏的信息 
	void SetFileExecInfo(IN FILEEXECINFO_C tagFileInfo);  //设置文件执行结构信息
	void SetArrtibuteInfo(IN ATTRIBUTE_C tagAttribute);  //显示属性对话框
public:
	afx_msg void OnNMClickTreeFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileDownload();   //文件下载
	afx_msg void OnFileUpload();   //文件上传
	afx_msg void OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileRun();
	afx_msg void OnFileRefresh();
	afx_msg void OnFolderCreate();
	afx_msg void OnFileDel();
	afx_msg void OnFileCopy();
	afx_msg void OnFilePaste();
	afx_msg void OnFileRename();
	afx_msg void OnFileAttribute();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
