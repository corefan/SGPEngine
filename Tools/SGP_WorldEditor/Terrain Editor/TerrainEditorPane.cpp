// .\Terrain Editor\TerrainEditorPane.cpp : 实现文件
//

#include "..\stdafx.h"
#include "..\WorldEditor.h"
#include "TerrainEditorPane.h"

#define ID_EDITOR_TAB 2013

// CTerrainEditorPane

IMPLEMENT_DYNAMIC(CTerrainEditorPane, CCommonDockablePane)

CTerrainEditorPane::CTerrainEditorPane()
{

}

CTerrainEditorPane::~CTerrainEditorPane()
{
}

BEGIN_MESSAGE_MAP(CTerrainEditorPane, CCommonDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CTerrainEditorPane 消息处理程序

BOOL CTerrainEditorPane::PreTranslateMessage(MSG* pMsg)
{
	

	return CCommonDockablePane::PreTranslateMessage(pMsg);
}


int CTerrainEditorPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCommonDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

//	m_TerrainDlg.Create(IDD_TERRAIN_EDITOR,this);
//	m_TerrainDlg.ShowWindow(SW_SHOW);
	CRect rect;
	GetClientRect(rect);
	m_TabCtrl.Create(CMFCTabCtrl::STYLE_3D,rect,this,ID_EDITOR_TAB,CMFCTabCtrl::LOCATION_TOP);

	return 0;
}


void CTerrainEditorPane::OnSize(UINT nType, int cx, int cy)
{
	CCommonDockablePane::OnSize(nType, cx, cy);

/*	if(m_TerrainDlg.m_hWnd!=NULL)
		m_TerrainDlg.SetWindowPos(NULL,0,0,cx,cy,SWP_SHOWWINDOW);*/
	if(m_TabCtrl.m_hWnd!=NULL)
		m_TabCtrl.SetWindowPos(NULL,0,0,cx,cy,SWP_NOZORDER|SWP_NOACTIVATE);
}

void CTerrainEditorPane::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 在此处添加消息处理程序代码
}
