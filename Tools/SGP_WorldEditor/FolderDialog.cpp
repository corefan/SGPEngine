#include "stdafx.h"

#include "FolderDialog.h"
#include <shlobj.h>
#include <shlwapi.h>
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"shlwapi.lib")

CFolderDialog::CFolderDialog(HWND hParent,CString sTitle,CString sPath)
{
	m_hWndParent = hParent;
	m_Title=sTitle;
	m_FolderPath = sPath;
	m_DefaultPath = sPath;
}

UINT CFolderDialog::DoModal()
{
	TCHAR pszPath[MAX_PATH];
	BROWSEINFO bi;
	bi.hwndOwner=m_hWndParent;
	bi.pidlRoot=NULL;
	bi.pszDisplayName=NULL;
	bi.lpszTitle=m_Title;
	bi.ulFlags=BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT|BIF_USENEWUI;
	bi.lpfn=BrowseCallBackFun;
	bi.lParam=(LPARAM)(char*)(LPCTSTR)m_DefaultPath;
	bi.iImage=NULL;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if(pidl==NULL) return IDCANCEL;
	SHGetPathFromIDList(pidl,pszPath);
	m_FolderPath = pszPath;
	return IDOK;
}

CString CFolderDialog::GetPath()
{
	return m_FolderPath;
}

int CALLBACK CFolderDialog::BrowseCallBackFun(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		{
			//Set Default Path
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