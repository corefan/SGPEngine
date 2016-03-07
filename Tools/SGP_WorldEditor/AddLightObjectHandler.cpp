#include "stdafx.h"

#include "AddLightObjectHandler.h"
#include "Render Interface/WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "WorldMapManager.h"
#include "MouseMsgInterface.h"
#include "SceneObjectManager.h"
#include "CommonObjDef.h"
#include "WorldMapManager.h"
#include "AddObjOperation.h"
#include "OperationManager.h"

IMPLEMENT_SINGLETON(AddLightObjHandler)

AddLightObjHandler::AddLightObjHandler()
{
	m_pLightObj=NULL;
	m_bBeginAdd=false;
	m_pAddObjOp=NULL;
}

AddLightObjHandler::~AddLightObjHandler()
{

}

void AddLightObjHandler::AddLightObject()
{
	m_pLightObj=SceneObjectManager::GetInstance()->CreateLightObject();
	WorldMapManager::GetInstance()->AddSceneObject(CommonObject(m_pLightObj));
}

void AddLightObjHandler::LButtonDown(CPoint pt,UINT nFlags)
{
	if(WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain()&&m_pLightObj!=NULL)
	{
		if(m_pAddObjOp==NULL) m_pAddObjOp=new AddObjOperation;
		m_pAddObjOp->AddObject(CommonObject(m_pLightObj));
		SceneObjectManager::GetInstance()->AddSceneObject(CommonObject(m_pLightObj));
		m_pLightObj=NULL;
		if(!(nFlags&MK_CONTROL))
		{
			OperationManager::GetInstance()->AddNewOperation(m_pAddObjOp);
			m_pAddObjOp=NULL;
			EndAdd();
			MouseMsgInterface::GetInstance()->SwitchToSelectEditor();
		}
	}
}

void AddLightObjHandler::LButtonUp(CPoint pt,UINT nFlags)
{

}

void AddLightObjHandler::MouseMove(CPoint pt,UINT nFlags)
{
	if(m_bBeginAdd)
	{
		if(m_pLightObj==NULL) AddLightObject();
		WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
		if(pRenderInterface->IsIntersectTerrain())
		{
			float x,y,z;
			pRenderInterface->GetIntersectPoint(x,z);
			y=pRenderInterface->GetTerrainHeight(x,z);
			m_pLightObj->m_fPosition[0]=x;
			m_pLightObj->m_fPosition[1]=y;
			m_pLightObj->m_fPosition[2]=z;
		}
	}
}

void AddLightObjHandler::CheckUncompleteOp()
{
	if(m_pLightObj!=NULL)
	{
		WorldMapManager::GetInstance()->DeleteSceneObject(CommonObject(m_pLightObj));
		delete m_pLightObj;
		m_pLightObj=NULL;
	}
	if(m_pAddObjOp!=NULL)
	{
		OperationManager::GetInstance()->AddNewOperation(m_pAddObjOp);
		m_pAddObjOp=NULL;
	}
	m_bBeginAdd=false;
}

void AddLightObjHandler::BeginAdd()
{
	CheckUncompleteOp();
	m_bBeginAdd=true;
}