// LogViewerEdit.cpp : 实现文件
//

#include "..\stdafx.h"
#include "LogViewerEdit.h"

#define IDM_CLEAR_ALLCONTENT 2001

extern CSGP_WorldEditorApp theApp;

// CLogViewerEdit

IMPLEMENT_DYNAMIC(CLogViewerEdit, CRichEditCtrl)

CLogViewerEdit::CLogViewerEdit()
{

}

CLogViewerEdit::~CLogViewerEdit()
{
}

BEGIN_MESSAGE_MAP(CLogViewerEdit, CRichEditCtrl)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CLogViewerEdit 消息处理程序

int CLogViewerEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Font.CreatePointFont(90,_T("微软雅黑"));
	SetFont(&m_Font);
	ModifyStyle(0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	return 0;
}

BOOL CLogViewerEdit::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC to exit richedit control
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	return CRichEditCtrl::PreTranslateMessage(pMsg);
}

void CLogViewerEdit::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_CLEAR_ALLCONTENT,"Clear All");
	CPoint pt;
	::GetCursorPos(&pt);
	UINT nMenuResult=theApp.GetContextMenuManager()->TrackPopupMenu(menu.m_hMenu, pt.x, pt.y, this);
	switch(nMenuResult)
	{
	case IDM_CLEAR_ALLCONTENT:
		SetWindowText("");
		break;
	}
}
