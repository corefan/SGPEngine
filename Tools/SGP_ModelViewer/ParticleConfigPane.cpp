// ParticleConfigPane.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ParticleConfigPane.h"

// CParticleConfigPane
#include "ModelViewerRenderInterface.h"
#include "MainFrame.h"
#include "ModelDetailPane.h"
#include "HelpFunction.h"
#include "../SGPLibraryCode/SGPHeader.h"

IMPLEMENT_DYNAMIC(CParticleConfigPane, /*CDockablePane*/CCommonDockPane)

CParticleConfigPane::CParticleConfigPane()
{
	m_pConfigDlg=NULL;
}

CParticleConfigPane::~CParticleConfigPane()
{
	if(m_pConfigDlg!=NULL)
	{
		m_pConfigDlg->DestroyWindow();
		delete m_pConfigDlg;
		m_pConfigDlg=NULL;
	}
}

BEGIN_MESSAGE_MAP(CParticleConfigPane, /*CDockablePane*/CCommonDockPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
//	ON_COMMAND(IDB_PARTICLE_REFRESH,OnParticleApply)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CParticleConfigPane 消息处理程序

int CParticleConfigPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (/*CDockablePane*/CCommonDockPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	//create toolbar
/*	m_ToolBar.Create(this,AFX_DEFAULT_TOOLBAR_STYLE,IDT_PARTICLE_TOOLBAR);
	m_ToolBar.LoadToolBar(IDT_PARTICLE_TOOLBAR,0,0,TRUE);
	m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
	m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()&~(CBRS_GRIPPER|\
		CBRS_SIZE_DYNAMIC|CBRS_BORDER_LEFT|CBRS_BORDER_RIGHT));
	m_ToolBar.SetRouteCommandsViaFrame(FALSE);

	// Add update button
	m_ToolBar.ReplaceButton(IDT_PARTICLE_UPDATE,CMFCToolBarButton(IDB_PARTICLE_REFRESH,-1,_T("Apply")));*/

	return 0;
}

void CParticleConfigPane::OnSize(UINT nType, int cx, int cy)
{
	/*CDockablePane*/CCommonDockPane::OnSize(nType, cx, cy);

	SetCtrlPos();
}

void CParticleConfigPane::OnSetFocus(CWnd* pOldWnd)
{	
	if(m_pConfigDlg!=NULL&&m_pConfigDlg->m_hWnd!=NULL)
		m_pConfigDlg->SetFocus();
	/*CDockablePane*/CCommonDockPane::OnSetFocus(pOldWnd);
}

void CParticleConfigPane::SetCtrlPos()
{
	CRect rc;
	GetClientRect(rc);

	int cyTlb=0;
/*	if(m_ToolBar.m_hWnd!=NULL)
	{
		cyTlb=m_ToolBar.CalcFixedLayout(FALSE,TRUE).cy;
		m_ToolBar.SetWindowPos(NULL,rc.left,rc.top,rc.Width(),cyTlb,SWP_NOZORDER|SWP_NOACTIVATE);
	}*/

	if(m_pConfigDlg!=NULL&&m_pConfigDlg->m_hWnd!=NULL)
	{
		m_pConfigDlg->SetWindowPos(NULL,rc.left,rc.top+cyTlb,rc.Width(),rc.Height()-cyTlb,SWP_NOZORDER|SWP_SHOWWINDOW);
	}
}

void CParticleConfigPane::OnParticleApply(bool* bShowFlag)
{
	ASSERT(m_pConfigDlg!=NULL);

	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
	CModelDetailPane& detailPane=pMainFrame->m_ModelDetailPane;
	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	if(pModelMF1!=NULL)
	{
		SGPMF1ParticleTag& particleTag=pModelMF1->m_pParticleEmitter[m_Index];
		UnRegisterTexture(particleTag);
		
		// save setting
		ParticleSystemParam systemParam;
		m_pConfigDlg->m_SystemDlg.SaveToData(systemParam);
		if(systemParam==particleTag.m_SystemParam) ;
		else pMainFrame->SetModifiedFlag();

		int pos=pMainFrame->m_ModelDetailPane.RetrieveParticlePos(m_Index);
		if(pos!=-1)
		{
			ParticleSystemParam& systemParam=particleTag.m_SystemParam;
			if(systemParam.m_groupCount!=0&&systemParam.m_pGroupParam!=NULL)
			{
				delete [] systemParam.m_pGroupParam;
				systemParam.m_pGroupParam=NULL;
			}
		}

	//	m_pConfigDlg->m_pSystemDlg->SaveToData(particleTag.m_SystemParam);
		ParticleSystemCopy(systemParam,particleTag.m_SystemParam);
		RegisterTexture(particleTag);
	
		ModelViewerRenderInterface::GetInstance()->ResetParticleSystemByID(m_Index,particleTag,bShowFlag);
		detailPane.AddParticleIndex(m_Index);
		pos=detailPane.RetrieveParticlePos(m_Index);
		ASSERT(pos!=-1);
		detailPane.m_ParticleGroupVector[pos]=particleTag.m_SystemParam.m_pGroupParam;
		if(systemParam.m_groupCount!=0&&systemParam.m_pGroupParam!=NULL)
			detailPane.m_DetailList.SetParticlePropertyInfo(m_Index);
		else
			detailPane.m_DetailList.SetParticlePropertyInfo(-1);
	}
}

void CParticleConfigPane::OnCloseParticlePane(BOOL bHide)
{
	CheckSaveOperation();// do save operation

	if(bHide) ShowPane(FALSE,FALSE,FALSE);
	if(m_pConfigDlg!=NULL)
	{
		m_pConfigDlg->DestroyWindow();
		delete m_pConfigDlg;
		m_pConfigDlg=NULL;
	}
}

void CParticleConfigPane::LoadFromData(const ParticleSystemParam& data)
{
	m_pConfigDlg=new CParticleConfigDlg;
	m_pConfigDlg->Create(IDD_PARTICLE_CONFIGURATION,this);
	m_pConfigDlg->ShowWindow(SW_SHOW);
	m_pConfigDlg->m_SystemDlg.LoadFromData(data);
	CString strName;
	strName.Format("Particle %d Setting",m_Index+1);
	m_pConfigDlg->m_SystemDlg.SetShowName(strName);
	SetCtrlPos();
}

void CParticleConfigPane::CheckSaveOperation()
{
	ASSERT(m_pConfigDlg!=NULL);
	m_pConfigDlg->m_SystemDlg.PreCloseAction();

	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	if(pModelMF1!=NULL)
	{
		ParticleSystemParam newParam;
		ParticleSystemParam& oldParam=pModelMF1->m_pParticleEmitter[m_Index].m_SystemParam;
		m_pConfigDlg->m_SystemDlg.SaveToData(newParam);
		if(newParam==oldParam) ;
		else// setting changed
		{
			CString strInfo;
			strInfo.Format("Parameters of Particle %d have been changed.\n\nDo you want to save changes to Particle %d ?",m_Index+1,m_Index+1);
			if(MessageBox(strInfo,"Particle Setting",MB_YESNO|MB_ICONQUESTION)==IDYES)
				OnParticleApply();
		}
	}
}

void CParticleConfigPane::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	
}
