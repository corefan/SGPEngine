#pragma once


// CResourcePane
#include "ResourceTabCtrl.h"
#include "ResourceToolBar.h"

#include <vector>
#include <algorithm>
#include "CommonDockPane.h"

class CResourcePane : public /*CDockablePane*/CCommonDockPane
{
	DECLARE_DYNAMIC(CResourcePane)

public:
	CResourcePane();
	virtual ~CResourcePane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFilterMenu(UINT nID);
	afx_msg void OnUpdateFilterMenu(CCmdUI *pCmdUI);
	afx_msg void OnSearchEdit();
	afx_msg void OnResourceFilter();
public:
	void AddFilter(CString str);//add filter
	void DeleteFilter(CString str);//delete filter
public:
	CResourceTabCtrl m_TabCtrl;
	CResourceToolBar m_ToolBar;
	CMFCToolBarEditBoxButton m_SearchEdit;
	BOOL m_bShowMF1;
	BOOL m_bShowBF1;
	BOOL m_bShowDDS;
	BOOL m_bShowTGA;
	std::vector<CString> m_FileFilter;//filter

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};


