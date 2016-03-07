#include "stdafx.h"

#include "ParticleEntryGridProperty.h"
#include "ParticleCtrlMsgDef.h"

#include "../SGPLibraryCode/SGPHeader.h"

IMPLEMENT_DYNAMIC(CParticleEntryGridProperty,CMFCPropertyGridProperty)

CParticleEntryGridProperty::CParticleEntryGridProperty(const CString strName):\
	CMFCPropertyGridProperty(strName)
{
	m_bRGBA=false;
	m_bAngle=false;
}

void CParticleEntryGridProperty::Init()
{
	m_EntryX=new CMFCPropertyGridProperty("x",COleVariant(0.0f),"Range:[0.0,1.0]");
	if(m_bRGBA)
	{
		m_EntryY0=new CMFCPropertyGridProperty("y0",COleVariant((long)0),"Range:[0,255]");
		m_EntryY1=new CMFCPropertyGridProperty("y1",COleVariant((long)0),"Range:[0,255]");
	}
	else if(m_bAngle)
	{
		m_EntryY0=new CMFCPropertyGridProperty("y0",COleVariant((long)0),"Range:[0,360]");
		m_EntryY1=new CMFCPropertyGridProperty("y1",COleVariant((long)0),"Range:[0,360]");
	}
	else
	{
		m_EntryY0=new CMFCPropertyGridProperty("y0",COleVariant(0.0f));
		m_EntryY1=new CMFCPropertyGridProperty("y1",COleVariant(0.0f));
	}

	AddSubItem(m_EntryX);
	AddSubItem(m_EntryY0);
	AddSubItem(m_EntryY1);
	Expand(FALSE);
}

void CParticleEntryGridProperty::OnRClickName(CPoint C)
{
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING,MENU_DELETE_ENTRY,"Delete");
	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,m_pWndList->GetParent());
}

void CParticleEntryGridProperty::LoadFromData(sgp::ParticleEntryParam& data)
{
	m_EntryX=new CMFCPropertyGridProperty("x",COleVariant(data.m_x));
	if(m_bRGBA)
	{
		m_EntryY0=new CMFCPropertyGridProperty("y0",COleVariant((long)(data.m_y0*255)));
		m_EntryY1=new CMFCPropertyGridProperty("y1",COleVariant((long)(data.m_y1*255)));
	}
	else if(m_bAngle)
	{
		m_EntryY0=new CMFCPropertyGridProperty("y0",COleVariant((long)(data.m_y0*180/float_Pi)));
		m_EntryY1=new CMFCPropertyGridProperty("y1",COleVariant((long)(data.m_y1*180/float_Pi)));
	}
	else
	{
		m_EntryY0=new CMFCPropertyGridProperty("y0",COleVariant(data.m_y0));
		m_EntryY1=new CMFCPropertyGridProperty("y1",COleVariant(data.m_y1));
	}

	AddSubItem(m_EntryX);
	AddSubItem(m_EntryY0);
	AddSubItem(m_EntryY1);
	Expand(FALSE);
}

void CParticleEntryGridProperty::SaveToData(sgp::ParticleEntryParam& data)
{
	data.m_x=m_EntryX->GetValue().fltVal;
//	data.m_y0=m_EntryY0->GetValue().fltVal;
//	data.m_y1=m_EntryY1->GetValue().fltVal;

	if(m_bRGBA)
	{
		data.m_y0=(float)m_EntryY0->GetValue().lVal/255.0f;
		data.m_y1=(float)m_EntryY1->GetValue().lVal/255.0f;
	}
	else if(m_bAngle)
	{
		data.m_y0=(float)m_EntryY0->GetValue().lVal*float_Pi/180.0f;
		data.m_y1=(float)m_EntryY1->GetValue().lVal*float_Pi/180.0f;
	}
	else
	{
		data.m_y0=m_EntryY0->GetValue().fltVal;
		data.m_y1=m_EntryY1->GetValue().fltVal;
	}

	if(data.m_y0>data.m_y1)
	{
		float tmp=data.m_y0;
		data.m_y0=data.m_y1;
		data.m_y1=tmp;
	}
}