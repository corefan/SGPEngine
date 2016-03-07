// MainFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "MainFrame.h"

#include "../SGPLibraryCode/SGPHeader.h"//engine header
#include "ModelViewerRenderInterface.h"
#include "ModelViewerConfig.h"
#include "ResourceTreeCtrl.h"

#include "CommonVisualManager.h"

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
};
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

CMainFrame::CMainFrame()
{
	theApp.m_nAppLook=theApp.GetInt(_T("Application Look"),IDM_VIEW_APPLOOK_OFFICE2003);
	m_bModified=FALSE;
	theApp.m_nAppLook=IDM_VIEW_APPLOOK_OFFICE2007_AQUA;
	theApp.m_nAppLook=IDM_VIEW_APPLOOK_OFFICE2007_BLACK;
//	theApp.m_nAppLook=IDM_VIEW_APPLOOK_OFFICE2007_BLUE;
//	theApp.m_nAppLook=IDM_VIEW_APPLOOK_OFFICE2007_SILVER;
//	theApp.m_nAppLook=IDM_VIEW_APPLOOK_OFFICE2003;
	theApp.m_nAppLook=IDM_VIEW_APPLOOK_VS2005;

	m_nFlashCount=0;
}

CMainFrame::~CMainFrame()
{
}

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(IDM_VIEW_RESOURCE_PANE,IDM_VIEW_STATUSBAR,OnMenuView)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_VIEW_RESOURCE_PANE,IDM_VIEW_STATUSBAR,OnUpdateMenuView)
	ON_COMMAND(IDM_SETTING_BKGNDCOLOR,OnMenuSetBkgndColor)
	ON_COMMAND_RANGE(IDM_VIEW_APPLOOK_XP,IDM_VIEW_APPLOOK_OFFICE2007_AQUA,OnViewApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_VIEW_APPLOOK_XP,IDM_VIEW_APPLOOK_OFFICE2007_AQUA,OnUpdateApplicationLook)
	ON_MESSAGE(UM_RENDERINIT,InitRender)
	ON_MESSAGE(UM_FRAMEUI_INIT,InitFrameUI)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_COMMAND(IDM_FILE_SAVE,OnFileSave)
	ON_COMMAND(IDM_FILE_OPEN,OnFileOpen)
	ON_COMMAND(IDM_FILE_SAVEAS,OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(IDM_FILE_SAVE,OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(IDM_FILE_SAVEAS,OnUpdateFileSaveAs)
	ON_COMMAND(IDM_FILE_CLOSE,OnFileClose)
	ON_UPDATE_COMMAND_UI(IDM_FILE_CLOSE,OnUpdateFileClose)
	ON_WM_TIMER()
END_MESSAGE_MAP()

extern HWND g_Hwnd;

// CMainFrame 消息处理程序
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_ANY);
	ModifyStyle(0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	
	// don't load dock state
//	EnableLoadDockState(FALSE);
//	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
//	OnViewApplicationLook(theApp.m_nAppLook);

	CMFCPopupMenu::SetForceMenuFocus(FALSE);
	//create menu
/*	if (!m_MenuBar.Create(this)) return -1;
	m_MenuBar.SetPaneStyle(m_MenuBar.GetPaneStyle()|CBRS_SIZE_DYNAMIC|CBRS_TOOLTIPS|CBRS_FLYBY);
	m_MenuBar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_MenuBar);*/
	//set font
	m_Font.CreatePointFont(90,_T("微软雅黑"));
	LOGFONT logFont;
	m_Font.GetLogFont(&logFont);
	CMFCMenuBar::SetMenuFont(&logFont);

	//create statusbar
	if(!m_StatusBar.Create(this)) return -1;
	m_StatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT));

	OnViewApplicationLook(theApp.m_nAppLook);
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	CDockingManager::SetDockingMode(DT_SMART);

//	m_View.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
	//create resource pane
	if (!m_ResourcePane.Create(_T("Resource Explorer"),this,CRect(10, 10, 200, 200),TRUE,\
		ID_RESOURCE_PANE,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI))
		return -1;
//	m_ResourcePane.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT|CBRS_ALIGN_BOTTOM);
	m_ResourcePane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_ResourcePane);

	//create model detail pane
	if (!m_ModelDetailPane.Create(_T("Model Details"),this,CRect(0, 0, 220, 220),TRUE,\
		ID_MODELDETAIL_PANE,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_RIGHT|CBRS_FLOAT_MULTI))
		return -1;
