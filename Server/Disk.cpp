#include "StdAfx.h"
#include "Disk.h"

CDisk::CDisk(void)
{
}

CDisk::~CDisk(void)
{
}

void CDisk::SendDiskInfo(IN SOCKET sock)
{
	if (INVALID_SOCKET == sock)
	{
		return;
	}

	MSGINFO_S tagMsg;
	DRIVER_S tagDir;  //磁盘结构体
	for (wchar_t i = 'B'; i <= 'Z'; i++)  //为什么不从A开始，应该A有可能是软盘，而这不是我们所关心的对象
	{
		memset(&tagDir, 0, sizeof(DRIVER_S));  //先清空
		memset(&tagMsg, 0, sizeof(MSGINFO_S));
		unsigned __int64 i32FreeBytesToCaller = 0;
		unsigned __int64 i32TotalBytes = 0;
		unsigned __int64 i32FreeBytes = 0;
		TCHAR szTemp[] = {i, ':', '\0'};
		UINT uType = GetDriveType(szTemp);  //获取磁盘的类型
		switch (uType)
		{
		case DRIVE_FIXED:  //固定硬盘
			{
				tagDir.nType = DISK_FIXED;
			}
			break;
		case DRIVE_REMOVABLE:  //移动硬盘
			{
				tagDir.nType = DISK_REMOVABLE;
			}
			break;
		case DRIVE_CDROM:  //CD-ROM
			{
				tagDir.nType = DISK_CDROM;
			}
			break;
		default:          //其他
			continue;
		}
        tagDir.disk = i;   //磁盘名
		BOOL bSuccess = GetDiskFreeSpaceEx(szTemp,
			                               (PULARGE_INTEGER)&i32FreeBytesToCaller,
										   (PULARGE_INTEGER)&i32TotalBytes,
										   (PULARGE_INTEGER)&i32FreeBytes);
		if (bSuccess)  //获取到磁盘空间信息
		{
			tagDir.dwTotal = (double)(i32TotalBytes/1024.0/1024/1024);
			tagDir.dwFree  = (double)(i32FreeBytesToCaller/1024.0/1024/1024);
		}
		else
		{
			tagDir.dwTotal = 0.0;
			tagDir.dwFree = 0.0;
		}

		tagMsg.Msg_id = CMD_DISKINFO;
		memcpy((char*)tagMsg.context, (char*)&tagDir, sizeof(DRIVER_S));
		bool bSend = true;
		UINT32 result = m_moduleSocket.SendCommand(sock, (char*)&tagMsg, sizeof(MSGINFO_S), &bSend);
		if (!bSend)
		{
			printf("磁盘信息发送失败! Error Code: %d\n", result);
		}
		else
		{
			printf("成功发送%c磁盘信息\n", i);
		}
	}
}

void CDisk::SendFileList(IN wchar_t* pPath, IN SOCKET sock)
{
	if (INVALID_SOCKET == sock)
	{
		return;
	}

	WIN32_FIND_DATAW FindData;
	memset(&FindData, 0, sizeof(WIN32_FIND_DATAW));
	HANDLE hRet = INVALID_HANDLE_VALUE;
	int nFileCount = 0;   //文件数量
	TCHAR szFilePathName[MAX_PATH] = {0};
	wchar_t filePath[MAX_PATH] = {0};
	wcscpy_s(filePath, pPath);
	wsprintfW(szFilePathName, _T("%s\\*.*"), filePath);
	hRet = FindFirstFileW(szFilePathName, &FindData);
	if (INVALID_HANDLE_VALUE == hRet)
	{
		printf("文件路径不存在!\n");
		return ;
	}
	FILEINFO_S tagFileInfo;  //文件信息结构体
	MSGINFO_S tagMsg;
	DWORDLONG dl = 0;
	bool bSuccess = true;
	memset(&tagFileInfo, 0, sizeof(FILEINFO_S));
	memset(&tagMsg, 0, sizeof(MSGINFO_S));

	while (::FindNextFile(hRet, &FindData))  //遍历
	{
		if (!wcscmp(_T("."), FindData.cFileName) || !wcscmp(_T(".."), FindData.cFileName))
		{
			continue;
		}
		//构造完整的路径, 文件数目+1
		tagMsg.Msg_id = CMD_FILELIST;

		wsprintfW(tagFileInfo.szFileName, _T("%s"), FindData.cFileName);  //文件路径
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  //文件夹
		{
			tagFileInfo.nType = FOLDER_ATTR;
		}
		else
		{
			tagFileInfo.nType = FILE_ATTR;
			tagFileInfo.size = ((__int64)FindData.nFileSizeHigh * (dl + 1)) + (__int64)FindData.nFileSizeLow;
		}
		nFileCount++;  //文件数量增加
		memcpy((char*)tagMsg.context, (char*)&tagFileInfo, sizeof(FILEINFO_S));
		m_moduleSocket.SendCommand(sock, (char*)&tagMsg, sizeof(MSGINFO_S), &bSuccess);
		memset(&FindData, 0, sizeof(WIN32_FIND_DATAW)); //清空
		memset(&tagFileInfo, 0, sizeof(FILEINFO_S));
		memset(&tagMsg, 0, sizeof(MSGINFO_S));
	}
	tagMsg.Msg_id = CMD_FILELIST;
	tagFileInfo.nType = FILE_TAG;  //文件信息发送完毕
	memcpy((char*)tagMsg.context, (char*)&tagFileInfo, sizeof(FILEINFO_S));
	m_moduleSocket.SendCommand(sock, (char*)&tagMsg, sizeof(MSGINFO_S), &bSuccess);
	if (bSuccess)
	{
		printf("文件列表信息成功发送完毕!\n");
	}
	else
	{
		printf("文件列表信息发送失败!\n");
	}
}

