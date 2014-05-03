#pragma once

#include "Common.h"
#include "ModuleSocket.h"

//文件操作类
class CFileInfo
{
public:
	CFileInfo(void);
	~CFileInfo(void);
	void SendDownFileInfo(IN wchar_t* pPath, IN SOCKET sock);  //发送指定下载文件信息
	void CreateUpLoadFile( IN wchar_t* pPath);  //创建文件
	void WriteDataInFile(IN DOWNFILEDATA_S tagDownFileDate);  //将数据块写入文件中
	void ExecuteFile(IN wchar_t* pFilePath, IN SOCKET sock);   //执行文件
	void SendAttriInfo(IN ATTRIBUTE_S tagAttribute, IN SOCKET sock);   //发送文件(夹)属性
private:
	CModuleSocket m_moduleSocket;
	HANDLE m_hFile;
};
