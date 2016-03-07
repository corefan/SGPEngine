#pragma once

#include "..\EditorTabCtrl.h"
#include "..\TerrainEditorDlg.h"
// CTerrainEditorPane

class CTerrainEditorPane : public CCommonDockablePane
{
	DECLARE_DYNAMIC(CTerrainEditorPane)
	friend class CMainFrame;
public:
	CTerrainEditorPane();
	virtual ~CTerrainEditorPane();

protected:
	DECLARE_MESSAGE_MAP()
private:
	CTerrainEditorDlg m_TerrainDlg;
	CEditorTabCtrl m_TabCtrl;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
