#pragma once

#include "..\SceneObjectTree.h"
#include "ObjLibToolbar.h"

enum ObjSearchMode
{
	SearchMode_ByName,
	SearchMode_ByID
};

// CSceneExplorerPane

class CSceneExplorerPane : public CCommonDockablePane
{
	DECLARE_DYNAMIC(CSceneExplorerPane)

public:
	CSceneExplorerPane();
	virtual ~CSceneExplorerPane();

protected:
	DECLARE_MESSAGE_MAP()
private:
	CSceneObjectTree m_SceneTree;
	CObjLibToolbar m_ToolBar;
	CMFCToolBarEditBoxButton m_SearchEdit;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnSearchObject();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSearchMode(UINT nID);
	afx_msg void OnUpdateSearchMode(CCmdUI* pCmdUI);
private:
	ObjSearchMode m_SearchMode;
public:
	void SearchObject(CString strInfo);
};
