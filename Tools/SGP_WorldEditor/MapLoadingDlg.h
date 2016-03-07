#pragma once
#include "afxcmn.h"


// CMapLoadingDlg �Ի���

class CMapLoadingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMapLoadingDlg)

public:
	CMapLoadingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMapLoadingDlg();

// �Ի�������
	enum { IDD = IDD_MAP_OPEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg LRESULT OnKickIdle(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_Progress;
	virtual BOOL OnInitDialog();
	void UpdateCtrls(int index,int total);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
