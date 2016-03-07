// SceneEditorPane.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "EditorPane.h"

#define ID_EDITOR_TAB 2013

// CSceneEditorPane

IMPLEMENT_DYNAMIC(CEditorPane, CCommonDockablePane)

CEditorPane* CEditorPane::s_pEditorPane=NULL;

CEditorPane::CEditorPane()
{

}

CEditorPane::~CEditorPane()
{
}

BEGIN_MESSAGE_MAP(CEditorPane, CCommonDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CSceneEditorPane 消息处理程序

int CEditorPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCommonDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(rect);
	m_TabCtrl.Create(CMFCTabCtrl::STYLE_3D,rect,this,ID_EDITOR_TAB,CMFCTabCtrl::LOCATION_TOP);

	s_pEditorPane=this;

	return 0;
}

void CEditorPane::OnSize(UINT nType, int cx, int cy)
{
	CCommonDockablePane::OnSize(nType, cx, cy);

	if(m_TabCtrl.m_hWnd!=NULL)
		m_TabCtrl.SetWindowPos(NULL,0,0,cx,cy,SWP_NOZORDER|SWP_NOACTIVATE);
}

void CEditorPane::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 在此处添加消息处理程序代码
}

void CEditorPane::EnableAllCtrls(BOOL bEnable)
{
	m_TabCtrl.EnableAllCtrls(bEnable);
}
