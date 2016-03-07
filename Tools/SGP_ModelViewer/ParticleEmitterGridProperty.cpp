#include "stdafx.h"

#include "ParticleEmitterGridProperty.h"
#include "ParticleZoneGridProperty.h"
#include "ParticleTypesGridProperty.h"
#include "ParticleCheckGridProperty.h"
#include "ParticleCtrlMsgDef.h"
#include "../SGPLibraryCode/SGPHeader.h"

IMPLEMENT_DYNAMIC(CParticleEmitterGridProperty,CMFCPropertyGridProperty)

CParticleEmitterGridProperty::CParticleEmitterGridProperty(const CString& strName):\
	CMFCPropertyGridProperty(strName)
{
}

void CParticleEmitterGridProperty::SetEmitterType(CString sType,BOOL bNotify)
{
	if(sType==m_sEmitterType&&bNotify) return ;

	unsigned int i;
	for(i=0;i<m_PropertyVector.size();++i) RemoveSubItem(m_PropertyVector[i]);
	m_PropertyVector.clear();

	if(sType=="Straight Emitter")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction Z",COleVariant(-1.0f)));
	}
	else if(sType=="Static Emitter"){}
	else if(sType=="Spheric Emitter")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction Z",COleVariant(-1.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Min Angle",COleVariant((long)0),"Range: [0,360]"));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Max Angle",COleVariant((long)0),"Range: [0,360]"));
	}
	else if(sType=="Random Emitter"){}
	else if(sType=="Normal Emitter")
	{
		m_PropertyVector.push_back(new CParticleCheckGridProperty("Inverted",false));
	}
	
	m_sEmitterType=sType;

	for(i=0;i<m_PropertyVector.size();++i) AddSubItem(m_PropertyVector[i]);
	if(bNotify) m_pWndList->GetParent()->PostMessage(UM_UPDATE_PROPERTYCTRL);
}

void CParticleEmitterGridProperty::OnRClickName(CPoint C)
{
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING,MENU_DELETE_EMITTER,"Delete");
	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,m_pWndList->GetParent());
}

void CParticleEmitterGridProperty::Init()
{
	m_sEmitterType="Straight Emitter";
	// Zone
	m_ZoneProperty=new CParticleZoneGridProperty("Zone Setting");
	AddSubItem(m_ZoneProperty);
	m_ZoneProperty->Init();
	// Emitter Type
	CParticleTypesGridProperty* pEmitterType=new CParticleTypesGridProperty("Emitter Type",m_sEmitterType);
	AddSubItem(pEmitterType);
	pEmitterType->AddOption("Straight Emitter");
	pEmitterType->AddOption("Static Emitter");
	pEmitterType->AddOption("Spheric Emitter");
	pEmitterType->AddOption("Random Emitter");

	pEmitterType->AddOption("Normal Emitter");
	pEmitterType->AllowEdit(FALSE);
	pEmitterType->SetType(Property_EmitterType);
	pEmitterType->SetDescription("Choose emitter type");
	// Flow
	m_FlowProperty=new CMFCPropertyGridProperty("Flow",COleVariant(1.0f));
	m_FlowProperty->SetDescription("The number of particles per step.");
	AddSubItem(m_FlowProperty);
	// Tank
	m_TankProperty=new CMFCPropertyGridProperty("Tank",COleVariant((long)-1));
	m_TankProperty->SetDescription("The number of particles in this emitter's tank. Negative value means infinite.");
	AddSubItem(m_TankProperty);
	// Force
	m_MinForceProperty=new CMFCPropertyGridProperty("Min Force",COleVariant(1.0f));
	m_MinForceProperty->SetDescription("The minimum force of the emitter.The particle launches at speed: force/mass of the particle.");
	AddSubItem(m_MinForceProperty);
	m_MaxForceProperty=new CMFCPropertyGridProperty("Max Force",COleVariant(1.0f));
	m_MaxForceProperty->SetDescription("The maximum force of the emitter.The particle launches at speed: force/mass of the particle.");
	AddSubItem(m_MaxForceProperty);

	SetEmitterType(m_sEmitterType,FALSE);

	Expand(FALSE);
}

