#include "..\stdafx.h"
#include "..\WorldEditor.h"
#include "SettingPane.h"
#include "SettingCheckProperty.h"

// CSettingPane

IMPLEMENT_DYNAMIC(CSettingPane, CCommonDockablePane)

CSettingPane* CSettingPane::s_pSettingPane=NULL;

CString ShowOptionArray[]=
{
	"Cell",
	"Chunk",
	"Wireframe",
	"Terrain",
	"Water",
	"Objects",
	"Lights",
	"Grass",
	"Skydome"
};

BOOL DefaultShowFlag[]=
{
	FALSE,
	FALSE,
	FALSE,
	TRUE,
	TRUE,
	TRUE,
	TRUE,
	TRUE,
	TRUE
};

CSettingPane::CSettingPane()
{

}

CSettingPane::~CSettingPane()
{
}

BEGIN_MESSAGE_MAP(CSettingPane, CCommonDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED,OnSettingChanged)
END_MESSAGE_MAP()

// CSettingPane 消息处理程序

int CSettingPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCommonDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	s_pSettingPane=this;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	m_SettingList.Create(WS_VISIBLE|WS_CHILD,rectDummy,this,(UINT)-1);
	m_SettingList.SetCustomColors(RGB(255,255,255),RGB(0,0,0),RGB(141,178,230),RGB(0,0,0),RGB(225,238,255),RGB(0,0,0),RGB(141,178,230));
	m_SettingList.SetVSDotNetLook();
	m_SettingList.MarkModifiedProperties();
	m_SettingList.EnableDescriptionArea();
	m_SettingList.SetDescriptionRows(3);
	m_SettingList.EnableHeaderCtrl(FALSE);

	m_Font.CreatePointFont(93,_T("微软雅黑"));
	m_SettingList.SetFont(&m_Font);
	InitSettingList();

	return 0;
}

void CSettingPane::OnSize(UINT nType, int cx, int cy)
{
	CCommonDockablePane::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(rect);
	if(m_SettingList.m_hWnd!=NULL)
		m_SettingList.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
}

void CSettingPane::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	
}

void CSettingPane::InitSettingList()
{
	// Show Option
	CMFCPropertyGridProperty* pShowOption = new CMFCPropertyGridProperty("Show Option");
	for(uint32 i=0;i<sizeof(ShowOptionArray)/sizeof(CString);++i)
	{
		CSettingCheckProperty* pShowProperty=new CSettingCheckProperty(ShowOptionArray[i],DefaultShowFlag[i],i);
		pShowProperty->CheckAction();
		pShowOption->AddSubItem(pShowProperty);
	}
	m_SettingList.AddProperty(pShowOption);
	// Camera Setting
	CMFCPropertyGridProperty* pCameraSetting =new CMFCPropertyGridProperty("Camera Setting");
	m_pNearPlaneProperty = new CMFCPropertyGridProperty("Near Plane",COleVariant(1.0f));
	pCameraSetting->AddSubItem(m_pNearPlaneProperty);
	m_pFarPlaneProperty = new CMFCPropertyGridProperty("Far Plane",COleVariant(1000.0f));
	pCameraSetting->AddSubItem(m_pFarPlaneProperty);
	m_pFovProperty = new CMFCPropertyGridProperty("Fov",COleVariant(45.0f));
	pCameraSetting->AddSubItem(m_pFovProperty);
	m_SettingList.AddProperty(pCameraSetting);
}

void CSettingPane::SetShowOption(int index,BOOL bShow)
{
	WorldEditorConfig* pConfig = WorldEditorConfig::GetInstance();
	switch(index)
	{
	case 0:
		pConfig->m_bShowChunkGrid = bShow;
		break;
	case 1:
		pConfig->m_bShowChunk = bShow;
		break;
	case 2:
		pConfig->m_bShowWireframe = bShow;
		break;
	case 3:
		if(bShow) CSGPWorldConfig::getInstance()->m_bShowTerrain=true;
		else CSGPWorldConfig::getInstance()->m_bShowTerrain=false;
		break;
	case 4:
		pConfig->m_bShowWater = bShow;
		if(bShow) CSGPWorldConfig::getInstance()->m_bShowWater=true;
		else CSGPWorldConfig::getInstance()->m_bShowWater=false;
		CTerrainWaterDlg::GetInstance()->EnableAllCtrls(pConfig->m_bShowWater\
			&&WorldMapManager::GetInstance()->m_bHaveMap);
		break;
	case 5:
		pConfig->m_bShowObjects = bShow;
		if(bShow) CSGPWorldConfig::getInstance()->m_bShowStaticObject=true;
		else CSGPWorldConfig::getInstance()->m_bShowStaticObject=false;
		CSceneObjectDlg::GetInstance()->EnableObjectCtrls(pConfig->m_bShowObjects\
			&&WorldMapManager::GetInstance()->m_bHaveMap);
		SelectObjHandler::GetInstance()->ClearSelection();
		break;
	case 6:
		pConfig->m_bShowLights = bShow;
		CSceneObjectDlg::GetInstance()->EnableLightCtrls(pConfig->m_bShowLights\
			&&WorldMapManager::GetInstance()->m_bHaveMap);
		SelectObjHandler::GetInstance()->ClearSelection();
		break;
	case 7:
		if(bShow) CSGPWorldConfig::getInstance()->m_bShowGrass=true;
		else CSGPWorldConfig::getInstance()->m_bShowGrass=false;
		break;
	case 8:
		if(bShow) CSGPWorldConfig::getInstance()->m_bShowSkyDome=true;
		else CSGPWorldConfig::getInstance()->m_bShowSkyDome=false;
		break;
	}
}

LRESULT CSettingPane::OnSettingChanged(WPARAM wParam,LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp=(CMFCPropertyGridProperty*)lParam;

	if(pProp==m_pNearPlaneProperty)
		WorldEditorRenderInterface::GetInstance()->SetCameraNearPlane(m_pNearPlaneProperty->GetValue().fltVal);
	else if(pProp==m_pFarPlaneProperty)
		WorldEditorRenderInterface::GetInstance()->SetCameraFarPlane(m_pFarPlaneProperty->GetValue().fltVal);
	else if(pProp==m_pFovProperty)
		WorldEditorRenderInterface::GetInstance()->SetCameraFov(m_pFovProperty->GetValue().fltVal);

	return 0;
}