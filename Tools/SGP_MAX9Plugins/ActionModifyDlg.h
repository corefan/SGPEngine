#pragma once

//动作修改对话框
class ActionModifyDlg
{
public:
	ActionModifyDlg(HWND hwnd);
	void DoModal();
public:
	//窗口过程回调函数
	static INT_PTR CALLBACK ActionModifyDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	/**********************************************************
	消息处理函数
	**********************************************************/
	static void InitDlg(HWND hwnd);
	static void InitCtrl(HWND hwnd);
	static BOOL CheckModifiedAction(HWND hwnd);
	static BOOL CheckActionFileNo(HWND hwnd);
	static void ModifySelectedAction(HWND hwnd);
private:
	static HWND m_HwndParent;//父窗口句柄
};