//	m_ModelDetailPane.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT|CBRS_ALIGN_BOTTOM);
	m_ModelDetailPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_ModelDetailPane);

	//create setting pane
	if (!m_SettingPane.Create(_T("Setting Panel"),this,CRect(0, 0, 220, 220),TRUE,\
		ID_SETTING_PANE,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_FLOAT_MULTI|CBRS_RIGHT))
		return -1;
//	m_SettingPane.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT|CBRS_ALIGN_BOTTOM);
	m_SettingPane.EnableDocking(CBRS_ALIGN_ANY);
	CDockablePane* pTabbedBar1 = NULL;
	m_SettingPane.AttachToTabWnd(&m_ModelDetailPane,DM_SHOW,TRUE,&pTabbedBar1);

	//create model control pane
	if (!m_ModelControlPane.Create(_T("Animation Contol"),this,CRect(0, 0, 200, 200),TRUE,\
		ID_MODELCONTROL_PANE,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_BOTTOM|CBRS_FLOAT_MULTI))
		return -1;
//	m_ModelControlPane.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT|CBRS_ALIGN_BOTTOM);
	m_ModelControlPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_ModelControlPane);

	//create logviewer pane
	if (!m_LogViewerPane.Create(_T("Log Viewer"),this,CRect(0, 0, 200, 200),TRUE,\
		ID_LOGVIEWER_PANE,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_FLOAT_MULTI))
		return -1;
//	m_LogViewerPane.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT|CBRS_ALIGN_BOTTOM);
	m_LogViewerPane.EnableDocking(CBRS_ALIGN_ANY);
	CDockablePane* pTabbedBar2 = NULL;
	m_LogViewerPane.AttachToTabWnd(&m_ModelControlPane,DM_SHOW,TRUE,&pTabbedBar2);

	// create particle config pane
	m_ParticlePane.Create(_T("Particle Pane"),this,CRect(0,0,200,200),TRUE,\
		ID_PARTICLE_CONFIG_PANE,WS_CHILD|/*WS_VISIBLE|*/WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_FLOAT_MULTI|CBRS_RIGHT);
//	m_ParticlePane.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT|CBRS_ALIGN_BOTTOM);
	m_ParticlePane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_ParticlePane);
	
	g_Hwnd=m_hWnd;

	//post message to initialize render interface
	PostMessage(UM_RENDERINIT);
	// post message to initialize frame ui
	PostMessage(UM_FRAMEUI_INIT);

//	::SetProp(m_hWnd,"SGP_ModelViewer",(HANDLE)this);

	return 0;
}

