#pragma once

#include "EditorTabCtrl.h"
// CSceneEditorPane

class CEditorPane : public CCommonDockablePane
{
	DECLARE_DYNAMIC(CEditorPane)
	static CEditorPane* s_pEditorPane;
public:
	static CEditorPane* GetInstance(){ASSERT(s_pEditorPane!=NULL);return s_pEditorPane;}
	CEditorPane();
	virtual ~CEditorPane();
protected:
	DECLARE_MESSAGE_MAP()
private:
	CEditorTabCtrl m_TabCtrl;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	void EnableAllCtrls(BOOL bEnable);
};
