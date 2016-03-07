#include "stdafx.h"

#include "ParticleModifiersGridProperty.h"
#include "ParticleModifierGridProperty.h"
#include "ParticleCtrlMsgDef.h"
#include "../SGPLibraryCode/SGPHeader.h"

IMPLEMENT_DYNAMIC(CParticleModifiersGridProperty,CMFCPropertyGridProperty)

CParticleModifiersGridProperty::CParticleModifiersGridProperty(const CString& strName):\
	CMFCPropertyGridProperty(strName)
{
	m_nNewModifierID=0;
}

void CParticleModifiersGridProperty::OnRClickName(CPoint C)
{
	CPoint pt;
	GetCursorPos(&pt);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING,MENU_ADD_MODIFIER,"Add New Modifier");
	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,m_pWndList->GetParent());
}

void CParticleModifiersGridProperty::AddNewModifier()
{
	CString strName;
	strName.Format("New Modifier %d",++m_nNewModifierID);
	CParticleModifierGridProperty* pNewModifier=new CParticleModifierGridProperty(strName);
	m_ModifierVector.push_back(pNewModifier);
	AddSubItem(pNewModifier);
	pNewModifier->Init();
}

void CParticleModifiersGridProperty::DeleteModifier(CParticleModifierGridProperty* pProperty)
{
	std::vector<CParticleModifierGridProperty*>::iterator iter;
	for(iter=m_ModifierVector.begin();iter!=m_ModifierVector.end();++iter)
	{
		if(*iter==pProperty)
		{
			m_ModifierVector.erase(iter);
			break;
		}
	}
}

void CParticleModifiersGridProperty::Init()
{
	CMFCPropertyGridProperty* pNoIndicatorProperty=new CMFCPropertyGridProperty("No Modifier","");
	AddSubItem(pNoIndicatorProperty);
	pNoIndicatorProperty->Show(FALSE,FALSE);

	Expand(FALSE);
}

void CParticleModifiersGridProperty::LoadFromData(ParticleGroupParam& data)
{
	uint32 nModifierCount=data.m_nModifierCount;
	for(uint32 i=0;i<nModifierCount;++i)
	{
		CString strName;
		strName.Format("Modifier %d",i+1);
		CParticleModifierGridProperty* pModifierProperty=new CParticleModifierGridProperty(strName);
		pModifierProperty->SetDescription("Right click mouse can popup menu to delete this modifier .");
		AddSubItem(pModifierProperty);
		pModifierProperty->LoadFromData(data.m_pModifierParam[i]);
		m_ModifierVector.push_back(pModifierProperty);
	}

	CMFCPropertyGridProperty* pNoIndicatorProperty=new CMFCPropertyGridProperty("No Modifier","");
	AddSubItem(pNoIndicatorProperty);
	pNoIndicatorProperty->Show(FALSE,FALSE);

	Expand(FALSE);
}

void CParticleModifiersGridProperty::SaveToData(sgp::ParticleGroupParam& data)
{
	data.m_nModifierCount=m_ModifierVector.size();
	if(data.m_nModifierCount!=0)
	{
		data.m_pModifierParam=new ParticleModifierParam[data.m_nModifierCount];
		for(uint32 i=0;i<data.m_nModifierCount;++i)
			m_ModifierVector[i]->SaveToData(data.m_pModifierParam[i]);
	}
}