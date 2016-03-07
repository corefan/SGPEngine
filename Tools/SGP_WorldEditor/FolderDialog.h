#pragma once

class CFolderDialog
{
public:
	CFolderDialog(HWND hParent,CString sTitle,CString sPath="");
	UINT DoModal();
	CString GetPath();
private:
	HWND m_hWndParent;
	CString m_Title;
	CString m_FolderPath;
	CString m_DefaultPath;
public:
	//callback function
	static int CALLBACK BrowseCallBackFun(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData);
};