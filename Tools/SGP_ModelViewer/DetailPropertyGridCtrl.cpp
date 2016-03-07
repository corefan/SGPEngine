// DetailPropertyGridCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "DetailPropertyGridCtrl.h"

#include "MainFrame.h"
#include "ModelDetailPane.h"
#include "MenuPropertyGridProperty.h"
#include "ModelViewerConfig.h"
// CDetailPropertyGridCtrl

IMPLEMENT_DYNAMIC(CDetailPropertyGridCtrl, CMFCPropertyGridCtrl)

CDetailPropertyGridCtrl::CDetailPropertyGridCtrl()
{
	m_MenuProperty=NULL;
}

CDetailPropertyGridCtrl::~CDetailPropertyGridCtrl()
{
}

BEGIN_MESSAGE_MAP(CDetailPropertyGridCtrl, CMFCPropertyGridCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CDetailPropertyGridCtrl 消息处理程序

BOOL CDetailPropertyGridCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT nID=LOWORD(wParam);
	if(nID==IDM_USE_SELECTED_TEXTURE)
		OnSelectTexture();
	
	if( nID>=PARTICLE_MENU_START && nID<=PARTICLE_MENU_END )
		OnParticleSameTo(nID);

	if(nID==PARTICLE_MENU_CONFIG)
		OnParticleConfig();

	return CMFCPropertyGridCtrl::OnCommand(wParam, lParam);
}

void CDetailPropertyGridCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// this function should be called to avoid bug
//	SetCurSel(NULL);
	CMFCPropertyGridProperty* prevSel=m_pSel;
	SetCurSel(NULL);

	m_nLeftColumnWidth = cx/2;
	AdjustLayout();

	SetCurSel(prevSel);
}

void CDetailPropertyGridCtrl::OnSelectTexture()
{
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
	CString texturePath;
	pMainFrame->GetSelectedResourceTexture(texturePath);

	CMenuPropertyGridProperty* pMenuProperty=(CMenuPropertyGridProperty*)GetCurSel();
	if(pMenuProperty!=NULL)
	{
		pMenuProperty->SetValue(texturePath);
		if(pMenuProperty->GetOriginalValue()!=pMenuProperty->GetValue())
			pMenuProperty->SetModifiedFlag();
		GetOwner()->SendMessage(AFX_WM_PROPERTY_CHANGED,GetDlgCtrlID(),(LPARAM)(pMenuProperty));
	}
}

void CDetailPropertyGridCtrl::OnParticleSameTo(UINT nID)
{
	CMenuPropertyGridProperty* pMenuProperty=(CMenuPropertyGridProperty*)GetCurSel();
	m_MenuProperty=pMenuProperty;
	int index=pMenuProperty->GetCurrIndex();
	((CModelDetailPane*)GetParent())->ParticleConfigureSameTo(index,nID-PARTICLE_MENU_START);
	SetParticlePropertyInfo(nID+1-PARTICLE_MENU_START);
}

void CDetailPropertyGridCtrl::OnParticleConfig()
{
	CMenuPropertyGridProperty* pMenuProperty=(CMenuPropertyGridProperty*)GetCurSel();
	int index=pMenuProperty->GetCurrIndex();
	m_MenuProperty=pMenuProperty;
	((CModelDetailPane*)GetParent())->ParticleConfigure(index);
}

void CDetailPropertyGridCtrl::OnChangeSelection(CMFCPropertyGridProperty* pNewSel,CMFCPropertyGridProperty* pOldSel)
{
	if(pNewSel==NULL) return ;
	
	int nCurrParticle=-1;
	CString sName=pNewSel->GetName();
	if(sName.GetLength()>9&&sName.Left(8)=="Particle")
	{
		CString sNumber=sName.Right(sName.GetLength()-9);
		nCurrParticle=atoi(sNumber);
	}
	else if(pNewSel->GetParent()!=NULL)
	{
		sName=pNewSel->GetParent()->GetName();
		if(sName.GetLength()>9&&sName.Left(8)=="Particle")
		{
			CString sNumber=sName.Right(sName.GetLength()-9);
			nCurrParticle=atoi(sNumber);
		}
	}
	ModelViewerConfig::GetInstance()->m_nCurrSelParticle=nCurrParticle;
}

void CDetailPropertyGridCtrl::SetParticlePropertyInfo(int index)
{
	if(m_MenuProperty!=NULL)
	{
		if(index!=-1)
		{
			CString strInfo;
			strInfo.Format("Particle %d Setting",index+1);
			m_MenuProperty->SetValue(strInfo);
		}
		else
		{
			m_MenuProperty->SetValue("");
		}
	}
}