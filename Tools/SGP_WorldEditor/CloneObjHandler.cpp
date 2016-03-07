#include "stdafx.h"

#include "CloneObjHandler.h"
#include "SceneObjectManager.h"
#include "Render Interface\WorldEditorRenderInterface.h"
#include "AddObjOperation.h"
#include "OperationManager.h"
#include "MouseMsgInterface.h"
#include "WorldMapManager.h"

IMPLEMENT_SINGLETON(CloneObjHandler)

CloneObjHandler::CloneObjHandler()
{
	m_CurrObj.SetNULL();
	m_OrigObj.SetNULL();
	m_pAddObjOp=NULL;
	m_bBeginAdd=false;
}

CloneObjHandler::~CloneObjHandler()
{

}

void CloneObjHandler::AddSceneObject()
{
	ASSERT(!m_OrigObj.IsNULL());
	CWaitCursor cursor;
	m_CurrObj=SceneObjectManager::GetInstance()->CloneSceneObject(m_OrigObj);
	if(m_CurrObj.IsMF1())
		SceneObjectManager::GetInstance()->EngineAddSceneObject(m_CurrObj);
	else if(m_CurrObj.IsLight())
	{
		WorldMapManager::GetInstance()->AddSceneObject(m_CurrObj);
	}
}

void CloneObjHandler::LButtonDown(CPoint pt,UINT nFlags)
{
	if(WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain()&&(!(m_CurrObj.IsNULL())))
	{
		if(m_pAddObjOp==NULL) m_pAddObjOp=new AddObjOperation;
		m_pAddObjOp->AddObject(m_CurrObj);
		if(m_CurrObj.IsMF1())
			SceneObjectManager::GetInstance()->EditorAddSceneObject(m_CurrObj);
		else if(m_CurrObj.IsLight())
			SceneObjectManager::GetInstance()->AddSceneObject(m_CurrObj);

		m_CurrObj.SetNULL();
		if(!(nFlags&MK_CONTROL))
		{
			EndAdd();
			if(m_OrigObj.IsMF1()) WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
			OperationManager::GetInstance()->AddNewOperation(m_pAddObjOp);
			m_pAddObjOp=NULL;
			MouseMsgInterface::GetInstance()->SwitchToSelectEditor();
		}
	}
}

void CloneObjHandler::MouseMove(CPoint pt,UINT nFlags)
{
	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	if(m_bBeginAdd&&pRenderInterface->IsIntersectTerrain())
	{
		if(m_CurrObj.IsNULL()) AddSceneObject();

		float x,y,z;
		pRenderInterface->GetIntersectPoint(x,z);
		y=pRenderInterface->GetTerrainHeight(x,z);
		SceneObjectManager::GetInstance()->MoveSceneObject(m_CurrObj,x,y,z,TRUE);
	}
}

void CloneObjHandler::RButtonUp(CPoint pt,UINT nFlags)
{
	if(WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain())
	{
		if(m_bBeginAdd&&(!m_CurrObj.IsNULL()))
		{
			float rotAngle;
			if(nFlags&MK_SHIFT) rotAngle=(float)float_Pi*5/180;
			else rotAngle=(float)float_Pi*45/180;
			SceneObjectManager::GetInstance()->RotateSceneObject(m_CurrObj,0.0f,rotAngle,0.0f,FALSE);
		}
	}
}

void CloneObjHandler::CheckUncompleteOp()
{
	if(!m_CurrObj.IsNULL())
	{
		if(m_CurrObj.IsMF1())
		{
			SceneObjectManager::GetInstance()->EngineDeleteSceneObject(m_CurrObj);
			delete m_CurrObj.m_pObj;
			WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
		}
		else if(m_CurrObj.IsLight())
		{
			WorldMapManager::GetInstance()->DeleteSceneObject(m_CurrObj);
			delete m_CurrObj.m_pLightObj;
		}
		m_CurrObj.SetNULL();
	}
	if(m_pAddObjOp!=NULL)
	{
		OperationManager::GetInstance()->AddNewOperation(m_pAddObjOp);
		m_pAddObjOp=NULL;
	}
	m_bBeginAdd=false;
}

void CloneObjHandler::BeginAdd(const CommonObject& obj)
{
	m_OrigObj=obj;
	m_bBeginAdd=true;
}