#include "stdafx.h"

#include "ParticleZoneGridProperty.h"
#include "ParticleTypesGridProperty.h"
#include "ParticleCheckGridProperty.h"
#include "ParticleCtrlMsgDef.h"
#include "../SGPLibraryCode/SGPHeader.h"

IMPLEMENT_DYNAMIC(CParticleZoneGridProperty,CMFCPropertyGridProperty)

CParticleZoneGridProperty::CParticleZoneGridProperty(const CString& strName):\
	CMFCPropertyGridProperty(strName)
{
	
}

void CParticleZoneGridProperty::SetZoneType(CString sType,BOOL bNotify)
{
	if(sType==m_sZoneType&&bNotify) return ;

	unsigned int iItemCount=m_PropertyVector.size();
	unsigned int i;

	for(i=0;i<iItemCount;++i) RemoveSubItem(m_PropertyVector[i]);
	m_PropertyVector.clear();

	if(sType=="Sphere Zone")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Radius",COleVariant(0.0f)));
	}
	else if(sType=="AABox Zone")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Dimension X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Dimension Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Dimension Z",COleVariant(0.0f)));
	}
	else if(sType=="Line Zone")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("StartPos X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("StartPos Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("StartPos Z",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("EndPos X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("EndPos Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("EndPos Z",COleVariant(0.0f)));
	}
	else if(sType=="Plane Zone")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal Y",COleVariant(1.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal Z",COleVariant(0.0f)));
	}
	else if(sType=="Ring Zone")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal Y",COleVariant(1.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal Z",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("MinRadius",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("MaxRadius",COleVariant(1.0f)));
	}
	else if(sType=="Point Zone")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(0.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(0.0f)));
	}

	m_sZoneType=sType;

	for(i=0;i<m_PropertyVector.size();++i) AddSubItem(m_PropertyVector[i]);
	if(bNotify) m_pWndList->GetParent()->PostMessage(UM_UPDATE_PROPERTYCTRL);
}

void CParticleZoneGridProperty::Init()
{
	m_sZoneType="Sphere Zone";
	m_pTypeProperty=new CParticleTypesGridProperty("Zone Type",m_sZoneType);
	m_pTypeProperty->AddOption("Sphere Zone");
	m_pTypeProperty->AddOption("AABox Zone");
	m_pTypeProperty->AddOption("Line Zone");
	m_pTypeProperty->AddOption("Plane Zone");
	m_pTypeProperty->AddOption("Ring Zone");
	m_pTypeProperty->AddOption("Point Zone");
	m_pTypeProperty->AllowEdit(FALSE);
	m_pTypeProperty->SetType(Property_ZoneType);
	m_pTypeProperty->SetDescription("Choose zone type");
	AddSubItem(m_pTypeProperty);
//	CPropertyGridCheckProperty* pFullZone=new CPropertyGridCheckProperty("Full Zone","",Check_None,0);
	m_pOptionProperty=new CParticleCheckGridProperty("Full Zone",true);
	m_pOptionProperty->SetDescription("generate a position in the whole volume of the Zone or only at border");
	AddSubItem(m_pOptionProperty);
	SetZoneType(m_sZoneType,FALSE);
	Expand(FALSE);
}

