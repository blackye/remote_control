#pragma once

#include "ModuleSocket.h"
#include "Common.h"
#include <list>

class CDisk
{
public:
	CDisk(void);
	~CDisk(void);

     void SendDiskInfo(IN SOCKET sock); //发送磁盘信息
	 void SendFileList(IN wchar_t* pPath, IN SOCKET sock);  //发送磁盘列表信息
	 void CreateFolder(IN const CString strPath, OUT bool* pbSuccess);  //创建目录
	 void FolderExist(IN const CString strPath, OUT bool* pbExist);  //检测目录是否存在
	 void ExecCreateFolder(IN wchar_t* pFilePath, IN SOCKET sock);  //执行创建文件操作
	 void DeleteTargetFile(IN FILEINFO_S tagFileInfo, IN SOCKET sock);  //删除文件或文件夹
	 void AddCopyFileIntoList(IN FILECOPY_S tagFileInfo);  //将待复制的文件路径放入到list容器中
	 void ExecuteCopyFile(IN wchar_t* pPath, IN SOCKET sock);   //执行文件拷贝操作
	 void FileReName(IN FILECOPY_S tagFileInfo, IN SOCKET sock);  //重命名
	 void RefreshList(IN wchar_t* pPath, IN SOCKET sock); //重新刷新客户端列表
	 void ReNameFile(IN FILECOPY_S tagFileInfo);   //重命名文件名
	 void ReNameFolder(IN FILECOPY_S tagFileInfo);  //重命名文件夹名
	
	 UINT32 DeleteDirectory( IN const CString strInstallPath, OUT bool* pbSuccess ); //删除目录文件夹
	 UINT32 DeleteSpecifiedFile( IN const CString strFileFullPath);  //删除指定文件
	 void DeleteEmptyFolder( IN const CString strFolderPath);  //删除空文件夹
private:
	CModuleSocket m_moduleSocket;
	std::list<FILECOPY_S> m_listFileInfo;   //待复制的文件路径
};
