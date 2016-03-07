// LogViewerPane.cpp : 实现文件
//

#include "..\stdafx.h"
#include "LogViewerPane.h"

#define IDE_LOGVIEWER_RICHEDIT 2000
// CLogViewerPane

IMPLEMENT_DYNAMIC(CLogViewerPane, CCommonDockablePane)

CLogViewerPane::CLogViewerPane()
{

}

CLogViewerPane::~CLogViewerPane()
{
}

BEGIN_MESSAGE_MAP(CLogViewerPane, CCommonDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CLogViewerPane 消息处理程序

int CLogViewerPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCommonDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	//create richedit
	CRect rect;
	GetClientRect(rect);
	m_RichEdit.Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE\
		|ES_WANTRETURN|ES_AUTOHSCROLL|ES_READONLY,rect,this,IDE_LOGVIEWER_RICHEDIT);

	return 0;
}

void CLogViewerPane::OnSize(UINT nType, int cx, int cy)
{
	CCommonDockablePane::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(rect);
	int cyTlb =0;
	if(m_RichEdit.m_hWnd!=NULL)
		m_RichEdit.SetWindowPos(NULL,rect.left,rect.top+cyTlb,rect.Width(),rect.Height()-cyTlb,SWP_NOZORDER|SWP_NOACTIVATE);
}

void CLogViewerPane::OnContextMenu(CWnd* pWnd, CPoint point)
{
	
}
