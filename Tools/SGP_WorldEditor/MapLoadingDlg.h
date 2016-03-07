#pragma once
#include "afxcmn.h"


// CMapLoadingDlg 对话框

class CMapLoadingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMapLoadingDlg)

public:
	CMapLoadingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMapLoadingDlg();

// 对话框数据
	enum { IDD = IDD_MAP_OPEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnKickIdle(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_Progress;
	virtual BOOL OnInitDialog();
	void UpdateCtrls(int index,int total);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
