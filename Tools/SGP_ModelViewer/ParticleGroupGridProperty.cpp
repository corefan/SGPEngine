#include "stdafx.h"

#include "ParticleGroupGridProperty.h"
#include "ParticleModelGridProperty.h"
#include "ParticleRenderGridProperty.h"
#include "ParticleEmittersGridProperty.h"
#include "ParticleModifiersGridProperty.h"
#include "ParticleCtrlMsgDef.h"
#include "ParticleCheckGridProperty.h"
#include "../SGPLibraryCode/SGPHeader.h"

IMPLEMENT_DYNAMIC(CParticleGroupGridProperty,CMFCPropertyGridProperty)

CParticleGroupGridProperty::CParticleGroupGridProperty(const CString& strName):\
	CMFCPropertyGridProperty(strName)
{
	
}

void CParticleGroupGridProperty::Init()
{
	// whether show this group
	m_ShowProperty=new CParticleCheckGridProperty("Show",true);
	AddSubItem(m_ShowProperty);

	m_ComputeBoundBoxProperty=new CParticleCheckGridProperty("Enable Compute BoundBox","");
	AddSubItem(m_ComputeBoundBoxProperty);
	m_ComputeDistanceProperty=new CParticleCheckGridProperty("Enable Compute Distance","");
	AddSubItem(m_ComputeDistanceProperty);
	m_EnableSortingProperty=new CParticleCheckGridProperty("Enable Sorting","");
	AddSubItem(m_EnableSortingProperty);

	m_GravityXProperty=new CMFCPropertyGridProperty("Gravity X",COleVariant(0.0f));
	AddSubItem(m_GravityXProperty);
	m_GravityYProperty=new CMFCPropertyGridProperty("Gravity Y",COleVariant(0.0f));
	AddSubItem(m_GravityYProperty);
	m_GravityZProperty=new CMFCPropertyGridProperty("Gravity Z",COleVariant(0.0f));
	AddSubItem(m_GravityZProperty);
	m_CapacityProperty=new CMFCPropertyGridProperty("Capacity",COleVariant((long)1000));
	AddSubItem(m_CapacityProperty);
	m_FrictionProperty=new CMFCPropertyGridProperty("Friction",COleVariant(0.0f));
	AddSubItem(m_FrictionProperty);

	m_ModelProperty=new CParticleModelGridProperty("Model Setting");
	AddSubItem(m_ModelProperty);
	m_ModelProperty->Init();

	m_RenderProperty=new CParticleRenderGridProperty("Render Setting");
	AddSubItem(m_RenderProperty);
	m_RenderProperty->Init();

	m_EmittersProperty=new CParticleEmittersGridProperty("Emitters Setting");
	m_EmittersProperty->SetDescription("Right click mouse can popup menu to add new emitter .");
	AddSubItem(m_EmittersProperty);
	m_EmittersProperty->Init();
	
	m_ModifiersProperty=new CParticleModifiersGridProperty("Modifiers Setting");
	m_ModifiersProperty->SetDescription("Right click moust can popup menu to add new modifier .");
	AddSubItem(m_ModifiersProperty);
	m_ModifiersProperty->Init();
	Expand(FALSE);
}

void CParticleGroupGridProperty::OnRClickName(CPoint C)
{
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING,MENU_GROUP_RENAME,"Rename");
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING,MENU_DELETE_GROUP,"Delete");
	
	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,m_pWndList->GetParent());
}

void CParticleGroupGridProperty::LoadFromData(ParticleGroupParam& data)
{
	// whether show this group
	m_ShowProperty=new CParticleCheckGridProperty("Show",true);
	AddSubItem(m_ShowProperty);

	m_ComputeBoundBoxProperty=new CParticleCheckGridProperty("Enable Compute BoundBox",data.m_EnableAABBComputing);
	AddSubItem(m_ComputeBoundBoxProperty);
	m_ComputeDistanceProperty=new CParticleCheckGridProperty("Enable Compute Distance",data.m_EnableComputeDistance);
	AddSubItem(m_ComputeDistanceProperty);
	m_EnableSortingProperty=new CParticleCheckGridProperty("Enable Sorting",data.m_EnableSorting);
	AddSubItem(m_EnableSortingProperty);

	m_GravityXProperty=new CMFCPropertyGridProperty("Gravity X",COleVariant(data.m_Gravity[0]));
	AddSubItem(m_GravityXProperty);
	m_GravityYProperty=new CMFCPropertyGridProperty("Gravity Y",COleVariant(data.m_Gravity[1]));
	AddSubItem(m_GravityYProperty);
	m_GravityZProperty=new CMFCPropertyGridProperty("Gravity Z",COleVariant(data.m_Gravity[2]));
	AddSubItem(m_GravityZProperty);
	m_CapacityProperty=new CMFCPropertyGridProperty("Capacity",COleVariant((long)data.m_Capacity));
	AddSubItem(m_CapacityProperty);
	m_FrictionProperty=new CMFCPropertyGridProperty("Friction",COleVariant(data.m_Friction));
	AddSubItem(m_FrictionProperty);

	m_ModelProperty=new CParticleModelGridProperty("Model Setting");
	AddSubItem(m_ModelProperty);
	m_ModelProperty->LoadFromData(data.m_ModelParam);

	m_RenderProperty=new CParticleRenderGridProperty("Render Setting");
	AddSubItem(m_RenderProperty);
	m_RenderProperty->LoadFromData(data.m_RenderParam);

	m_EmittersProperty=new CParticleEmittersGridProperty("Emitters Setting");
	m_EmittersProperty->SetDescription("Right click mouse can popup menu to add new emitter .");
	AddSubItem(m_EmittersProperty);
	m_EmittersProperty->LoadFromData(data);

	m_ModifiersProperty=new CParticleModifiersGridProperty("Modifiers Setting");
	m_ModifiersProperty->SetDescription("Right click mouse can popup menu to add new modifier .");
	AddSubItem(m_ModifiersProperty);
	m_ModifiersProperty->LoadFromData(data);
	Expand(FALSE);
}

void CParticleGroupGridProperty::SaveToData(ParticleGroupParam& data)
{
	CString strName=GetName();
	ASSERT(strName.Left(6)=="Group ");
	strName=strName.Right(strName.GetLength()-6);
	if(strName.GetLength()>15) strName=strName.Left(15);
	strcpy_s(data.m_GroupName,strName);

	data.m_EnableAABBComputing=m_ComputeBoundBoxProperty->GetCheck();
	data.m_EnableComputeDistance=m_ComputeDistanceProperty->GetCheck();
	data.m_EnableSorting=m_EnableSortingProperty->GetCheck();

	data.m_Gravity[0]=m_GravityXProperty->GetValue().fltVal;
	data.m_Gravity[1]=m_GravityYProperty->GetValue().fltVal;
	data.m_Gravity[2]=m_GravityZProperty->GetValue().fltVal;
	data.m_Capacity=m_CapacityProperty->GetValue().lVal;
	data.m_Friction=m_FrictionProperty->GetValue().fltVal;
	m_ModelProperty->SaveToData(data.m_ModelParam);
	m_RenderProperty->SaveToData(data.m_RenderParam);
	m_EmittersProperty->SaveToData(data);
	m_ModifiersProperty->SaveToData(data);
}
