// SceneExplorerPane.cpp : 实现文件
//

#include "..\stdafx.h"
#include "SceneExplorerPane.h"
#include "WorldMapManager.h"

#define IDE_SEARCH_EDIT 1000
#define IDB_SEARCH_INFO 1001
#define IDM_SEARCH_BY_NAME 1002
#define IDM_SEARCH_BY_ID 1003
// CSceneExplorerPane

IMPLEMENT_DYNAMIC(CSceneExplorerPane, CCommonDockablePane)

CSceneExplorerPane::CSceneExplorerPane()\
	:m_SearchEdit(IDE_SEARCH_EDIT,-1,ES_AUTOHSCROLL,200)
{
	m_SearchMode=SearchMode_ByName;
}

CSceneExplorerPane::~CSceneExplorerPane()
{
}


BEGIN_MESSAGE_MAP(CSceneExplorerPane, CCommonDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDE_SEARCH_EDIT,OnSearchObject)
	ON_COMMAND_RANGE(IDM_SEARCH_BY_NAME,IDM_SEARCH_BY_ID,OnSearchMode)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_SEARCH_BY_NAME,IDM_SEARCH_BY_ID,OnUpdateSearchMode)
END_MESSAGE_MAP()

// CSceneExplorerPane 消息处理程序

int CSceneExplorerPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCommonDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Scene Tree
	const DWORD dwTreeStyle = WS_CHILD|WS_VISIBLE|TVS_HASLINES/*|TVS_LINESATROOT*/|TVS_HASBUTTONS/*|TVS_SHOWSELALWAYS*/;
	m_SceneTree.Create(dwTreeStyle,CRect(),this,100);
	m_SceneTree.Init();

	// create Toolbar
/*	m_ToolBar.Create(this,AFX_DEFAULT_TOOLBAR_STYLE,IDR_OBJECTLIB_TOOLBAR);
	m_ToolBar.LoadToolBar(IDR_OBJECTLIB_TOOLBAR,0,0,TRUE);
	m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
	m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()&~(CBRS_GRIPPER|\
		CBRS_SIZE_DYNAMIC|CBRS_BORDER_LEFT|CBRS_BORDER_RIGHT));
	m_ToolBar.SetRouteCommandsViaFrame(FALSE);
	// Add Toolbar button
//	m_ToolBar.ReplaceButton(IDT_SERACH_INFO,CMFCToolBarButton(IDB_SEARCH_INFO,-1,_T("Search:")));
	CMenu menuOption;
	menuOption.CreatePopupMenu();
	menuOption.AppendMenu(MF_STRING,IDM_SEARCH_BY_NAME,"By Name");
	menuOption.AppendMenu(MF_STRING,IDM_SEARCH_BY_ID,"By ID");
	
	m_ToolBar.ReplaceButton(IDT_SERACH_INFO, CMFCToolBarMenuButton(-1,menuOption.GetSafeHmenu(),-1,"Search:"));
	CMFCToolBarMenuButton* pButton =  DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0));
	pButton->SetMessageWnd(this);

	m_SearchEdit.CanBeStretched();
	m_SearchEdit.HaveHotBorder();
	m_SearchEdit.SetContents(_T(""));
	m_SearchEdit.SetFlatMode(TRUE);
	m_SearchEdit.SetStyle(TBBS_PRESSED);
	m_ToolBar.ReplaceButton(IDT_SEARCH_ITEM,m_SearchEdit);*/

	return 0;
}

void CSceneExplorerPane::OnSize(UINT nType, int cx, int cy)
{
	CCommonDockablePane::OnSize(nType, cx, cy);

	int cyTlb=0;
	CRect rect;
	GetClientRect(rect);
/*	cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	if(m_ToolBar.m_hWnd!=NULL)
		m_ToolBar.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),cyTlb,SWP_NOZORDER|SWP_NOACTIVATE);*/
	if(m_SceneTree.m_hWnd!=NULL)
		m_SceneTree.SetWindowPos(NULL,rect.left,rect.top+cyTlb,rect.Width(),rect.Height()-cyTlb,SWP_NOZORDER|SWP_NOACTIVATE);
}

void CSceneExplorerPane::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	
}

void CSceneExplorerPane::OnSearchObject()
{

}

BOOL CSceneExplorerPane::PreTranslateMessage(MSG* pMsg)
{
	UINT nID = ::GetDlgCtrlID(pMsg->hwnd);
	if(nID==IDE_SEARCH_EDIT)
	{
		if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
		{
			SearchObject(m_SearchEdit.GetContentsAll(IDE_SEARCH_EDIT));
		}
	}

	return CCommonDockablePane::PreTranslateMessage(pMsg);
}

void CSceneExplorerPane::OnSearchMode(UINT nID)
{
	switch(nID)
	{
	case IDM_SEARCH_BY_NAME:
		m_SearchMode=SearchMode_ByName;
		break;
	case IDM_SEARCH_BY_ID:
		m_SearchMode=SearchMode_ByID;
		break;
	}
}

void CSceneExplorerPane::OnUpdateSearchMode(CCmdUI* pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case IDM_SEARCH_BY_NAME:
		pCmdUI->SetCheck(m_SearchMode==SearchMode_ByName);
		break;
	case IDM_SEARCH_BY_ID:
		pCmdUI->SetCheck(m_SearchMode==SearchMode_ByID);
		break;
	}
}

void CSceneExplorerPane::SearchObject(CString strInfo)
{
	strInfo.TrimLeft();
	strInfo.TrimRight();
	if(strInfo.IsEmpty()) return ;
	if(m_SearchMode==SearchMode_ByName)
	{
		if(!m_SceneTree.SearchObject(strInfo))
			MessageBox("Can't find objects!","WorldEditor",MB_OK|MB_ICONERROR);
	}
	else if(m_SearchMode==SearchMode_ByID)
	{
		for(int i=0;i<strInfo.GetLength();++i)
		{
			char c=strInfo.GetAt(i);
			if(c<'0'||c>'9')
			{
				MessageBox("Please input ID in correct format!","WorldEditor",MB_OK|MB_ICONERROR);
				return ;
			}
		}
		int iID=atoi(strInfo);
		if(!m_SceneTree.SearchObject(iID))
			MessageBox("Can't find objects!","WorldEditor",MB_OK|MB_ICONERROR);
	}
}