void CParticleZoneGridProperty::LoadFromData(ParticleZoneParam& data)
{
	m_PropertyVector.clear();
	switch(data.m_ZoneType)
	{
	case Zone_Sphere:
		{
			m_sZoneType="Sphere Zone";
			ParticleSphereZoneData& sphereData=data.m_sphereZoneData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(sphereData.m_position[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(sphereData.m_position[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(sphereData.m_position[2])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Radius",COleVariant(sphereData.m_radius)));
		}
		break;
	case Zone_AABox:
		{
			m_sZoneType="AABox Zone";
			ParticleAABoxZoneData& aaboxData=data.m_aaboxZoneData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(aaboxData.m_position[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(aaboxData.m_position[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(aaboxData.m_position[2])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Dimension X",COleVariant(aaboxData.m_dimension[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Dimension Y",COleVariant(aaboxData.m_dimension[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Dimension Z",COleVariant(aaboxData.m_dimension[2])));
		}
		break;
	case Zone_Line:
		{
			m_sZoneType="Line Zone";
			ParticleLineZoneData& lineData=data.m_lineZoneData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("StartPos X",COleVariant(lineData.m_p0[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("StartPos Y",COleVariant(lineData.m_p0[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("StartPos Z",COleVariant(lineData.m_p0[2])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("EndPos X",COleVariant(lineData.m_p1[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("EndPos Y",COleVariant(lineData.m_p1[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("EndPos Z",COleVariant(lineData.m_p1[2])));
		}
		break;
	case Zone_Plane:
		{
			m_sZoneType="Plane Zone";
			ParticlePlaneZoneData& planeData=data.m_planeZoneData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(planeData.m_position[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(planeData.m_position[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(planeData.m_position[2])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal X",COleVariant(planeData.m_normal[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal Y",COleVariant(planeData.m_normal[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal Z",COleVariant(planeData.m_normal[2])));
		}
		break;
	case Zone_Ring:
		{
			m_sZoneType="Ring Zone";
			ParticleRingZoneData& ringData=data.m_ringZoneData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(ringData.m_position[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(ringData.m_position[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(ringData.m_position[2])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal X",COleVariant(ringData.m_normal[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal Y",COleVariant(ringData.m_normal[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Normal Z",COleVariant(ringData.m_normal[2])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("MinRadius",COleVariant(ringData.m_minRadius)));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("MaxRadius",COleVariant(ringData.m_maxRadius)));
		}
		break;
	case Zone_Point:
		{
			m_sZoneType="Point Zone";
			ParticlePointZoneData& pointData=data.m_pointZoneData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position X",COleVariant(pointData.m_position[0])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Y",COleVariant(pointData.m_position[1])));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Position Z",COleVariant(pointData.m_position[2])));
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	m_pTypeProperty=new CParticleTypesGridProperty("Zone Type",m_sZoneType);
	m_pTypeProperty->AddOption("Sphere Zone");
	m_pTypeProperty->AddOption("AABox Zone");
	m_pTypeProperty->AddOption("Line Zone");
	m_pTypeProperty->AddOption("Plane Zone");
	m_pTypeProperty->AddOption("Ring Zone");
	m_pTypeProperty->AddOption("Point Zone");
	m_pTypeProperty->AllowEdit(FALSE);
	m_pTypeProperty->SetType(Property_ZoneType);
	m_pTypeProperty->SetDescription("Choose zone type");
	AddSubItem(m_pTypeProperty);

	m_pOptionProperty=new CParticleCheckGridProperty("Full Zone",data.m_bFullZone);
	m_pOptionProperty->SetDescription("generate a position in the whole volume of the Zone or only at border");
	AddSubItem(m_pOptionProperty);

	for(unsigned int i=0;i<m_PropertyVector.size();++i) AddSubItem(m_PropertyVector[i]);
	Expand(FALSE);
}

void CParticleZoneGridProperty::SaveToData(ParticleZoneParam& data)
{
	if(m_pOptionProperty->GetCheck()) data.m_bFullZone=true;
	else data.m_bFullZone=false;

	if(m_sZoneType=="Sphere Zone")
	{
		data.m_ZoneType=Zone_Sphere;
		ParticleSphereZoneData& sphereData=data.m_sphereZoneData;
		ASSERT(m_PropertyVector.size()==4);
		sphereData.m_position[0]=m_PropertyVector[0]->GetValue().fltVal;
		sphereData.m_position[1]=m_PropertyVector[1]->GetValue().fltVal;
		sphereData.m_position[2]=m_PropertyVector[2]->GetValue().fltVal;
		sphereData.m_radius=m_PropertyVector[3]->GetValue().fltVal;
	}
	else if(m_sZoneType=="AABox Zone")
	{
		data.m_ZoneType=Zone_AABox;
		ParticleAABoxZoneData& aaboxData=data.m_aaboxZoneData;
		ASSERT(m_PropertyVector.size()==6);
		aaboxData.m_position[0]=m_PropertyVector[0]->GetValue().fltVal;
		aaboxData.m_position[1]=m_PropertyVector[1]->GetValue().fltVal;
		aaboxData.m_position[2]=m_PropertyVector[2]->GetValue().fltVal;
		aaboxData.m_dimension[0]=m_PropertyVector[3]->GetValue().fltVal;
		aaboxData.m_dimension[1]=m_PropertyVector[4]->GetValue().fltVal;
		aaboxData.m_dimension[2]=m_PropertyVector[5]->GetValue().fltVal;
	}
	else if(m_sZoneType=="Line Zone")
	{
		data.m_ZoneType=Zone_Line;
		ParticleLineZoneData& lineData=data.m_lineZoneData;
		ASSERT(m_PropertyVector.size()==6);
		lineData.m_p0[0]=m_PropertyVector[0]->GetValue().fltVal;
		lineData.m_p0[1]=m_PropertyVector[1]->GetValue().fltVal;
		lineData.m_p0[2]=m_PropertyVector[2]->GetValue().fltVal;
		lineData.m_p1[0]=m_PropertyVector[3]->GetValue().fltVal;
		lineData.m_p1[1]=m_PropertyVector[4]->GetValue().fltVal;
		lineData.m_p1[2]=m_PropertyVector[5]->GetValue().fltVal;
	}
	else if(m_sZoneType=="Plane Zone")
	{
		data.m_ZoneType=Zone_Plane;
		ParticlePlaneZoneData& planeData=data.m_planeZoneData;
		ASSERT(m_PropertyVector.size()==6);
		planeData.m_position[0]=m_PropertyVector[0]->GetValue().fltVal;
		planeData.m_position[1]=m_PropertyVector[1]->GetValue().fltVal;
		planeData.m_position[2]=m_PropertyVector[2]->GetValue().fltVal;
		planeData.m_normal[0]=m_PropertyVector[3]->GetValue().fltVal;
		planeData.m_normal[1]=m_PropertyVector[4]->GetValue().fltVal;
		planeData.m_normal[2]=m_PropertyVector[5]->GetValue().fltVal;
	}
	else if(m_sZoneType=="Ring Zone")
	{
		data.m_ZoneType=Zone_Ring;
		ParticleRingZoneData& ringData=data.m_ringZoneData;
		ASSERT(m_PropertyVector.size()==8);
		ringData.m_position[0]=m_PropertyVector[0]->GetValue().fltVal;
		ringData.m_position[1]=m_PropertyVector[1]->GetValue().fltVal;
		ringData.m_position[2]=m_PropertyVector[2]->GetValue().fltVal;
		ringData.m_normal[0]=m_PropertyVector[3]->GetValue().fltVal;
		ringData.m_normal[1]=m_PropertyVector[4]->GetValue().fltVal;
		ringData.m_normal[2]=m_PropertyVector[5]->GetValue().fltVal;
		ringData.m_minRadius=m_PropertyVector[6]->GetValue().fltVal;
		ringData.m_maxRadius=m_PropertyVector[7]->GetValue().fltVal;
	}
	else if(m_sZoneType=="Point Zone")
	{
		data.m_ZoneType=Zone_Point;
		ParticlePointZoneData& pointData=data.m_pointZoneData;
		ASSERT(m_PropertyVector.size()==3);
		pointData.m_position[0]=m_PropertyVector[0]->GetValue().fltVal;
		pointData.m_position[1]=m_PropertyVector[1]->GetValue().fltVal;
		pointData.m_position[2]=m_PropertyVector[2]->GetValue().fltVal;
	}
	else
	{
		ASSERT(FALSE);
	}
}