
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "WorldEditor.h"

#include "MainFrame.h"
#include <io.h>

#include "..\SGP_ModelViewer\CommonVisualManager.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "WorldEditorConfig.h"
#include "NewMapConfigDlg.h"
#include "OperationManager.h"
#include "TerrainHeightHandler.h"
#include "TerrainTextureHandler.h"
#include "TerrainGrassHandler.h"
#include "TerrainWaterHandler.h"
#include "AddObjectHandler.h"
#include "AddLightObjectHandler.h"
#include "CloneObjHandler.h"
#include "SelectObjHandler.h"
#include "MouseMsgInterface.h"
#include "WorldMapManager.h"
#include "NewMapLoadingDlg.h"
#include "WorldEditorConfig.h"
#include "DragMoveHandler.h"
#include "DragScaleHandler.h"
#include "DragRotateHandler.h"
#include "SceneObjectManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_LOGVIEWER_PANE		1000
#define ID_SCENEEXPLORER_PANE	1001
#define ID_TERRAINEDITOR_PANE	1002
#define ID_EDITOR_PANE			1003
#define ID_SETTING_PANE			1004
#define ID_OBJATTRIBUTE_PANE	1005
#define ID_FINDRESULT_PANE		1006
// CMainFrame
#define ID_SHOW_SELECTED_OJBNUM 1007

extern CSGP_WorldEditorApp theApp;

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND(IDM_MODULE_MODELVIEWER,OnModuleModelViewer)
	ON_UPDATE_COMMAND_UI(IDM_MODULE_MODELVIEWER,OnUpdateModuleModelViewer)
	ON_WM_CLOSE()
	ON_MESSAGE(UM_RENDERINIT,InitRender)
	ON_COMMAND_RANGE(IDM_VIEW_SCENE_EXPLORER,IDM_VIEW_STATUSBAR,OnMenuView)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_VIEW_SCENE_EXPLORER,IDM_VIEW_STATUSBAR,OnUpdateMenuView)
	ON_COMMAND(IDM_FILE_NEW,OnFileNew)
	ON_UPDATE_COMMAND_UI(IDM_FILE_NEW,OnUpdateFileNew)
	ON_COMMAND(IDM_FILE_OPEN,OnFileOpen)
	ON_UPDATE_COMMAND_UI(IDM_FILE_OPEN,OnUpdateFileOpen)
	ON_COMMAND(IDM_FILE_SAVE,OnFileSave)
	ON_UPDATE_COMMAND_UI(IDM_FILE_SAVE,OnUpdateFileSave)
	ON_COMMAND(IDM_FILE_SAVE_AS,OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(IDM_FILE_SAVE_AS,OnUpdateFileSaveAs)
	ON_COMMAND(IDM_FILE_CLOSE,OnFileClose)
	ON_UPDATE_COMMAND_UI(IDM_FILE_CLOSE,OnUpdateFileClose)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_EDIT_UNDO,IDM_EDIT_REDO,OnUpdateEdit)
	ON_UPDATE_COMMAND_UI_RANGE(RECENT_FILES_START,RECENT_FILES_END,OnUpdateRecentFiles)
	ON_COMMAND_RANGE(RECENT_FILES_START,RECENT_FILES_END,OnRecentFiles)
	ON_COMMAND(IDM_OBJ_MOVE,OnMenuSceneMove)
	ON_UPDATE_COMMAND_UI(IDM_OBJ_MOVE,OnUpdateMenuSceneMove)
	ON_COMMAND(IDM_OBJ_ROTATE,OnMenuSceneRotate)
	ON_UPDATE_COMMAND_UI(IDM_OBJ_ROTATE,OnUpdateMenuSceneRotate)
	ON_COMMAND(IDM_OBJ_SCALE,OnMenuSceneScale)
	ON_UPDATE_COMMAND_UI(IDM_OBJ_SCALE,OnUpdateMenuSceneScale)
	ON_COMMAND(IDM_TOOLS_BUILDLIGHTMAP,OnToolsBuildLightmap)
	ON_UPDATE_COMMAND_UI(IDM_TOOLS_BUILDLIGHTMAP,OnUpdateToolsBuildLightmap)
	ON_COMMAND(IDM_TOOLS_PACKUP_RESOURCE,OnToolsPackResForMultiPlatform)
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(IDM_VIEW_FULLSCREEN,OnViewFullscreen)
	ON_COMMAND(IDM_EDIT_FIND,OnEditFind)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SELECTED_OJBNUM,OnUpdateStatusSelectedObjNum)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_SHOW_SELECTED_OJBNUM