void CDisk::ExecCreateFolder(IN wchar_t* pFilePath, IN SOCKET sock)
{
	bool bExist = false;
	FolderExist(pFilePath, &bExist);
	if (!bExist)   //不存在
	{
		CreateFolder(pFilePath, &bExist);
		if (bExist)
		{
			printf("创建文件目录成功!\n");
		}
		else
		{
			printf("创建文件目录失败\n");
		}
	}
	else
	{
		printf("目录已经存在");
		return ;
	}
	RefreshList(pFilePath, sock);  //重新刷新文件列表
}

void CDisk::FolderExist(IN const CString strPath, OUT bool* pbExist)
{
	WIN32_FIND_DATA  wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;   
	}
	FindClose(hFind);
    *pbExist = rValue;
}

void CDisk::CreateFolder(IN const CString strPath, OUT bool* pbSuccess)
{
	bool bSuccess = true;
	SECURITY_ATTRIBUTES attrib;
	attrib.bInheritHandle = FALSE;
	attrib.lpSecurityDescriptor = NULL;
	attrib.nLength =sizeof(SECURITY_ATTRIBUTES);
	bSuccess = (TRUE == CreateDirectory(strPath, &attrib) ? true: false);
	*pbSuccess = bSuccess;
}

void CDisk::DeleteTargetFile(IN FILEINFO_S tagFileInfo, IN SOCKET sock)
{
	CString strPath = _T("");
	strPath.Format(_T("%s"), tagFileInfo.szFileName);

	if (FILE_ATTR == tagFileInfo.nType)  //删除的是文件
	{
		DeleteSpecifiedFile(strPath);
	}
	else if (FOLDER_ATTR == tagFileInfo.nType)  //删除的是文件夹
	{
		bool bSuccess = true;
		DeleteDirectory(strPath, &bSuccess);
	}

	RefreshList(tagFileInfo.szFileName, sock);  //重新刷新文件列表
}

//删除文件夹
UINT32 CDisk::DeleteDirectory( IN const CString strInstallPath, 
								   OUT bool* pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	if (strInstallPath.IsEmpty() || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		*pbSuccess = false;
		return result;
	}
	CFileFind findFile;
	bool bSuccess = true;
	BOOL bFound = TRUE;
	CString strAllFilePath = _T("");
	CString strFileName = _T(""); //文件名
	if (_T("\\") != strInstallPath.Right(1))
	{
		strAllFilePath.Format(_T("%s\\*.*"), strInstallPath);
	}
	else
	{
		strAllFilePath.Format(_T("%s"), strInstallPath);
	}
	try
	{
		bFound = findFile.FindFile(strAllFilePath);
		if (0 == bFound)//没有找到
		{
			result = GetLastError();
			bSuccess = false;
			throw result;
		}
		while (bFound)
		{
			bFound = findFile.FindNextFile();
			if (findFile.IsDirectory())//如果是文件夹
			{
				CString strTempDir = _T("");
				if (findFile.IsDots())
				{
					continue;
				}
				strFileName.Format(_T("%s"), findFile.GetFileName());
				strTempDir = strInstallPath + _T("\\") + strFileName;
				if (findFile.IsReadOnly()) //如果文件具有只读属性的话，去掉只读属性
				{
					DWORD dwFileAttr = GetFileAttributes(strTempDir);
					if (INVALID_FILE_ATTRIBUTES != dwFileAttr)
					{
						dwFileAttr &= ~FILE_ATTRIBUTE_READONLY;  //去掉只读属性
						SetFileAttributes(strTempDir, dwFileAttr);
					}
				}
				result = DeleteDirectory(strTempDir, &bSuccess); //递归删除
			}
			else //如果是文件
			{
				strFileName.Format(_T("%s"), findFile.GetFilePath());
				if (findFile.IsReadOnly())
				{
					SetFileAttributes(strFileName, GetFileAttributes(strFileName) &(~FILE_ATTRIBUTE_READONLY) );
				}
				result = DeleteSpecifiedFile(strFileName);   //删除文件
			}
		}
		findFile.Close();
	}
	catch (UINT32)
	{
	}
	DeleteEmptyFolder(strInstallPath);  //删除空文件夹
	*pbSuccess = bSuccess;
	return result;
}

