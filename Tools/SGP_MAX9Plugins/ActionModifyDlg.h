#pragma once

//�����޸ĶԻ���
class ActionModifyDlg
{
public:
	ActionModifyDlg(HWND hwnd);
	void DoModal();
public:
	//���ڹ��̻ص�����
	static INT_PTR CALLBACK ActionModifyDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	/**********************************************************
	��Ϣ������
	**********************************************************/
	static void InitDlg(HWND hwnd);
	static void InitCtrl(HWND hwnd);
	static BOOL CheckModifiedAction(HWND hwnd);
	static BOOL CheckActionFileNo(HWND hwnd);
	static void ModifySelectedAction(HWND hwnd);
private:
	static HWND m_HwndParent;//�����ھ��
};

