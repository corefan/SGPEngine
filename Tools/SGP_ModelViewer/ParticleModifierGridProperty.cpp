#include "stdafx.h"

#include "ParticleModifierGridProperty.h"
#include "ParticleTypesGridProperty.h"
#include "ParticleCheckGridProperty.h"
#include "ParticleZoneGridProperty.h"
#include "../SGPLibraryCode/SGPHeader.h"
#include "ParticleCtrlMsgDef.h"

IMPLEMENT_DYNAMIC(CParticleModifierGridProperty,CMFCPropertyGridProperty)

CParticleModifierGridProperty::CParticleModifierGridProperty(const CString& strName):\
	CMFCPropertyGridProperty(strName)
{
}

void CParticleModifierGridProperty::SetModifierType(CString sType,BOOL bNotify)
{
	if(m_sModifierType==sType&&bNotify) return ;

	unsigned int i;
	for(i=0;i<m_PropertyVector.size();++i) RemoveSubItem(m_PropertyVector[i]);
	m_PropertyVector.clear();

	if(sType=="Obstacle")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Bounce Ratio",COleVariant(1.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Friction",COleVariant(1.0f)));
	}
	else if(sType=="Linear Force")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Force X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Force Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Force Z",COleVariant(0.0f)));
		CMFCPropertyGridProperty* pFactorType=new CMFCPropertyGridProperty("Factor Type","None");
		pFactorType->AllowEdit(FALSE);
		pFactorType->AddOption("None");
		pFactorType->AddOption("Linear");
		pFactorType->AddOption("Square");
		pFactorType->SetDescription("The type of multiplier for the factor .");
		m_PropertyVector.push_back(pFactorType);

		CMFCPropertyGridProperty *pFactorParam=new CMFCPropertyGridProperty("Factor Param","Size");
		pFactorParam->AddOption("Red");
		pFactorParam->AddOption("Green");
		pFactorParam->AddOption("Blue");
		pFactorParam->AddOption("Alpha");
		pFactorParam->AddOption("Size");
		pFactorParam->AddOption("Mass");
		pFactorParam->AddOption("Angle");
		pFactorParam->AddOption("Texture Index");
		pFactorParam->AddOption("Rotation Speed");
		pFactorParam->AllowEdit(FALSE);
		pFactorParam->SetDescription("The parameter of the particle to use as the factor .");
		m_PropertyVector.push_back(pFactorParam);
	}
	else if(sType=="Point Mass")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Mass",COleVariant(1.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Min Distance",COleVariant(0.05f)));
	}

	m_sModifierType=sType;

	for(i=0;i<m_PropertyVector.size();++i) AddSubItem(m_PropertyVector[i]);
	if(bNotify) m_pWndList->GetParent()->PostMessage(UM_UPDATE_PROPERTYCTRL);
}

void CParticleModifierGridProperty::OnRClickName(CPoint C)
{
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING,1000,"Delete");
	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,m_pWndList->GetParent());
}

void CParticleModifierGridProperty::Init()
{
	m_sModifierType="Obstacle";
	// Custom Zone
	m_CustomZoneProperty=new CParticleCheckGridProperty("Custom Zone",false);
	m_CustomZoneProperty->SetCheckType(Particle_CheckZone);
	m_CustomZoneProperty->SetDescription("Choose whether set zone. If not ,the Zone is the entire universe .");
	AddSubItem(m_CustomZoneProperty);
	// Zone
	m_ZoneProperty=new CParticleZoneGridProperty("Zone");
	AddSubItem(m_ZoneProperty);
	m_ZoneProperty->Init();
	m_ZoneProperty->Show(FALSE,FALSE);
	// Modifier Type
	CParticleTypesGridProperty* pModifierType=new CParticleTypesGridProperty("Modifier Type",m_sModifierType);
	AddSubItem(pModifierType);
	pModifierType->AddOption("Obstacle");
	pModifierType->AddOption("Linear Force");
	pModifierType->AddOption("Point Mass");
	pModifierType->AllowEdit(FALSE);
	pModifierType->SetType(Property_ModifierType);
	pModifierType->SetDescription("Choose modifier type .");
	SetModifierType(m_sModifierType,FALSE);
	Expand(FALSE);
}

void CParticleModifierGridProperty::ShowZoneProperty(bool bShow)
{
	if(bShow) m_ZoneProperty->Show(TRUE);
	else m_ZoneProperty->Show(FALSE);
}

