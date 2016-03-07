// FindResultPane.cpp : 实现文件
//

#include "stdafx.h"
#include "FindResultPane.h"

#include "FindDlg.h"

// CFindResultPane

IMPLEMENT_DYNAMIC(CFindResultPane, CCommonDockablePane)

CFindResultPane::CFindResultPane()
{

}

CFindResultPane::~CFindResultPane()
{
}

BEGIN_MESSAGE_MAP(CFindResultPane, CCommonDockablePane)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CFindResultPane 消息处理程序

void CFindResultPane::OnSize(UINT nType, int cx, int cy)
{
	CCommonDockablePane::OnSize(nType, cx, cy);

	m_ResultList.MoveWindow(0,0,cx,cy);
}

int CFindResultPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCommonDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	m_ResultList.Create(dwStyle, rectDummy,this,112);

	return 0;
}

void CFindResultPane::FindObjects()
{
	MouseMsgInterface::GetInstance()->CheckUncompleteOp();
	CFindDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		ShowPane(TRUE,FALSE,TRUE);
		if(dlg.m_FindOption==IDC_OPTION_SCENE_NAME) m_ResultList.FindObjectsBySceneName(dlg.m_FindContent);
		else if(dlg.m_FindOption==IDC_OPTION_ID) m_ResultList.FindObjectsByID((uint32)atoi(dlg.m_FindContent));
		else if(dlg.m_FindOption==IDC_OPTION_MF1NAME) m_ResultList.FindObjectsByMF1Name(dlg.m_FindContent);
	}
}