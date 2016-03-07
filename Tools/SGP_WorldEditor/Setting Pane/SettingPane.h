#pragma once

#include "SettingDlg.h"
// CSettingPane

class CSettingPane : public CCommonDockablePane
{
	DECLARE_DYNAMIC(CSettingPane)

public:
	CSettingPane();
	virtual ~CSettingPane();
	static CSettingPane* s_pSettingPane;
	static CSettingPane* GetInstance(){ASSERT(s_pSettingPane!=NULL);return s_pSettingPane;}
protected:
	DECLARE_MESSAGE_MAP()
private:
	CSettingDlg m_SettingDlg;
	CMFCPropertyGridCtrl m_SettingList;
	CFont m_Font;

	CMFCPropertyGridProperty* m_pNearPlaneProperty;
	CMFCPropertyGridProperty* m_pFarPlaneProperty;
	CMFCPropertyGridProperty* m_pFovProperty;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
public:
	void InitSettingList();
	void SetShowOption(int index,BOOL bShow);
	afx_msg LRESULT OnSettingChanged(WPARAM wParam,LPARAM lParam);
};
