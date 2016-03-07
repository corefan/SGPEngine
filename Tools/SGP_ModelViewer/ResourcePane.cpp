// ResourcePane.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ResourcePane.h"

#include <vector>
// CResourcePane

IMPLEMENT_DYNAMIC(CResourcePane, /*CDockablePane*/CCommonDockPane)

CResourcePane::CResourcePane()\
	:m_SearchEdit(IDE_RESOURCE_EDIT,-1,ES_AUTOHSCROLL,60)
{
	m_bShowMF1=TRUE;
	m_bShowBF1=TRUE;
	m_bShowDDS=TRUE;
	m_bShowTGA=TRUE;
	AddFilter(CString(_T(".mf1")));
	AddFilter(CString(_T(".bf1")));
	AddFilter(CString(_T(".tga")));
	AddFilter(CString(_T(".dds")));
}

CResourcePane::~CResourcePane()
{
}


BEGIN_MESSAGE_MAP(CResourcePane, /*CDockablePane*/CCommonDockPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(IDM_RESOURCE_MF1,IDM_RESOURCE_TGA,OnFilterMenu)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_RESOURCE_MF1,IDM_RESOURCE_TGA,OnUpdateFilterMenu)
	ON_COMMAND(IDB_RESOURCE_FILTER,OnResourceFilter)
	ON_COMMAND(IDE_RESOURCE_EDIT,OnSearchEdit)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CResourcePane 消息处理程序

int CResourcePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (/*CDockablePane*/CCommonDockPane::OnCreate(lpCreateStruct) == -1)
		return -1;
	//create tabctrl
	CRect rect;
	GetClientRect(rect);
	m_TabCtrl.Create(CMFCTabCtrl::STYLE_3D,rect,this,ID_RESOURCE_TAB,CMFCTabCtrl::LOCATION_BOTTOM);

	//create toolbar
	m_ToolBar.Create(this,AFX_DEFAULT_TOOLBAR_STYLE,IDT_RESOURCE_TOOLBAR);
	m_ToolBar.LoadToolBar(IDT_RESOURCE_TOOLBAR,0,0,TRUE);
	m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
	m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()&~(CBRS_GRIPPER|\
		CBRS_SIZE_DYNAMIC|CBRS_BORDER_LEFT|CBRS_BORDER_RIGHT));
	m_ToolBar.SetRouteCommandsViaFrame(FALSE);

	m_ToolBar.ReplaceButton(IDT_ITEM1,CMFCToolBarButton(IDB_RESOURCE_FILTER,-1,_T("Filter")));

/*	CMenu menu;
	menu.LoadMenu(IDM_RESOURCE_FILTER);
	m_ToolBar.ReplaceButton(IDT_ITEM1,CMFCToolBarMenuButton(-1,menu.GetSubMenu(0)->GetSafeHmenu(),-1,_T("Filter:")));
	CMFCToolBarMenuButton* pButton =  DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_ToolBar.GetButton(0));
	pButton->SetMessageWnd(this);
	*/
//	CMFCToolBarButton *pButton=new CMFCToolBarButton(IDM_RESOURCE_MF1,-1,_T("Filter"));
//	m_ToolBar.ReplaceButton(IDT_ITEM1,*pButton);
	
/*	m_SearchEdit.CanBeStretched();
	m_SearchEdit.HaveHotBorder();
	m_SearchEdit.SetContents(_T(""));
	m_SearchEdit.SetFlatMode(TRUE);
	m_SearchEdit.SetStyle(TBBS_PRESSED);
	m_ToolBar.InsertButton(m_SearchEdit,1);*/
	
/*	m_wndToolBar.InsertButton(CMFCToolBarComboBoxButton(ID_GO_ADDRESS_EDIT, 0), 3);
	//设置组合框属性
	m_comboAddress = (CMFCToolBarComboBoxButton*)m_wndToolBar.GetButton(3); //假设插入在第三个按钮
	m_comboAddress->EnableWindow(TRUE);
	m_comboAddress->SetCenterVert();
	m_comboAddress->SetDropDownHeight(100);//设置下拉列表的高度
	m_comboAddress->SetFlatMode();

	//添加内容
	m_comboAddress->AddItem(_T(""));
	m_comboAddress->AddItem(_T(""));
	m_comboAddress->AddItem(_T(""));
	m_comboAddress->AddItem(_T(""));
	m_comboAddress->SelectItem(0);	 //默认选中*/

	return 0;
}


