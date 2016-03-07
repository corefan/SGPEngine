#pragma once

class FolderDialog
{
public:
	FolderDialog(HWND hparent,char* strTitle,char* strPath="");

	void DoModal();
	char* GetPath();
	BOOL IsDirectory();
private:
	HWND m_hWndParent;
	char m_Title[MAX_PATH];
	char m_DefaultPath[MAX_PATH];
	char m_FolderPath[MAX_PATH];
public:
	//callback function
	static int CALLBACK BrowseCallBackFun(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
public:
	static BOOL IsDirectory(char* sPath);
};

