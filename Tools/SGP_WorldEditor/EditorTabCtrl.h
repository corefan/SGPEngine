#pragma once

#include "TerrainEditorDlg.h"
#include "SceneEditorDlg.h"

// CEditorTabCtrl

class CEditorTabCtrl : public CMFCTabCtrl
{
	DECLARE_DYNAMIC(CEditorTabCtrl)

public:
	CEditorTabCtrl();
	virtual ~CEditorTabCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL SetActiveTab(int iTab);

	CTerrainEditorDlg m_TerrainDlg;
	CSceneEditorDlg m_SceneDlg;
	void EnableAllCtrls(BOOL bEnable);
};


