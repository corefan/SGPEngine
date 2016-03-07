// SettingPane.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "SettingPane.h"

//#include "ModelViewerRenderInterface.h"
#include "ModelViewerConfig.h"
// CSettingPane

IMPLEMENT_DYNAMIC(CSettingPane, /*CDockablePane*/CCommonDockPane)

CSettingPane::CSettingPane()
{

}

CSettingPane::~CSettingPane()
{
}


BEGIN_MESSAGE_MAP(CSettingPane, /*CDockablePane*/CCommonDockPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED,OnSettingChanged)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CSettingPane 消息处理程序
int CSettingPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (/*CDockablePane*/CCommonDockPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_SettingDlg.Create(IDD_SETTING_DLG,this);
	m_SettingDlg.ShowWindow(SW_SHOW);
/*	CRect rectDummy;
	rectDummy.SetRectEmpty();
	m_SettingList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2);

	m_SettingList.EnableHeaderCtrl(FALSE);
	m_SettingList.SetVSDotNetLook();
	m_SettingList.MarkModifiedProperties();
	m_SettingList.SetShowDragContext();
	//m_SettingList.SetAlphabeticMode(TRUE);

	// set font of m_DetailList control
	m_Font.CreatePointFont(100,_T("微软雅黑"));
	m_SettingList.SetFont(&m_Font);

	InitSettingList();*/
/*	CRect rcClient;
	GetClientRect(rcClient);
	m_SettingDlg.MoveWindow(rcClient);*/

	return 0;
}

void CSettingPane::OnSize(UINT nType, int cx, int cy)
{
	/*CDockablePane*/CCommonDockPane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(rect);
	if(m_SettingDlg.m_hWnd!=NULL)
		m_SettingDlg.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
/*	if(m_SettingList.m_hWnd!=NULL)
	{
		m_SettingList.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),\
			rect.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	}*/
}

void CSettingPane::InitSettingList()
{
/*	ModelViewerConfig* pConfig=ModelViewerConfig::GetInstance();

	//background color
	COLORREF col=RGB((BYTE)(pConfig->m_BkgndColor[0]*255),(BYTE)(pConfig->m_BkgndColor[1]*255),\
		(BYTE)(pConfig->m_BkgndColor[2]*255));
	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Background Color"),RGB(210,192,254)col);
	pColorProp->EnableOtherButton(_T("其他..."),FALSE);
	COLORREF defaultCol=RGB((BYTE)(pConfig->m_DefaultBkgndColor[0]*255),\
		(BYTE)(pConfig->m_DefaultBkgndColor[1]*255),(BYTE)(pConfig->m_DefaultBkgndColor[2]*255));
	pColorProp->EnableAutomaticButton(_T("默认"),defaultCol);
	m_SettingList.AddProperty(pColorProp);

	//boundingbox show
	CString strContent;
	if(pConfig->m_bShowBoundingBox) strContent=_T("True");
	else strContent=_T("False");
	CMFCPropertyGridProperty* pBoundingBoxProp=new CMFCPropertyGridProperty(_T("Show BoundingBox"),COleVariant(strContent));
	pBoundingBoxProp->AddOption(_T("True"));
	pBoundingBoxProp->AddOption(_T("False"));
	pBoundingBoxProp->AllowEdit(FALSE);
	m_SettingList.AddProperty(pBoundingBoxProp);*/

}

LRESULT CSettingPane::OnSettingChanged(WPARAM wParam,LPARAM lParam)
{
/*	CMFCPropertyGridProperty* pProp=(CMFCPropertyGridProperty*)lParam;
	ModelViewerConfig* pConfig=ModelViewerConfig::GetInstance();
	CString strAttrName=pProp->GetName();

	if(strAttrName==_T("Background Color"))
	{
		CMFCPropertyGridColorProperty* pColorProp=(CMFCPropertyGridColorProperty*)pProp;
		COLORREF col=pColorProp->GetColor();
		pConfig->m_BkgndColor[0]=(float)(GetRValue(col)/255.0);
		pConfig->m_BkgndColor[1]=(float)(GetGValue(col)/255.0);
		pConfig->m_BkgndColor[2]=(float)(GetBValue(col)/255.0);
		return 0;
	}

	if(strAttrName==_T("Show BoundingBox"))
	{
		const COleVariant& strValue=pProp->GetValue();
		CString str=(CString)strValue;
		if(str==_T("True")) pConfig->m_bShowBoundingBox=TRUE;
		if(str==_T("False")) pConfig->m_bShowBoundingBox=FALSE;
		return 0;
	}*/

	return 0;
}