//删除指定文件
UINT32 CDisk::DeleteSpecifiedFile( IN const CString strFileFullPath)
{
	UINT32 result = ERROR_SUCCESS;
	if (strFileFullPath.IsEmpty())
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}

	try
	{
		if (0 == DeleteFile(strFileFullPath))
		{
			printf("文件删除失败 !\n");
			result = GetLastError();
			throw result;
		}
		else
		{
			printf("文件删除成功 !\n");
		}
	}
	catch (UINT32)
	{
	}
	return result;
}

//删除空文件夹
void CDisk::DeleteEmptyFolder( IN const CString strFolderPath)
{
	BOOL bDelete = RemoveDirectory(strFolderPath);
	if (0 == bDelete)
	{
		printf("文件夹删除失败 !\n");
	}
	else
	{
		printf("文件夹删除成功 !\n");
	}
}

void CDisk::AddCopyFileIntoList(IN FILECOPY_S tagFileInfo)
{
	if (tagFileInfo.bTag)  //如果接受到开始发送信息，清空LIST容器
	{
		m_listFileInfo.clear();
		return;
	}
	m_listFileInfo.push_back(tagFileInfo);  //将文件信息的结构体放入到LIST容器中
}

void CDisk::ExecuteCopyFile(IN wchar_t* pPath, IN SOCKET sock)
{
	std::list<FILECOPY_S>::const_iterator iter;
	BOOL bRet = true;
	for (iter = m_listFileInfo.begin(); iter != m_listFileInfo.end(); iter++)
	{
		FILECOPY_S tagFileInfo = *iter;
		CString strExistFileName = tagFileInfo.szFilePath;  //已存在文件的路径
		CString strNewFileName = _T("");

		CString strPath = _T("");
		strPath.Format(_T("%s"), pPath);
		CString strName = _T("");
		strName.Format(_T("%s"), tagFileInfo.szFileName);
		strNewFileName = strPath + strName;

		bRet = ::CopyFile(strExistFileName, strNewFileName, FALSE);
		if (bRet)
		{
			printf("%s文件拷贝成功! \n", strNewFileName);
		}
		else
		{
			printf("%s文件拷贝失败! \n", strNewFileName);
		}
	}
	RefreshList(pPath, sock);
}

void CDisk::FileReName(IN FILECOPY_S tagFileInfo, IN SOCKET sock)
{
	if (tagFileInfo.bTag)  //重命名文件
	{
		ReNameFile(tagFileInfo);
	}
	else  //重命名文件夹
	{
		ReNameFolder(tagFileInfo);
	}
	RefreshList(tagFileInfo.szFilePath, sock);
}

//重命名文件
void CDisk::ReNameFile(IN FILECOPY_S tagFileInfo)
{
	CString strFolderPath = _T("");
	strFolderPath.Format(_T("%s"), tagFileInfo.szFilePath);
	int nIndex = strFolderPath.ReverseFind(_T('\\'));
	strFolderPath = strFolderPath.Left(nIndex + 1);  //tagFileInfo.szFilePath == c:\windows\1.txt ---> c:\windows\

	CString strFileName = _T("");
	strFileName.Format(_T("%s"), tagFileInfo.szFileName);
    CString strNewFile = strFolderPath + strFileName;  // c:\windows\ ---> c:\windows\2.bat

	try
	{
		CFile::Rename(tagFileInfo.szFilePath, strNewFile);
		printf("文件重命名成功!\n");
	}
	catch(CFileException* pEx )
	{
		TRACE(_T("File %20s not found, cause = %d\n"), tagFileInfo.szFilePath, pEx->m_cause);
		pEx->Delete();
	}
}

//重命名文件夹
void CDisk::ReNameFolder(IN FILECOPY_S tagFileInfo)
{
	CString strFolderPath = _T("");
	strFolderPath.Format(_T("%s"), tagFileInfo.szFilePath);
	int nIndex = strFolderPath.ReverseFind(_T('\\'));
	strFolderPath = strFolderPath.Left(nIndex + 1);  //tagFileInfo.szFilePath == c:\windows\1 ---> c:\windows\
    
	CString strFolderName = _T("");
	strFolderName.Format(_T("%s"), tagFileInfo.szFileName);
	CString strNewFolder = strFolderPath + strFolderName;  // c:\windows\ ---> c:\windows\2
	if (::MoveFile(tagFileInfo.szFilePath, strNewFolder))
	{
		printf("文件夹重命名成功!\n");
	}
	else
	{
		printf("文件夹重命名失败!\n");
	}
}

//重新刷新客户端列表
void CDisk::RefreshList(IN wchar_t* pPath, IN SOCKET sock)
{
	CString strFolderPath = _T("");
	strFolderPath.Format(_T("%s"), pPath);
	int nIndex = strFolderPath.ReverseFind(_T('\\'));
	strFolderPath = strFolderPath.Left(nIndex + 1);
	SendFileList(strFolderPath.GetBuffer(strFolderPath.GetLength()), sock);
}