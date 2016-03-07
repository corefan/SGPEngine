#include <shlobj.h>
#include <shlwapi.h>
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"shlwapi.lib")

#include "FolderDialog.h"

FolderDialog::FolderDialog(HWND hparent,char* strTitle,char* strPath)\
	:m_hWndParent(hparent)
{
	strcpy(m_Title,strTitle);
	strcpy(m_DefaultPath,strPath);
	strcpy(m_FolderPath,strPath);
}

void FolderDialog::DoModal()
{
	TCHAR pszPath[MAX_PATH];
	BROWSEINFO bi;
	bi.hwndOwner=m_hWndParent;
	bi.pidlRoot=NULL;
	bi.pszDisplayName=NULL;
	bi.lpszTitle=m_Title;
	bi.ulFlags=BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT|BIF_USENEWUI;
	bi.lpfn=BrowseCallBackFun;
	bi.lParam=(LPARAM)m_DefaultPath;
	bi.iImage=NULL;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if(pidl==NULL) return ;
	SHGetPathFromIDList(pidl,pszPath);
	strcpy(m_FolderPath,pszPath);
}

char* FolderDialog::GetPath()
{
	return m_FolderPath;
}

BOOL FolderDialog::IsDirectory()
{
	return PathIsDirectory(m_FolderPath);
}

int CALLBACK FolderDialog::BrowseCallBackFun(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
//	FolderDialog* pDlg=(FolderDialog*)lParam;
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		{
			//设置默认路径
			::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData);
			//在STATUSTEXT区域显示当前路径
			::SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,lpData);
		}
		break;
	case BFFM_SELCHANGED:
		{
			TCHAR pszPath[MAX_PATH];
			SHGetPathFromIDList((LPCITEMIDLIST)lParam,pszPath);
			::SendMessage(hwnd,BFFM_SETSTATUSTEXT,TRUE,(LPARAM)pszPath);
		}
		break;
	}
	return 0;
}

BOOL FolderDialog::IsDirectory(char* sPath)
{
	return PathIsDirectory(sPath);
}
