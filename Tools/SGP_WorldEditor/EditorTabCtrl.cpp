// EditorTabCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "EditorTabCtrl.h"

#include "MouseMsgInterface.h"
// CEditorTabCtrl

IMPLEMENT_DYNAMIC(CEditorTabCtrl, CMFCTabCtrl)

CEditorTabCtrl::CEditorTabCtrl()
{

}

CEditorTabCtrl::~CEditorTabCtrl()
{
}

BEGIN_MESSAGE_MAP(CEditorTabCtrl, CMFCTabCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CEditorTabCtrl 消息处理程序

int CEditorTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	EnableTabSwap(FALSE);

	m_TerrainDlg.Create(IDD_TERRAIN_EDITOR,this);
	m_TerrainDlg.ShowWindow(SW_SHOW);
	m_SceneDlg.Create(IDD_SCENE_EDITOR,this);
	m_SceneDlg.ShowWindow(SW_SHOW);

	AddTab(&m_TerrainDlg,"Terrain",-1);
	AddTab(&m_SceneDlg,"Object",-1);

	return 0;
}

BOOL CEditorTabCtrl::SetActiveTab(int iTab)
{
	if(GetActiveTab()!=iTab)
	{
		m_TerrainDlg.SetButtonState(-1);
		switch(iTab)
		{
		case 0:
			CSceneObjectTree::GetInstance()->ClearSelection();
			MouseMsgInterface::GetInstance()->SwitchToNULL();
			break;
		case 1:
			MouseMsgInterface::GetInstance()->SwitchToSelectEditor();
			break;
		}
	}

	return CMFCTabCtrl::SetActiveTab(iTab);
}

void CEditorTabCtrl::EnableAllCtrls(BOOL bEnable)
{
	m_SceneDlg.EnableAllCtrls(bEnable);
	m_TerrainDlg.EnableAllCtrls(bEnable);
}