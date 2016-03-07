// LogViewerPane.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "LogViewerPane.h"


// CLogViewerPane

IMPLEMENT_DYNAMIC(CLogViewerPane, /*CDockablePane*/CCommonDockPane)

CLogViewerPane::CLogViewerPane()
{

}

CLogViewerPane::~CLogViewerPane()
{
}


BEGIN_MESSAGE_MAP(CLogViewerPane, /*CDockablePane*/CCommonDockPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CLogViewerPane 消息处理程序

int CLogViewerPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (/*CDockablePane*/CCommonDockPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	//create toolbar
/*	m_ToolBar.Create(this,AFX_DEFAULT_TOOLBAR_STYLE,IDT_LOGVIEWER_TOOLBAR);
	m_ToolBar.LoadToolBar(IDT_LOGVIEWER_TOOLBAR,0,0,TRUE);
	m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
	m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()&~(CBRS_GRIPPER|\
		CBRS_SIZE_DYNAMIC|CBRS_BORDER_TOP|CBRS_BORDER_BOTTOM|CBRS_BORDER_LEFT|CBRS_BORDER_RIGHT));
	m_ToolBar.SetOwner(this);
	m_ToolBar.SetRouteCommandsViaFrame(FALSE);*/

	//create richedit
	CRect rect;
	GetClientRect(rect);
	m_RichEdit.Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE\
		|ES_WANTRETURN|ES_AUTOHSCROLL|ES_READONLY,rect,this,IDE_LOGVIEWER_RICHEDIT);

	/*	for(int i=0;i<10;i++)
	{
	m_RichEdit.SetSel(-1,-1);
	CString str;
		str.Format(_T("Line %d Text\r\n"),i);
		m_RichEdit.ReplaceSel(str);

		CHARFORMAT2 cFormat;
	//	m_RichEdit.SetSel(0,-1);
		cFormat.cbSize=sizeof(cFormat);
		cFormat.dwMask=CFM_COLOR;
		cFormat.crTextColor=RGB(0,25*i,0);
		cFormat.dwEffects &= ~CFE_AUTOCOLOR;
		m_RichEdit.SetSelectionCharFormat(cFormat);
	}*/

/*	CHARFORMAT2 cFormat;
	m_RichEdit.SetSel(0,-1);
	cFormat.cbSize=sizeof(cFormat);
	cFormat.dwMask=CFM_COLOR;
	cFormat.crTextColor=RGB(0,255,0);
	cFormat.dwEffects &= ~CFE_AUTOCOLOR;
	m_RichEdit.SetSelectionCharFormat(cFormat);*/
//	g_RichCtrl=&m_RichEdit;

	//Init1(m_hWnd);
	return 0;
}


void CLogViewerPane::OnSize(UINT nType, int cx, int cy)
{
	/*CDockablePane*/CCommonDockPane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(rect);
	int cyTlb =0;
/*	cyTlb=m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	if(m_ToolBar.m_hWnd!=NULL)
		m_ToolBar.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),cyTlb,SWP_NOZORDER|SWP_NOACTIVATE);*/
	if(m_RichEdit.m_hWnd!=NULL)
		m_RichEdit.SetWindowPos(NULL,rect.left,rect.top+cyTlb,rect.Width(),rect.Height()-cyTlb,SWP_NOZORDER|SWP_NOACTIVATE);
}
