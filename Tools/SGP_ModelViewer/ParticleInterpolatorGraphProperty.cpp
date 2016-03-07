#include "stdafx.h"
#include "Resource.h"

#include "ParticleInterpolatorGraphDlg.h"
#include "ParticleInterpolatorGraphProperty.h"
#include "ParticleModelParamGridProperty.h"

IMPLEMENT_DYNAMIC(CParticleInterpolatorGraphProperty,CMFCPropertyGridProperty)

CParticleInterpolatorGraphProperty::CParticleInterpolatorGraphProperty(const CString& strName,const COleVariant& varValue)\
	:CMFCPropertyGridProperty(strName,varValue)
{
	m_bAllowEdit=FALSE;
	m_dwStart=GetTickCount();
	SetValue("Double click to show graph");
	SetDescription("Double click to show graph");
}

CParticleInterpolatorGraphProperty::~CParticleInterpolatorGraphProperty()
{

}

BOOL CParticleInterpolatorGraphProperty::OnDblClk(CPoint point)
{
	CParticleInterpolatorGraphDlg dlg;
	((CParticleModelParamGridProperty*)GetParent())->SaveToData(dlg.m_Graph.m_InterpolatorParam);
	dlg.DoModal();
	return TRUE;
}

BOOL CParticleInterpolatorGraphProperty::OnClickValue(UINT uiMsg,CPoint point)
{
	if(GetTickCount()-m_dwStart<90)
	{
		CParticleInterpolatorGraphDlg dlg;
		((CParticleModelParamGridProperty*)GetParent())->SaveToData(dlg.m_Graph.m_InterpolatorParam);
		dlg.DoModal();
	}
	
	m_dwStart=GetTickCount();
	
	return TRUE;
}