void CParticleEmitterGridProperty::LoadFromData(ParticleEmitterParam& data)
{
	m_PropertyVector.clear();
	switch (data.m_EmitterType)
	{
	case Emitter_Straight:
		{
			m_sEmitterType="Straight Emitter";
			ParticleStraightEmitterData& straightData=data.m_straightEmitterData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction X",COleVariant(straightData.m_direction[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction Y",COleVariant(straightData.m_direction[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction Z",COleVariant(straightData.m_direction[2])));
		}
		break;
	case Emitter_Static:
		m_sEmitterType="Static Emitter";
		break;
	case Emitter_Spheric:
		{
			m_sEmitterType="Spheric Emitter";
			ParticleSphericEmitterData& sphericData=data.m_sphericEmitterData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction X",COleVariant(sphericData.m_direction[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction Y",COleVariant(sphericData.m_direction[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Direction Z",COleVariant(sphericData.m_direction[2])));
			long minAngle=(long)(sphericData.m_angleA*180/float_Pi);
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Min Angle",COleVariant(minAngle),"Range: [0,360]"));
			long maxAngle=(long)(sphericData.m_angleB*180/float_Pi);
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Max Angle",COleVariant(maxAngle),"Range: [0,360]"));
		}
		break;
	case Emitter_Random:
		m_sEmitterType="Random Emitter";
		break;
	case Emitter_Normal:
		{
			m_sEmitterType="Normal Emitter";
			ParticleNormalEmitterData& normalData=data.m_normalEmitterData;
			CParticleCheckGridProperty* pInvertProperty=new CParticleCheckGridProperty("Inverted",normalData.m_bInverted);
			m_PropertyVector.push_back(pInvertProperty);
		}
		break;
	default:
		TRACE("Invalidate Emitter Type!");
		break;
	}

	m_ZoneProperty=new CParticleZoneGridProperty("Zone Setting");
	AddSubItem(m_ZoneProperty);
	m_ZoneProperty->LoadFromData(data.m_zoneParam);
	// Emitter Type
	CParticleTypesGridProperty* pEmitterType=new CParticleTypesGridProperty("Emitter Type",m_sEmitterType);
	AddSubItem(pEmitterType);
	pEmitterType->AddOption("Straight Emitter");
	pEmitterType->AddOption("Static Emitter");
	pEmitterType->AddOption("Spheric Emitter");
	pEmitterType->AddOption("Random Emitter");
	pEmitterType->AddOption("Normal Emitter");
	pEmitterType->AllowEdit(FALSE);
	pEmitterType->SetType(Property_EmitterType);
	pEmitterType->SetDescription("Choose emitter type");
	// Flow
	m_FlowProperty=new CMFCPropertyGridProperty("Flow",COleVariant(data.m_Flow));
	m_FlowProperty->SetDescription("The number of particles per step.");
	AddSubItem(m_FlowProperty);
	// Tank
	m_TankProperty=new CMFCPropertyGridProperty("Tank",COleVariant((long)data.m_Tank));
	m_TankProperty->SetDescription("The number of particles in this emitter's tank. Negative value means infinite.");
	AddSubItem(m_TankProperty);
	// Force
	m_MinForceProperty=new CMFCPropertyGridProperty("Min Force",COleVariant(data.m_ForceMin));
	m_MinForceProperty->SetDescription("The minimum force of the emitter.The particle launches at speed: force/mass of the particle.");
	AddSubItem(m_MinForceProperty);
	m_MaxForceProperty=new CMFCPropertyGridProperty("Max Force",COleVariant(data.m_ForceMax));
	m_MaxForceProperty->SetDescription("The maximum force of the emitter.The particle launches at speed: force/mass of the particle.");
	AddSubItem(m_MaxForceProperty);

	for(unsigned int i=0;i<m_PropertyVector.size();++i) AddSubItem(m_PropertyVector[i]);

	Expand(FALSE);
}

void CParticleEmitterGridProperty::SaveToData(ParticleEmitterParam& data)
{
	memset(&data,0,sizeof(ParticleEmitterParam));

	m_ZoneProperty->SaveToData(data.m_zoneParam);
	
	data.m_Flow=m_FlowProperty->GetValue().fltVal;
	data.m_Tank=m_TankProperty->GetValue().lVal;
	data.m_ForceMin=m_MinForceProperty->GetValue().fltVal;
	data.m_ForceMax=m_MaxForceProperty->GetValue().fltVal;

	if(m_sEmitterType=="Straight Emitter")
	{
		data.m_EmitterType=Emitter_Straight;
		ParticleStraightEmitterData& straightData=data.m_straightEmitterData;
		ASSERT(m_PropertyVector.size()==3);
		straightData.m_direction[0]=m_PropertyVector[0]->GetValue().fltVal;
		straightData.m_direction[1]=m_PropertyVector[1]->GetValue().fltVal;
		straightData.m_direction[2]=m_PropertyVector[2]->GetValue().fltVal;
	}
	else if(m_sEmitterType=="Static Emitter")
	{
		data.m_EmitterType=Emitter_Static;
	}
	else if(m_sEmitterType=="Spheric Emitter")
	{
		data.m_EmitterType=Emitter_Spheric;
		ParticleSphericEmitterData& sphericData=data.m_sphericEmitterData;
		ASSERT(m_PropertyVector.size()==5);
		sphericData.m_direction[0]=m_PropertyVector[0]->GetValue().fltVal;
		sphericData.m_direction[1]=m_PropertyVector[1]->GetValue().fltVal;
		sphericData.m_direction[2]=m_PropertyVector[2]->GetValue().fltVal;
		sphericData.m_angleA=m_PropertyVector[3]->GetValue().lVal*float_Pi/180;
		sphericData.m_angleB=m_PropertyVector[4]->GetValue().lVal*float_Pi/180;
	}
	else if(m_sEmitterType=="Random Emitter")
	{
		data.m_EmitterType=Emitter_Random;
	}
	else if(m_sEmitterType=="Normal Emitter")
	{
		data.m_EmitterType=Emitter_Normal;
		ParticleNormalEmitterData& normalData=data.m_normalEmitterData;
		ASSERT(m_PropertyVector.size()==1);
		CParticleCheckGridProperty* pInvertProperty=(CParticleCheckGridProperty*)m_PropertyVector[0];
		normalData.m_bInverted=pInvertProperty->GetCheck();
	}
	else
	{
		TRACE("Invalidate Emitter Type!");
	}
}