void CResourcePane::OnSize(UINT nType, int cx, int cy)
{
	//CDockablePane::OnSize(nType, cx, cy);
	CCommonDockPane::OnSize(nType,cx,cy);

	CRect rect;
	GetClientRect(rect);

	int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	if(m_ToolBar.m_hWnd!=NULL)
		m_ToolBar.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),cyTlb,SWP_NOZORDER|SWP_NOACTIVATE);
	if(m_TabCtrl.m_hWnd!=NULL)
		m_TabCtrl.SetWindowPos(NULL,rect.left,rect.top+cyTlb,rect.Width(),rect.Height()-cyTlb,SWP_NOZORDER|SWP_NOACTIVATE);
	
}

void CResourcePane::OnFilterMenu(UINT nID)
{
	switch(nID)
	{
	case IDM_RESOURCE_MF1:
		m_bShowMF1=!m_bShowMF1;
		if(m_bShowMF1) AddFilter(CString(_T(".mf1")));
		else DeleteFilter(CString(_T(".mf1")));
		break;
	case IDM_RESOURCE_BF1:
		m_bShowBF1=!m_bShowBF1;
		if(m_bShowBF1) AddFilter(CString(_T(".bf1")));
		else DeleteFilter(CString(_T(".bf1")));
		break;
	case IDM_RESOURCE_TGA:
		m_bShowTGA=!m_bShowTGA;
		if(m_bShowTGA) AddFilter(CString(_T(".tga")));
		else DeleteFilter(CString(_T(".tga")));
		break;
	case IDM_RESOURCE_DDS:
		m_bShowDDS=!m_bShowDDS;
		if(m_bShowDDS) AddFilter(CString(_T(".dds")));
		else DeleteFilter(CString(_T(".dds")));
		break;
	}

	//refresh treectrl
	std::vector<CResourceTreeCtrl*>& treeVector=m_TabCtrl.m_TreeVector;
	std::vector<CString>& pathVector=m_TabCtrl.m_PathVector;
	int count=treeVector.size();
	for(int i=0;i<count;++i)
	{
		treeVector[i]->DeleteAllItems();
		treeVector[i]->Insert(treeVector[i]->m_DirPath);
	}
}

void CResourcePane::OnUpdateFilterMenu(CCmdUI *pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case IDM_RESOURCE_MF1:
		pCmdUI->SetCheck(m_bShowMF1);
		break;
	case IDM_RESOURCE_BF1:
		pCmdUI->SetCheck(m_bShowBF1);
		break;
	case IDM_RESOURCE_TGA:
		pCmdUI->SetCheck(m_bShowTGA);
		break;
	case IDM_RESOURCE_DDS:
		pCmdUI->SetCheck(m_bShowDDS);
		break;
	}
	pCmdUI->Enable();
}

void CResourcePane::OnSearchEdit()
{

}

void CResourcePane::OnResourceFilter()
{
	CMenu menu;
	menu.LoadMenu(IDM_RESOURCE_FILTER);
//	CPoint pt;
//	GetCursorPos(&pt);
	CMenu* pSubMenu=menu.GetSubMenu(0);
	CRect rect;
	m_ToolBar.GetItemRect(0,&rect);
	ClientToScreen(&rect);
	if(m_bShowMF1) pSubMenu->CheckMenuItem(IDM_RESOURCE_MF1,MF_BYCOMMAND|MF_CHECKED);
	if(m_bShowBF1) pSubMenu->CheckMenuItem(IDM_RESOURCE_BF1,MF_BYCOMMAND|MF_CHECKED);
	if(m_bShowDDS) pSubMenu->CheckMenuItem(IDM_RESOURCE_DDS,MF_BYCOMMAND|MF_CHECKED);
	if(m_bShowTGA) pSubMenu->CheckMenuItem(IDM_RESOURCE_TGA,MF_BYCOMMAND|MF_CHECKED);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,rect.left,rect.bottom,this);

/*	UINT nMenuResult=afxContextMenuManager->TrackPopupMenu(pSubMenu->m_hMenu,rect.left,rect.bottom, this);
	OnFilterMenu(nMenuResult);*/
}

void CResourcePane::AddFilter(CString str)
{
	std::vector<CString>::iterator iter=find(m_FileFilter.begin(),m_FileFilter.end(),str);
	if(iter==m_FileFilter.end())
		m_FileFilter.push_back(str);
}

void CResourcePane::DeleteFilter(CString str)
{
	std::vector<CString>::iterator iter=find(m_FileFilter.begin(),m_FileFilter.end(),str);
	if(iter!=m_FileFilter.end())
		m_FileFilter.erase(iter);
}


void CResourcePane::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 在此处添加消息处理程序代码
}
