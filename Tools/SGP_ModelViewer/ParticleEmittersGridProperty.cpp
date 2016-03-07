#include "stdafx.h"

#include "ParticleEmittersGridProperty.h"
#include "ParticleEmitterGridProperty.h"
#include "ParticleCtrlMsgDef.h"
#include "../SGPLibraryCode/SGPHeader.h"

IMPLEMENT_DYNAMIC(CParticleEmittersGridProperty,CMFCPropertyGridProperty)

CParticleEmittersGridProperty::CParticleEmittersGridProperty(const CString& strName):\
	CMFCPropertyGridProperty(strName)
{
	m_nNewEmitterID=0;
}

void CParticleEmittersGridProperty::OnRClickName(CPoint C)
{
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING,MENU_ADD_EMITTER,"Add New Emitter");
	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,m_pWndList->GetParent());
}

void CParticleEmittersGridProperty::AddNewEmitter()
{
	CString strName;
	strName.Format("New Emitter %d",++m_nNewEmitterID);
	CParticleEmitterGridProperty* pNewEmitter=new CParticleEmitterGridProperty(strName);
	m_EmitterVector.push_back(pNewEmitter);
	AddSubItem(pNewEmitter);
	pNewEmitter->Init();
}

void CParticleEmittersGridProperty::DeleteEmitter(CParticleEmitterGridProperty* pProperty)
{
	std::vector<CParticleEmitterGridProperty*>::iterator iter;
	for(iter=m_EmitterVector.begin();iter!=m_EmitterVector.end();++iter)
	{
		if(*iter==pProperty)
		{
			m_EmitterVector.erase(iter);
			break;
		}
	}
}

void CParticleEmittersGridProperty::Init()
{
/*	m_EmitterVector.push_back(new CParticleEmitterGridProperty("Emitter 1"));
	for(unsigned int i=0;i<m_EmitterVector.size();++i)
	{
		AddSubItem(m_EmitterVector[i]);
		((CParticleEmitterGridProperty*)m_EmitterVector[i])->Init();
	}*/
	CParticleEmitterGridProperty* pEmitterProperty=new CParticleEmitterGridProperty("Emitter 1");
	pEmitterProperty->SetDescription("Right click mouse can popup menu to delete this emitter .");
	AddSubItem(pEmitterProperty);
	m_EmitterVector.push_back(pEmitterProperty);
	pEmitterProperty->Init();

	CMFCPropertyGridProperty* pNoIndicatorProperty=new CMFCPropertyGridProperty("No Emitter","");
	AddSubItem(pNoIndicatorProperty);
	pNoIndicatorProperty->Show(FALSE,FALSE);

	Expand(FALSE);
}

void CParticleEmittersGridProperty::LoadFromData(ParticleGroupParam& data)
{
	uint32 nEmitterCount=data.m_nEmitterCount;
	for(uint32 i=0;i<nEmitterCount;++i)
	{
		CString strName;
		strName.Format("Emitter %d",i+1);
		CParticleEmitterGridProperty* pEmitter=new CParticleEmitterGridProperty(strName);
		pEmitter->SetDescription("Right click mouse can popup menu to delete this emitter .");
		m_EmitterVector.push_back(pEmitter);
		AddSubItem(pEmitter);
		pEmitter->LoadFromData(data.m_pEmitterParam[i]);
	}

	CMFCPropertyGridProperty* pNoIndicatorProperty=new CMFCPropertyGridProperty("No Emitter","");
	AddSubItem(pNoIndicatorProperty);
	pNoIndicatorProperty->Show(FALSE,FALSE);

	Expand(FALSE);
}

void CParticleEmittersGridProperty::SaveToData(ParticleGroupParam& data)
{
	data.m_nEmitterCount=m_EmitterVector.size();
	if(data.m_nEmitterCount!=0)
	{
		data.m_pEmitterParam=new ParticleEmitterParam[data.m_nEmitterCount];
		for(uint32 i=0;i<data.m_nEmitterCount;++i)
		{
			m_EmitterVector[i]->SaveToData(data.m_pEmitterParam[i]);
		}
	}
	else data.m_pEmitterParam=NULL;
}
