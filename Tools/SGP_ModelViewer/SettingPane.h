#pragma once

// CSettingPane
#include "SettingDlg.h"
#include "CommonDockPane.h"

class CSettingPane : public /*CDockablePane*/CCommonDockPane
{
	DECLARE_DYNAMIC(CSettingPane)

public:
	CSettingPane();
	virtual ~CSettingPane();

protected:
	DECLARE_MESSAGE_MAP()
public:
//	CSettingDlg m_SettingDlg;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnSettingChanged(WPARAM wParam,LPARAM lParam);
private:
//	CMFCPropertyGridCtrl m_SettingList;
	CSettingDlg m_SettingDlg;
	CFont m_Font;
	void InitSettingList();
};


