#include "stdafx.h"

#include "ParticleEntrysGridProperty.h"
#include "ParticleEntryGridProperty.h"
#include "ParticleCtrlMsgDef.h "
#include "../SGPLibraryCode/SGPHeader.h"

IMPLEMENT_DYNAMIC(CParticleEntrysGridProperty,CMFCPropertyGridProperty)

CParticleEntrysGridProperty::CParticleEntrysGridProperty(const CString& strName):\
	CMFCPropertyGridProperty(strName)
{
	m_nNewEntryID=0;
	m_bRGBA=false;
	m_bAngle=false;
}

void CParticleEntrysGridProperty::Init()
{
	CString strName;
	strName.Format("Entry %d",1);
	CParticleEntryGridProperty* pEntry=new CParticleEntryGridProperty(strName);
	if(m_bRGBA) pEntry->SetIsRGBA();
	else if(m_bAngle) pEntry->SetIsAngle();
	pEntry->SetDescription("Right click mouse can popup menu to delete this entry.");
	AddSubItem(pEntry);
	m_EntryVector.push_back(pEntry);
	pEntry->Init();

	CMFCPropertyGridProperty* pNoIndicatorProperty=new CMFCPropertyGridProperty("No Entry","");
	AddSubItem(pNoIndicatorProperty);
	pNoIndicatorProperty->Show(FALSE,FALSE);

	Expand(FALSE);
}

void CParticleEntrysGridProperty::OnRClickName(CPoint C)
{
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING,MENU_ADD_ENTRY,"Add New Entry");
	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,m_pWndList->GetParent());
}

void CParticleEntrysGridProperty::AddNewEntry()
{
	CString strEntryName;
	strEntryName.Format("New Entry %d",++m_nNewEntryID);
	CParticleEntryGridProperty* pEntry=new CParticleEntryGridProperty(strEntryName);
	if(m_bRGBA) pEntry->SetIsRGBA();
	else if(m_bAngle) pEntry->SetIsAngle();

	AddSubItem(pEntry);
	m_EntryVector.push_back(pEntry);
	pEntry->Init();
}

void CParticleEntrysGridProperty::DeleteEntry(CParticleEntryGridProperty* pEntry)
{
	std::vector<CParticleEntryGridProperty*>::iterator iter;
	for(iter=m_EntryVector.begin();iter!=m_EntryVector.end();++iter)
	{
		if(*iter==pEntry)
		{
			m_EntryVector.erase(iter);
			break;
		}
	}
}

void CParticleEntrysGridProperty::LoadFromData(sgp::ParticleSelfDefInterpolatorData& data)
{
	for(uint32 i=0;i<data.m_count;++i)
	{
		CString strName;
		strName.Format("Entry %d",i+1);
		CParticleEntryGridProperty* pEntry=new CParticleEntryGridProperty(strName);
		if(m_bRGBA) pEntry->SetIsRGBA();
		else if(m_bAngle) pEntry->SetIsAngle();
		pEntry->SetDescription("Right click mouse can popup menu to delete this entry.");
		AddSubItem(pEntry);
		m_EntryVector.push_back(pEntry);
		pEntry->LoadFromData(data.m_pEntry[i]);
	}

	CMFCPropertyGridProperty* pNoIndicatorProperty=new CMFCPropertyGridProperty("No Entry","");
	AddSubItem(pNoIndicatorProperty);
	pNoIndicatorProperty->Show(FALSE,FALSE);

	Expand(FALSE);
}

void CParticleEntrysGridProperty::SaveToData(sgp::ParticleSelfDefInterpolatorData& data)
{
	uint32 entrycount=m_EntryVector.size();
	data.m_count=entrycount;
	if(entrycount!=0)
	{
		data.m_pEntry=new ParticleEntryParam[entrycount];
		for(uint32 i=0;i<entrycount;++i)
			m_EntryVector[i]->SaveToData(data.m_pEntry[i]);
	}
	else
	{
		data.m_pEntry=NULL;
	}
}