void CMainFrame::OnMenuView(UINT nID)
{
	switch(nID)
	{
	case IDM_VIEW_RESOURCE_PANE:
		m_ResourcePane.ShowPane(!m_ResourcePane.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_MODELDETAL_PANE:
		m_ModelDetailPane.ShowPane(!m_ModelDetailPane.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_MODELCONTROL_PANE:
		m_ModelControlPane.ShowPane(!m_ModelControlPane.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_LOGVIEWER_PANE:
		m_LogViewerPane.ShowPane(!m_LogViewerPane.IsVisible(),FALSE,FALSE);
		break;
	case IDM_VIEW_SETTING_PANE:
		m_SettingPane.ShowPane(!m_SettingPane.IsVisible(),FALSE,FALSE);
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
	case IDM_VIEW_RESOURCE_PANE:
		pCmdUI->SetCheck(m_ResourcePane.IsVisible());
		break;
	case IDM_VIEW_MODELDETAL_PANE:
		pCmdUI->SetCheck(m_ModelDetailPane.IsVisible());
		break;
	case IDM_VIEW_MODELCONTROL_PANE:
		pCmdUI->SetCheck(m_ModelControlPane.IsVisible());
		break;
	case IDM_VIEW_LOGVIEWER_PANE:
		pCmdUI->SetCheck(m_LogViewerPane.IsVisible());
		break;
	case IDM_VIEW_SETTING_PANE:
		pCmdUI->SetCheck(m_SettingPane.IsVisible());
		break;
	case IDM_VIEW_STATUSBAR:
		pCmdUI->SetCheck(m_StatusBar.IsVisible());
		break;
	}
}

void CMainFrame::OnMenuSetBkgndColor()
{
	ModelViewerConfig *pConfig=ModelViewerConfig::GetInstance();
	CColorDialog dlg(RGB((BYTE)(pConfig->m_BkgndColor[0]*255),\
		(BYTE)(pConfig->m_BkgndColor[1]*255),(BYTE)(pConfig->m_BkgndColor[2]*255)),CC_FULLOPEN,this);
	if(IDOK==dlg.DoModal())
	{
		COLORREF col=dlg.GetColor();
		pConfig->m_BkgndColor[0]=(float)(GetRValue(col)/255.0);
		pConfig->m_BkgndColor[1]=(float)(GetGValue(col)/255.0);
		pConfig->m_BkgndColor[2]=(float)(GetBValue(col)/255.0);
	}
}

void CMainFrame::OnViewApplicationLook(UINT nID)
{
	CWaitCursor waitCursor;
	theApp.m_nAppLook=nID;
	switch(theApp.m_nAppLook)
	{
	case IDM_VIEW_APPLOOK_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;
	case IDM_VIEW_APPLOOK_OFFICE2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		break;
	case IDM_VIEW_APPLOOK_VS2005:
	//	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CCommonVisualManager));
		break;
	case IDM_VIEW_APPLOOK_VS2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		break;
	case IDM_VIEW_APPLOOK_OFFICE2007_AQUA:
		CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		break;
	case IDM_VIEW_APPLOOK_OFFICE2007_BLACK:
		CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		break;
	case IDM_VIEW_APPLOOK_OFFICE2007_BLUE:
		CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		break;
	case IDM_VIEW_APPLOOK_OFFICE2007_SILVER:
		CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		break;
	default:
		break;
	}
	CDockingManager::SetDockingMode(DT_SMART);

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("Application Look"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_nAppLook==pCmdUI->m_nID);
	switch(pCmdUI->m_nID)
	{
	case IDM_VIEW_APPLOOK_OFFICE2007_AQUA:
	case IDM_VIEW_APPLOOK_OFFICE2007_BLACK:
	case IDM_VIEW_APPLOOK_OFFICE2007_BLUE:
	case IDM_VIEW_APPLOOK_OFFICE2007_SILVER:
		pCmdUI->Enable(FALSE);
		break;
	default:
		break;
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CFrameWndEx::PreCreateWindow(cs)) return FALSE;
	
	cs.style &= ~FWS_ADDTOTITLE;
	cs.style &= ~FWS_PREFIXTITLE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	WNDCLASS wndclass;
	::GetClassInfo(AfxGetInstanceHandle(),cs.lpszClass,&wndclass);
	wndclass.lpszClassName="SGP_ModelViewerClass";
	wndclass.hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	VERIFY(AfxRegisterClass(&wndclass));
	cs.lpszClass=wndclass.lpszClassName;

	return TRUE;
}

LRESULT CMainFrame::InitRender(WPARAM wParam,LPARAM lParam)
{
//	m_ParticlePane.ShowPane(FALSE,FALSE,FALSE);

	CView* pView=GetActiveView();
	if(pView!=NULL)
	{
		// Initialize render interface
		if(!ModelViewerRenderInterface::GetInstance()->Init(pView->m_hWnd,&(m_LogViewerPane.m_RichEdit)))
			MessageBox(_T("Render initialize failed!"));
		CString strPath=ModelViewerConfig::GetInstance()->m_InitModelPath;
		if(!strPath.IsEmpty())
		{
			CString strName=strPath.Right(strPath.GetLength()-strPath.ReverseFind('\\')-1);
			CResourceTreeCtrl::OpenMF1File(strPath,strName);
			ModelViewerConfig::GetInstance()->m_InitModelPath.Empty();
		}
	}

	BringWindowToTop();

	return 0;
}

void CMainFrame::OnDestroy()
{
	CFrameWndEx::OnDestroy();

	//release renderinterface instance
	ModelViewerRenderInterface::GetInstance()->Release();
	ModelViewerRenderInterface::ReleaseInstance();
	ModelViewerConfig::ReleaseInstance();
}

BOOL CMainFrame::GetSelectedResourceTexture(CString& texturePath)
{
	CResourceTabCtrl& tabCtrl=m_ResourcePane.m_TabCtrl;
	CResourceTreeCtrl *pTreeCtrl=tabCtrl.m_TreeVector[tabCtrl.GetActiveTab()];
	
	CString strWorkDir=tabCtrl.m_TreeVector[0]->m_DirPath;
	strWorkDir=strWorkDir.Left(strWorkDir.GetLength()-1);

	HTREEITEM hItem=NULL;
	if((hItem=pTreeCtrl->GetSelectedItem())==NULL)
	{
		return FALSE;
	}

	CString strItem=pTreeCtrl->GetItemText(hItem);
	int index=-1;
	if((index=strItem.ReverseFind('.'))==-1)
	{
		return FALSE;
	}

	CString strExt=strItem.Right(strItem.GetLength()-index);
	strExt.MakeLower();
	if(strExt!=_T(".tga")&&strExt!=_T(".dds"))
	{
		return FALSE;
	}

	CString strFilePath=pTreeCtrl->GetItemPath(pTreeCtrl->GetSelectedItem());

	char relativePath[MAX_PATH];
	if(!PathRelativePathTo(relativePath,(LPCTSTR)strWorkDir,FILE_ATTRIBUTE_DIRECTORY,(LPCTSTR)strFilePath,0))
		return FALSE;
	texturePath=relativePath;
	if(texturePath.Left(2)==_T(".\\"))
	{
		texturePath=texturePath.Right(texturePath.GetLength()-2);
	}

	return TRUE;
}

LRESULT CMainFrame::InitFrameUI(WPARAM wParam,LPARAM lParam)
{
	m_ParticlePane.ShowPane(FALSE,FALSE,FALSE);

	if(::GetForegroundWindow()!=m_hWnd) SetTimer(1,800,NULL);

	CString strPath = ModelViewerConfig::GetInstance()->m_InitModelPath;
	if(!strPath.IsEmpty())
	{
		CString strName = strPath.Right(strPath.GetLength()-strPath.ReverseFind('\\')-1);
		CResourceTreeCtrl::OpenMF1File(strPath,strName);
	}

	return 0;
}

void CMainFrame::OnClose()
{
	if(m_ParticlePane.IsVisible())
		m_ParticlePane.OnCloseParticlePane();

	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	if(pModelMF1!=NULL&&(m_bModified||m_ModelDetailPane.m_ConfigDlg.IsConfigSettingChanged()))
	{
		CString strInfo;
		strInfo.Format("Model \"%s\" has been changed.\n\nDo you want to save changes to %s ?",LPCTSTR(m_MF1FileName),LPCTSTR(m_MF1FileName));
		int nResult=MessageBox(strInfo,"ModelViewer",MB_YESNOCANCEL|MB_ICONQUESTION);
		switch (nResult)
		{
		case IDYES:
			// do save operation
			SaveMF1File(m_MF1Path,m_MF1FileName);
			break;
		case IDNO:
			// do nothing
			break;
		case IDCANCEL:
			return ;
		default:
			break;
		}
	}

//	::RemoveProp(m_hWnd,"SGP_ModelViewer");

	CFrameWndEx::OnClose();
}

BOOL CMainFrame::OnCloseDockingPane(CDockablePane* pPane)
{
	if(pPane->IsKindOf(RUNTIME_CLASS(CTabbedPane)))// Multi window
	{
		CTabbedPane* tabpane=(CTabbedPane*)pPane;
		CMFCBaseTabCtrl* pTabWnd = tabpane->GetUnderlyingWindow();
		int num = pTabWnd->GetActiveTab();
		CDockablePane* pPane = DYNAMIC_DOWNCAST(CDockablePane, pTabWnd->GetTabWnd(num));
		if(pPane==&m_ParticlePane) m_ParticlePane.OnCloseParticlePane(FALSE);
	}
	else// Single Window
	{
		if(pPane==&m_ParticlePane) m_ParticlePane.OnCloseParticlePane(FALSE);
	}

	return TRUE;
}

BOOL CMainFrame::OnCloseMiniFrame(CPaneFrameWnd* pWnd)
{
	CWnd* pwnd=pWnd->GetPane();
	if(pwnd==NULL) return TRUE;

	if(pwnd->IsKindOf(RUNTIME_CLASS(CTabbedPane)))
	{
		CTabbedPane* tabpane=(CTabbedPane*)pwnd;
		CMFCBaseTabCtrl* pTabWnd=tabpane->GetUnderlyingWindow();
		int num=pTabWnd->GetActiveTab();
		CDockablePane* pPane = DYNAMIC_DOWNCAST(CDockablePane, pTabWnd->GetTabWnd(num));
		if(pPane==&m_ParticlePane) m_ParticlePane.OnCloseParticlePane(FALSE);
	}
	else
	{
		CDockablePane* pPane=(CDockablePane*)pwnd;
		if(pPane==&m_ParticlePane) m_ParticlePane.OnCloseParticlePane(FALSE);
	}

	return TRUE;
}

void CMainFrame::OnFileSave()
{
	// save change to mf1
	PreSaveMF1File();
	SaveMF1File(m_MF1Path,m_MF1FileName);
}

void CMainFrame::OnUpdateFileSave(CCmdUI *pCmdUI)
{
/*	if(ModelViewerRenderInterface::GetInstance()->GetModelMF1()!=NULL&&\
		(m_bModified||m_ModelDetailPane.m_ConfigDlg.IsConfigSettingChanged()))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);*/
}

void CMainFrame::OnFileOpen()
{
	char szFilter[]="MF1 Files (*.mf1)|*.mf1||";
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,\
		szFilter,this);
	if(dlg.DoModal()==IDOK)
	{
		if(dlg.GetFileExt().MakeUpper()=="MF1")
		{
			CResourceTreeCtrl::OpenMF1File(dlg.GetPathName(),dlg.GetFileName());
		}
	}
}

void CMainFrame::OnFileSaveAs()
{
	PreSaveMF1File();
	
	char szFilter[]="MF1 Files (*.mf1)|*.mf1||";
	CFileDialog dlg(FALSE,"mf1",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter,this);
	if(dlg.DoModal()==IDOK)
	{
		
		SaveMF1File(dlg.GetPathName(),dlg.GetFileName());
	}
}

void CMainFrame::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	if(ModelViewerRenderInterface::GetInstance()->GetModelMF1()!=NULL)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::SaveMF1File(CString strPath,CString strName)
{
//	CWaitCursor waitCursor;

	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	if(pModelMF1!=NULL)
	{
		WCHAR wPath[MAX_PATH];
		wmemset(wPath,0,MAX_PATH);
		MultiByteToWideChar(CP_ACP,0,(LPCTSTR)strPath,strPath.GetLength(),wPath,MAX_PATH);
		
		m_ModelDetailPane.m_ConfigDlg.SaveConfigSetting();// prev save
		pModelMF1->SaveMF1(String(""),String(wPath));
		m_ModelDetailPane.m_ConfigDlg.AfterSaveConfigSetting();// after save

		CString strLog;
		strLog.Format("Save %s succeed",strName);
		ModelViewerRenderInterface::GetInstance()->ShowInfoLog(strLog);
	}
	ClearModifiedFlag();
}

void CMainFrame::PreSaveMF1File()
{
	if(m_ParticlePane.IsVisible())
		m_ParticlePane.CheckSaveOperation();
}

void CMainFrame::OnFileClose()
{
	// check whether changed
	PreCloseModel();

	ModelViewerRenderInterface::GetInstance()->CloseModel();
	// update control state
	m_ModelDetailPane.ModelClosed();
	m_ModelControlPane.m_ModelControlDlg.ModelClosed();

	SetWindowText("SGP_ModelViewer");
}

void CMainFrame::OnUpdateFileClose(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(ModelViewerRenderInterface::GetInstance()->GetModelMF1()!=NULL);
}

void CMainFrame::PreCloseModel()
{
	if(m_ParticlePane.IsVisible())
		m_ParticlePane.OnCloseParticlePane();

	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	if(pModelMF1==NULL) return ;
	//	if(m_ContentChanged)
	if(m_bModified||m_ModelDetailPane.m_ConfigDlg.IsConfigSettingChanged())
	{
		CString strInfo;
		strInfo.Format("Model \"%s\" has been changed.\n\nDo you want to save changes to %s ?",LPCTSTR(m_MF1FileName),LPCTSTR(m_MF1FileName));
		int nResult=MessageBox(strInfo,"ModelViewer",MB_YESNO|MB_ICONQUESTION);
		switch (nResult)
		{
		case IDYES:
			// do save operation
			SaveMF1File(m_MF1Path,m_MF1FileName);
			break;
		case IDNO:
			// do nothing
			break;
		}
	}
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if(m_nFlashCount++ < 5) FlashWindow(TRUE);
	else KillTimer(1);

	CFrameWndEx::OnTimer(nIDEvent);
}