/*	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,*/
};

#ifdef _DEBUG
#include <vld.h>
#endif

CMainFrame* CMainFrame::s_pMainFrame=NULL;

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
	m_bFullScreen=FALSE;
}

CMainFrame::~CMainFrame()
{
	ReleaseAllStaticInstance();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	s_pMainFrame=this;

	EnableDocking(CBRS_ALIGN_ANY);

	m_Font.CreatePointFont(86,_T("微软雅黑"));
	LOGFONT logFont;
	m_Font.GetLogFont(&logFont);
	CMFCMenuBar::SetMenuFont(&logFont);
	
	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	// 允许用户定义的工具栏操作:
//	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	// Menu Bar
	m_wndMenuBar.Create(this);
	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle()|CBRS_SIZE_DYNAMIC|CBRS_TOOLTIPS|CBRS_FLYBY);
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);

	// Standard ToolBar
	m_wndToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC);
	m_wndToolBar.LoadToolBar(IDR_MAINFRAME_256);
	m_wndToolBar.SetWindowText("Standard");
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndToolBar);

	// Scene ToolBar
	m_SceneToolBar.Create(this,AFX_DEFAULT_TOOLBAR_STYLE,IDR_SCENE_TOOLBAR);
	m_SceneToolBar.LoadToolBar(IDR_SCENE_TOOLBAR);
	m_SceneToolBar.SetWindowText("Object Toolbar");
	m_SceneToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_SceneToolBar);
	m_SceneToolBar.SendMessage(TB_SETEXTENDEDSTYLE,0,TBSTYLE_EX_DRAWDDARROWS);
	
	// StatusBar
	m_StatusBar.Create(this);
	m_StatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	m_StatusBar.SetPaneWidth(1,300);
	m_StatusBar.SetPaneText(1,"");

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CCommonVisualManager));
	CDockingManager::SetDockingMode(DT_SMART);
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	if(!m_SceneExplorerPane.Create(_T("Object Library"),this,CRect(0,0,200,200),TRUE,\
		ID_SCENEEXPLORER_PANE,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_FLOAT_MULTI|CBRS_LEFT))
		return -1;
	m_SceneExplorerPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_SceneExplorerPane);

	if(!m_EditorPane.Create(_T("Editor Panel"),this,CRect(0, 0, 300, 200),TRUE,\
		ID_EDITOR_PANE,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_FLOAT_MULTI|CBRS_RIGHT))
		return -1;
	m_EditorPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_EditorPane);

	if(!m_ObjAttrPane.Create(_T("Attributes"),this,CRect(0, 0, 300, 300),TRUE,\
		ID_OBJATTRIBUTE_PANE,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_FLOAT_MULTI))
		return -1;
	m_ObjAttrPane.EnableDocking(CBRS_ALIGN_ANY);
	CDockablePane* pTabbedBar1 = NULL;
	m_ObjAttrPane.AttachToTabWnd(&m_EditorPane,DM_SHOW,TRUE,&pTabbedBar1);

	//create setting pane
	if (!m_SettingPane.Create(_T("Setting Panel"),this,CRect(0, 0, 300, 300),TRUE,\
		ID_SETTING_PANE,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_FLOAT_MULTI))
		return -1;
	m_SettingPane.EnableDocking(CBRS_ALIGN_ANY);
	CDockablePane* pTabbedBar2 = NULL;
	m_SettingPane.AttachToTabWnd(&m_EditorPane,DM_SHOW,TRUE,&pTabbedBar2);

	if(!m_LogViewerPane.Create(_T("Log Viewer"),this,CRect(0,0,200,200),TRUE,\
		ID_LOGVIEWER_PANE,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_FLOAT_MULTI|CBRS_BOTTOM))
		return -1;
	m_LogViewerPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_LogViewerPane);
	
	if (!m_FindResultPane.Create(_T("Find Result"),this,CRect(0, 0, 300, 300),TRUE,\
		ID_SETTING_PANE,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_FLOAT_MULTI))
		return -1;
	m_FindResultPane.EnableDocking(CBRS_ALIGN_ANY);
	CDockablePane* pTabbedBar3 = NULL;
	m_FindResultPane.AttachToTabWnd(&m_LogViewerPane,DM_SHOW,TRUE,&pTabbedBar3);

	m_hAccel=::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));

	PostMessage(UM_RENDERINIT);

	m_EditorPane.EnableAllCtrls(FALSE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	
	cs.style &= ~FWS_ADDTOTITLE;
	cs.style &= ~FWS_PREFIXTITLE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

void CMainFrame::OnModuleModelViewer()
{
	OpenModelViewer();
}

void CMainFrame::OnUpdateModuleModelViewer(CCmdUI* pCmdUI)
{
	char path[MAX_PATH];
	GetModuleFileName(NULL,path,MAX_PATH);
	int pos=strlen(path);
	while(path[--pos]!='\\');
	path[pos]='\0';
	strcat_s(path,"\\ModelViewer.dll");

	if(_access((LPCTSTR)path,0)!=-1) pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}

void CMainFrame::OnClose()
{
	if(!WorldMapManager::GetInstance()->CloseMap()) return ;
	WorldEditorRenderInterface::GetInstance()->Release();

	CWnd *pWnd=CWnd::FindWindow("SGP_ModelViewerClass",NULL);
	if(pWnd!=NULL)
	{
		CString strInfo="SGP_ModelViewer module is running!\n\nPlease close SGP_ModelViewer first!";
		MessageBox(strInfo,"WorldEditor",MB_OK|MB_ICONINFORMATION);
		pWnd->BringWindowToTop();
		return ;
	}

	CFrameWndEx::OnClose();
}

LRESULT CMainFrame::InitRender(WPARAM wParam,LPARAM lParam)
{
	CView* pView=GetActiveView();
	if(pView!=NULL)
	{
		// Initialize render interface
		if(!WorldEditorRenderInterface::GetInstance()->Init(pView->m_hWnd,&(m_LogViewerPane.m_RichEdit)))
		{
			MessageBox(_T("Render initialize failed!"));
		}
		MouseMsgInterface::GetInstance()->SetRenderWindow(pView->m_hWnd);
	}

	return 0;
}

void CMainFrame::OnMenuView(UINT nID)
{
	switch(nID)
	{
	case IDM_VIEW_SCENE_EXPLORER:
		m_SceneExplorerPane.ShowPane(!m_SceneExplorerPane.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_EDITOR_PANEL:
		m_EditorPane.ShowPane(!m_EditorPane.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_ATTRIBUTE_PANE:
		m_ObjAttrPane.ShowPane(!m_ObjAttrPane.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_SETTING_PANE:
		m_SettingPane.ShowPane(!m_SettingPane.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_LOGVIEWER:
		m_LogViewerPane.ShowPane(!m_LogViewerPane.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_FIND_RESULT:
		m_FindResultPane.ShowPane(!m_FindResultPane.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_STANDARD_TOOLBAR:
		m_wndToolBar.ShowPane(!m_wndToolBar.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_OBJECT_TOOLBAR:
		m_SceneToolBar.ShowPane(!m_SceneToolBar.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_STATUSBAR:
		m_StatusBar.ShowPane(!m_StatusBar.IsVisible(),FALSE,FALSE);
		break;
	}
}

void CMainFrame::OnUpdateMenuView(CCmdUI *pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case IDM_VIEW_SCENE_EXPLORER:
		pCmdUI->SetCheck(m_SceneExplorerPane.IsVisible()||m_SceneExplorerPane.IsAutoHideMode());
		break;
	case IDM_VIEW_EDITOR_PANEL:
		pCmdUI->SetCheck(m_EditorPane.IsVisible()||m_EditorPane.IsAutoHideMode());
		break;
	case IDM_VIEW_ATTRIBUTE_PANE:
		pCmdUI->SetCheck(m_ObjAttrPane.IsVisible()||m_ObjAttrPane.IsAutoHideMode());
		break;
	case IDM_VIEW_SETTING_PANE:
		pCmdUI->SetCheck(m_SettingPane.IsVisible()||m_SettingPane.IsAutoHideMode());
		break;
	case IDM_VIEW_LOGVIEWER:
		pCmdUI->SetCheck(m_LogViewerPane.IsVisible()||m_LogViewerPane.IsAutoHideMode());
		break;
	case IDM_VIEW_FIND_RESULT:
		pCmdUI->SetCheck(m_FindResultPane.IsVisible()||m_FindResultPane.IsAutoHideMode());
		break;
	case IDM_VIEW_STANDARD_TOOLBAR:
		pCmdUI->SetCheck(m_wndToolBar.IsVisible());
		break;
	case IDM_VIEW_OBJECT_TOOLBAR:
		pCmdUI->SetCheck(m_SceneToolBar.IsVisible());
	case IDM_VIEW_STATUSBAR:
		pCmdUI->SetCheck(m_StatusBar.IsVisible());
		break;
	}
}

void CMainFrame::OnFileNew()
{
	WorldMapManager::GetInstance()->CreateMap();
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT nID=LOWORD(wParam);
	switch(nID)
	{
	case IDM_EDIT_UNDO:
		OperationManager::GetInstance()->Undo();
		break;
	case IDM_EDIT_REDO:
		OperationManager::GetInstance()->Redo();
		break;
	}

	return CFrameWndEx::OnCommand(wParam, lParam);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(::TranslateAccelerator(GetSafeHwnd(),m_hAccel,pMsg)) return TRUE;

	return CFrameWndEx::PreTranslateMessage(pMsg);
}

void CMainFrame::OnUpdateEdit(CCmdUI *pCmdUI)
{
	OperationManager* opManager=OperationManager::GetInstance();
	switch(pCmdUI->m_nID)
	{
	case IDM_EDIT_UNDO:
		{
			if(opManager->HasUndo()) pCmdUI->Enable(TRUE);
			else pCmdUI->Enable(FALSE);
		}
		break;
	case IDM_EDIT_REDO:
		{
			if(opManager->HasRedo()) pCmdUI->Enable(TRUE);
			else pCmdUI->Enable(FALSE);
		}
		break;
	}
}

BOOL CMainFrame::OnShowPopupMenu(CMFCPopupMenu* pMenuPopup)
{
	if(pMenuPopup!=NULL&&pMenuPopup->GetMenuBar()->CommandToIndex(IDM_FILE_SAVE_AS)>=0)
	{
		int index=pMenuPopup->GetMenuBar()->CommandToIndex(IDM_FILE_SAVE_AS);
		CMenu menu;
		menu.CreatePopupMenu();

		m_RecentFilesVector.clear();
		WorldEditorConfig::GetInstance()->GetRecentFiles(m_RecentFilesVector);
		int size=m_RecentFilesVector.size();
		if(size>0)
		{
			for(int i=0;i<size;++i)
			{
				CString itemText;
				itemText.Format("%d   %s",i+1,WorldEditorConfig::GetInstance()->GetDataFileDir()+"\\"+m_RecentFilesVector[i]);
				menu.AppendMenu(MF_STRING,RECENT_FILES_START+i,itemText);
			}
		}
		else
			menu.AppendMenu(MF_STRING,RECENT_FILES_START,"None");

		pMenuPopup->InsertItem(CMFCToolBarMenuButton(-1,menu.m_hMenu,-1,"Recent Files"),index+2);
	}

	return CFrameWndEx::OnShowPopupMenu(pMenuPopup);
}


void CMainFrame::OnUpdateRecentFiles(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnRecentFiles(UINT nID)
{
	uint32 index=nID - RECENT_FILES_START;
	if(index<m_RecentFilesVector.size())
	{
		CString filePath=WorldEditorConfig::GetInstance()->GetDataFileDir()+"\\"+m_RecentFilesVector[index];
		if(!WorldMapManager::GetInstance()->CheckExist(filePath))
		{
			WorldEditorConfig::GetInstance()->DeleteRecentFiles(m_RecentFilesVector[index]);
			return ;
		}
		WorldMapManager::GetInstance()->LoadMap(filePath);
	}
}

void CMainFrame::ReleaseAllStaticInstance()
{
	WorldEditorRenderInterface::ReleaseInstance();
	WorldEditorConfig::ReleaseInstance();
	OperationManager::ReleaseInstance();
	TerrainHeightHandler::ReleaseInstance();
	TerrainTextureHandler::ReleaseInstance();
	TerrainGrassHandler::ReleaseInstance();
	TerrainWaterHandler::ReleaseInstance();
	AddObjectHandler::ReleaseInstance();
	AddLightObjHandler::ReleaseInstance();
	CloneObjHandler::ReleaseInstance();
	SelectObjHandler::ReleaseInstance();
	MouseMsgInterface::ReleaseInstance();
	WorldMapManager::ReleaseInstance();
	DragMoveHandler::ReleaseInstance();
	DragRotateHandler::ReleaseInstance();
	DragScaleHandler::ReleaseInstance();
	SceneObjectManager::ReleaseInstance();
}

void CMainFrame::OnFileOpen()
{
	TCHAR strFilter[] = _T("MAP Files (*.map)|*.map||");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strFilter,this);
	dlg.m_ofn.lpstrInitialDir = (char*)(LPCTSTR)WorldEditorConfig::GetInstance()->GetDataFileDir();
	if(dlg.DoModal()==IDOK)
	{
		WorldMapManager::GetInstance()->LoadMap(dlg.GetPathName());
	}
}

void CMainFrame::OnUpdateFileOpen(CCmdUI *pCmdUI)
{

}

void CMainFrame::OnFileSave()
{
	WorldMapManager::GetInstance()->SaveMap();
}

void CMainFrame::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	if(/*OperationManager::GetInstance()->NeedSave()*/WorldMapManager::GetInstance()->m_bHaveMap) pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}

void CMainFrame::OnFileSaveAs()
{
	TCHAR strFilter[] = _T("MAP Files (*.map)|*.map||");
	CFileDialog dlg(FALSE,".map",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strFilter,this);
	if(dlg.DoModal()==IDOK)
	{
		WorldMapManager::GetInstance()->SaveMapAs(dlg.GetPathName());
	}
}

void CMainFrame::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	if(WorldMapManager::GetInstance()->m_bHaveMap) pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}

void CMainFrame::OnFileClose()
{
	WorldMapManager::GetInstance()->CloseMap();
}

void CMainFrame::OnUpdateFileClose(CCmdUI *pCmdUI)
{
	if(WorldMapManager::GetInstance()->m_bHaveMap) pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}

void CMainFrame::OnToolsBuildLightmap()
{
	WorldMapManager::GetInstance()->BuildLightMap();
}

void CMainFrame::OnUpdateToolsBuildLightmap(CCmdUI *pCmdUI)
{
	if(WorldMapManager::GetInstance()->m_bHaveMap) pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}

void CMainFrame::OpenModelViewer(CString modelPath)
{
	typedef void (*Show)(CString);
	CWnd *pWnd=CWnd::FindWindow("SGP_ModelViewerClass",NULL);
	if(pWnd!=NULL)
	{
		if(!modelPath.IsEmpty())
		{
			Show pAddr=(Show)GetProcAddress(theApp.m_hDllModelViewer,"OpenModel");
			if(pAddr!=NULL) pAddr(modelPath);
		}
		pWnd->BringWindowToTop();

		((CMainFrame*)AfxGetMainWnd())->ShowWindow(SW_MINIMIZE);
		return ;
	}

	if(theApp.m_hDllModelViewer==NULL)
	{
		char path[MAX_PATH];
		GetModuleFileName(NULL,path,MAX_PATH);
		int pos=strlen(path);
		while(path[--pos]!='\\');
		path[pos]='\0';
		strcat_s(path,"\\ModelViewer.dll");
		theApp.m_hDllModelViewer=LoadLibrary(path);
	}

	Show pAddr=(Show)GetProcAddress(theApp.m_hDllModelViewer,"CreateModelViewer");
	if(pAddr!=NULL) pAddr(modelPath);
}

void CMainFrame::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	
}

void CMainFrame::OnMenuSceneMove()
{
	if(SelectObjHandler::GetInstance()->GetDragMode()!=Drag_Move)
		SelectObjHandler::GetInstance()->SetDragMode(Drag_Move);
	else
		SelectObjHandler::GetInstance()->SetDragMode(Drag_None);
}

void CMainFrame::OnUpdateMenuSceneMove(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(MouseMsgInterface::GetInstance()->IsSelectObjHandler());
	pCmdUI->SetCheck(SelectObjHandler::GetInstance()->GetDragMode()==Drag_Move);
}

void CMainFrame::OnMenuSceneRotate()
{
	if(SelectObjHandler::GetInstance()->GetDragMode()!=Drag_Rotate)
		SelectObjHandler::GetInstance()->SetDragMode(Drag_Rotate);
	else
		SelectObjHandler::GetInstance()->SetDragMode(Drag_None);
}

void CMainFrame::OnUpdateMenuSceneRotate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(MouseMsgInterface::GetInstance()->IsSelectObjHandler());
	pCmdUI->SetCheck(SelectObjHandler::GetInstance()->GetDragMode()==Drag_Rotate);
}

void CMainFrame::OnMenuSceneScale()
{
	if(SelectObjHandler::GetInstance()->GetDragMode()!=Drag_Scale)
		SelectObjHandler::GetInstance()->SetDragMode(Drag_Scale);
	else
		SelectObjHandler::GetInstance()->SetDragMode(Drag_None);
}

void CMainFrame::OnUpdateMenuSceneScale(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(MouseMsgInterface::GetInstance()->IsSelectObjHandler());
	pCmdUI->SetCheck(SelectObjHandler::GetInstance()->GetDragMode()==Drag_Scale);
}

void CMainFrame::OnToolsPackResForMultiPlatform()
{
	WorldMapManager::GetInstance()->PackupResourceForMultiPlatform();
}

void CMainFrame::SetMapName(CString mapName)
{
	CString strText;
	strText=mapName+CString(".map - WorldEditor");
	SetWindowText(strText);
}

void CMainFrame::OnViewFullscreen()
{
	if(!m_bFullScreen) EnterFullScreen();
	else ExitFullScreen();
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if(m_bFullScreen)
	{
		lpMMI->ptMaxSize.x=m_FullScreenRect.Width();
		lpMMI->ptMaxSize.y=m_FullScreenRect.Height();
		lpMMI->ptMaxPosition.x=m_FullScreenRect.left;
		lpMMI->ptMaxPosition.y=m_FullScreenRect.top;

		lpMMI->ptMaxTrackSize.x=m_FullScreenRect.Width();
		lpMMI->ptMaxTrackSize.y=m_FullScreenRect.Height();
	}

	CFrameWndEx::OnGetMinMaxInfo(lpMMI);
}

void CMainFrame::ExitFullScreen()
{
	if(m_bFullScreen)
	{
		m_SceneExplorerPane.ExitFullScreen();
		m_LogViewerPane.ExitFullScreen();
		m_FindResultPane.ExitFullScreen();
		m_SettingPane.ExitFullScreen();
		m_EditorPane.ExitFullScreen();
		m_ObjAttrPane.ExitFullScreen();
		m_wndMenuBar.ShowPane(TRUE,FALSE,FALSE);
		m_wndToolBar.ShowPane(TRUE,FALSE,FALSE);
		m_SceneToolBar.ShowPane(TRUE,FALSE,FALSE);
		m_StatusBar.ShowPane(TRUE,FALSE,FALSE);

		m_bFullScreen=FALSE;
		SetWindowPlacement(&m_OldWndPlacement);
	}
}

void CMainFrame::EnterFullScreen()
{
	if(!m_bFullScreen)
	{
		GetWindowPlacement(&m_OldWndPlacement);
		m_OldWndPlacement.ptMaxPosition.x=0;
		m_OldWndPlacement.ptMaxPosition.y=0;
		m_OldWndPlacement.ptMinPosition.x=0;
		m_OldWndPlacement.ptMinPosition.y=0;

		CRect WindowRect;
		GetWindowRect(&WindowRect);
		CRect ClientRect;
		RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery, &ClientRect);
		ClientToScreen(&ClientRect);

		int nFullWidth=GetSystemMetrics(SM_CXSCREEN);
		int nFullHeight=GetSystemMetrics(SM_CYSCREEN);

		m_FullScreenRect.left=WindowRect.left-ClientRect.left-1;
		m_FullScreenRect.top=WindowRect.top-ClientRect.top-1;
		m_FullScreenRect.right=WindowRect.right-ClientRect.right+nFullWidth+2;
		m_FullScreenRect.bottom=WindowRect.bottom-ClientRect.bottom+nFullHeight+2;
		m_bFullScreen=TRUE;

		WINDOWPLACEMENT wndpl;
		wndpl.length=sizeof(WINDOWPLACEMENT);
		wndpl.flags=0;
		wndpl.showCmd=SW_SHOWNORMAL;
		wndpl.rcNormalPosition=m_FullScreenRect;
		SetWindowPlacement(&wndpl);

		m_SceneExplorerPane.EnterFullScreen();
		m_LogViewerPane.EnterFullScreen();
		m_FindResultPane.EnterFullScreen();
		m_SettingPane.EnterFullScreen();
		m_EditorPane.EnterFullScreen();
		m_ObjAttrPane.EnterFullScreen();
		m_wndMenuBar.ShowPane(FALSE,FALSE,FALSE);
		m_wndToolBar.ShowPane(FALSE,FALSE,FALSE);
		m_SceneToolBar.ShowPane(FALSE,FALSE,FALSE);
		m_StatusBar.ShowPane(FALSE,FALSE,FALSE);
	}
}

void CMainFrame::OnUpdateStatusSelectedObjNum(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	CString strText;
	if(MouseMsgInterface::GetInstance()->IsSelectObjHandler())
		strText.Format("%d Objects Selected",SelectObjHandler::GetInstance()->GetSelectedObjCount());
	m_StatusBar.SetPaneText(1,strText);
}

void CMainFrame::OnEditFind()
{
/*	CommonObject obj;
	obj=SelectObjHandler::GetInstance()->GetLastObj();
	Vector4D CamPos;
	WorldEditorRenderInterface::GetInstance()->GetRenderDevice()->getCamreaPosition(&CamPos);
	Vector3D distancePos=Vector3D(CamPos.x,CamPos.y,CamPos.z)-Vector3D(obj.m_pObj->m_fPosition[0],\
		obj.m_pObj->m_fPosition[1],obj.m_pObj->m_fPosition[2]);
	float distance=distancePos.GetLength();
	CString strInfo;
	strInfo.Format("%f",distance);
	MessageBox(strInfo);*/
	
/*	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );

	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	CString strPath=WorldEditorConfig::GetInstance()->GetExeDir()+"\\OpenGLES2\\OGLES2_Win32PCEmulation.exe";
	::CreateProcess("","\"D:\\SGPEngine\\Bin\\Debug\\OpenGLES2\\OGLES2_Win32PCEmulation.exe\" worldmap/water.map",NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);*/
//	WinExec("\"D:\\SGPEngine\\Bin\\Debug\\OpenGLES2\\OGLES2_Win32PCEmulation.exe\" worldmap/water.map",SW_SHOW);
	CString strPath=WorldEditorConfig::GetInstance()->GetExeDir()+"\\OpenGLES2\\OGLES2_Win32PCEmulation.exe";
	CString cmdLine="\""+strPath+"\" "+"worldmap/lightmaptest.map";
	WinExec(cmdLine,SW_SHOW);

	m_FindResultPane.FindObjects();
}