void CParticleModifierGridProperty::LoadFromData(ParticleModifierParam& data)
{
	m_PropertyVector.clear();
	switch(data.m_ModifierType)
	{
	case Modifier_Obstacle:
		{
			m_sModifierType="Obstacle";
			ParticleObstacleData& obstacleData=data.m_ObstacleData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Bounce Ratio",COleVariant(obstacleData.m_bounceRatio)));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Friction",COleVariant(obstacleData.m_Friction)));
		}
		break;
	case Modifier_LinearForce:
		{
			m_sModifierType="Linear Force";
			ParticleLinearForceData& linearforceData=data.m_LinearForceData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Force X",COleVariant(linearforceData.m_Force[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Force Y",COleVariant(linearforceData.m_Force[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Force Z",COleVariant(linearforceData.m_Force[2])));
			
			// Factor Type
			CString strFactorType;
			switch(linearforceData.m_ForceFactorType)
			{
			case SPARK::FACTOR_NONE:
				strFactorType="None";
				break;
			case SPARK::FACTOR_LINEAR:
				strFactorType="Linear";
				break;
			case SPARK::FACTOR_SQUARE:
				strFactorType="Square";
				break;
			default:
				ASSERT(FALSE);
				break;
			}
			CMFCPropertyGridProperty* pFactorType=new CMFCPropertyGridProperty("Factor Type",strFactorType);
			pFactorType->AllowEdit(FALSE);
			pFactorType->AddOption("None");
			pFactorType->AddOption("Linear");
			pFactorType->AddOption("Square");
			pFactorType->SetDescription("The type of multiplier for the factor .");
			m_PropertyVector.push_back(pFactorType);
			
			// Factor Param
			CString strFactorParam;
			switch(linearforceData.m_FactorParam)
			{
			case SPARK::PARAM_RED:
				strFactorParam="Red";
				break;
			case SPARK::PARAM_GREEN:
				strFactorParam="Green";
				break;
			case SPARK::PARAM_BLUE:
				strFactorParam="Blue";
				break;
			case SPARK::PARAM_ALPHA:
				strFactorParam="Alpha";
				break;
			case SPARK::PARAM_SIZE:
				strFactorParam="Size";
				break;
			case SPARK::PARAM_MASS:
				strFactorParam="Mass";
				break;
			case SPARK::PARAM_ANGLE:
				strFactorParam="Angle";
				break;
			case SPARK::PARAM_TEXTURE_INDEX:
				strFactorParam="Texture Index";
				break;
			case SPARK::PARAM_ROTATION_SPEED:
				strFactorParam="Rotation Speed";
				break;
			default:
				ASSERT(FALSE);
				break;
			}
			CMFCPropertyGridProperty *pFactorParam=new CMFCPropertyGridProperty("Factor Param",strFactorParam);
			pFactorParam->AddOption("Red");
			pFactorParam->AddOption("Green");
			pFactorParam->AddOption("Blue");
			pFactorParam->AddOption("Alpha");
			pFactorParam->AddOption("Size");
			pFactorParam->AddOption("Mass");
			pFactorParam->AddOption("Angle");
			pFactorParam->AddOption("Texture Index");
			pFactorParam->AddOption("Rotation Speed");
			pFactorParam->AllowEdit(FALSE);
			pFactorParam->SetDescription("The parameter of the particle to use as the factor .");
			m_PropertyVector.push_back(pFactorParam);
		}
		break;
	case Modifier_PointMass:
		{
			m_sModifierType="Point Mass";
			ParticlePointMassData& pointMassData=data.m_PointMassData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(pointMassData.m_Position[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(pointMassData.m_Position[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(pointMassData.m_Position[2])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Mass",COleVariant(pointMassData.m_Mass)));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Min Distance",COleVariant(pointMassData.m_MinDistance)));
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	// Custom Zone
	m_CustomZoneProperty=new CParticleCheckGridProperty("Custom Zone",data.m_bCustomZone);
	m_CustomZoneProperty->SetCheckType(Particle_CheckZone);
	m_CustomZoneProperty->SetDescription("Choose whether set zone. If not ,the Zone is the entire universe .");
	AddSubItem(m_CustomZoneProperty);
	// Zone
	m_ZoneProperty=new CParticleZoneGridProperty("Zone");
	AddSubItem(m_ZoneProperty);
	if(data.m_bCustomZone)
	{
		m_ZoneProperty->LoadFromData(data.m_ZoneParam);
		m_ZoneProperty->Show(TRUE,FALSE);
	}
	else
	{
		m_ZoneProperty->Init();
		m_ZoneProperty->Show(FALSE,FALSE);
	}
	// Modifier Type
	CParticleTypesGridProperty* pModifierType=new CParticleTypesGridProperty("Modifier Type",m_sModifierType);
	AddSubItem(pModifierType);
	pModifierType->AddOption("Obstacle");
	pModifierType->AddOption("Linear Force");
	pModifierType->AddOption("Point Mass");
	pModifierType->AllowEdit(FALSE);
	pModifierType->SetType(Property_ModifierType);
	pModifierType->SetDescription("Choose modifier type .");

	for(uint32 i=0;i<m_PropertyVector.size();++i) AddSubItem(m_PropertyVector[i]);

	Expand(FALSE);
}

void CParticleModifierGridProperty::SaveToData(ParticleModifierParam& data)
{
	memset(&data,0,sizeof(ParticleModifierParam));
	// Zone Related
	if(m_CustomZoneProperty->GetCheck())
	{
		data.m_bCustomZone=true;
		m_ZoneProperty->SaveToData(data.m_ZoneParam);
	}
	else
		data.m_bCustomZone=false;

	// Modifier Type
	if(m_sModifierType=="Obstacle")
	{
		data.m_ModifierType=Modifier_Obstacle;
		ASSERT(m_PropertyVector.size()==2);
		data.m_ObstacleData.m_bounceRatio=m_PropertyVector[0]->GetValue().fltVal;
		data.m_ObstacleData.m_Friction=m_PropertyVector[1]->GetValue().fltVal;
	}
	else if(m_sModifierType=="Linear Force")
	{
		data.m_ModifierType=Modifier_LinearForce;
		ASSERT(m_PropertyVector.size()==5);
		ParticleLinearForceData& linearforceData=data.m_LinearForceData;
		linearforceData.m_Force[0]=m_PropertyVector[0]->GetValue().fltVal;
		linearforceData.m_Force[1]=m_PropertyVector[1]->GetValue().fltVal;
		linearforceData.m_Force[2]=m_PropertyVector[2]->GetValue().fltVal;

		// Factor Type
		CString strFactorType=m_PropertyVector[3]->GetValue();
		if(strFactorType=="None") linearforceData.m_ForceFactorType=SPARK::FACTOR_NONE;
		else if(strFactorType=="Linear") linearforceData.m_ForceFactorType=SPARK::FACTOR_LINEAR;
		else if(strFactorType=="Square") linearforceData.m_ForceFactorType=SPARK::FACTOR_SQUARE;
		else ASSERT(FALSE);

		// Factor Param
		CString strFactorParam=m_PropertyVector[4]->GetValue();
		if(strFactorParam=="Red") linearforceData.m_FactorParam=SPARK::PARAM_RED;
		else if(strFactorParam=="Green") linearforceData.m_FactorParam=SPARK::PARAM_GREEN;
		else if(strFactorParam=="Blue") linearforceData.m_FactorParam=SPARK::PARAM_BLUE;
		else if(strFactorParam=="Alpha") linearforceData.m_FactorParam=SPARK::PARAM_ALPHA;
		else if(strFactorParam=="Size") linearforceData.m_FactorParam=SPARK::PARAM_SIZE;
		else if(strFactorParam=="Mass") linearforceData.m_FactorParam=SPARK::PARAM_MASS;
		else if(strFactorParam=="Angle") linearforceData.m_FactorParam=SPARK::PARAM_ANGLE;
		else if(strFactorParam=="Texture Index") linearforceData.m_FactorParam=SPARK::PARAM_TEXTURE_INDEX;
		else if(strFactorParam=="Rotation Speed") linearforceData.m_FactorParam=SPARK::PARAM_ROTATION_SPEED;
		else ASSERT(FALSE);
	}
	else if(m_sModifierType=="Point Mass")
	{
		data.m_ModifierType=Modifier_PointMass;
		ASSERT(m_PropertyVector.size()==5);
		ParticlePointMassData& pointmassData=data.m_PointMassData;
		pointmassData.m_Position[0]=m_PropertyVector[0]->GetValue().fltVal;
		pointmassData.m_Position[1]=m_PropertyVector[1]->GetValue().fltVal;
		pointmassData.m_Position[2]=m_PropertyVector[2]->GetValue().fltVal;
		pointmassData.m_Mass=m_PropertyVector[3]->GetValue().fltVal;
		pointmassData.m_MinDistance=m_PropertyVector[4]->GetValue().fltVal;
	}
	else
	{
		ASSERT(FALSE);
	}
}
