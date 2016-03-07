#include "stdafx.h"

#include "AddObjectHandler.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "HelpFunction.h"
#include "WorldEditorConfig.h"
#include "AddObjOperation.h"
#include "OperationManager.h"
#include "MouseMsgInterface.h"
#include "SceneObjectManager.h"
#include "CommonObjDef.h"
#include "WorldMapManager.h"

IMPLEMENT_SINGLETON(AddObjectHandler)

AddObjectHandler::AddObjectHandler()
{
	m_pObj=NULL;
	m_bBeginAdd=false;
	m_pAddObjOp=NULL;
}

AddObjectHandler::~AddObjectHandler()
{

}

void AddObjectHandler::AddSceneObject()
{
	CWaitCursor cursor;

	m_pObj=SceneObjectManager::GetInstance()->CreateObject(m_MF1Path,m_fScale);
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->addSceneObject(m_pObj);
}

void AddObjectHandler::LButtonDown(CPoint pt,UINT nFlags)
{
	if(WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain()&&m_pObj!=NULL)
	{
		if(m_pAddObjOp==NULL) m_pAddObjOp=new AddObjOperation;
		m_pAddObjOp->AddObject(CommonObject(m_pObj));
		SceneObjectManager::GetInstance()->EditorAddSceneObject(CommonObject(m_pObj));
		m_pObj=NULL;
		if(!(nFlags&MK_CONTROL))
		{
			EndAdd();
			WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
			OperationManager::GetInstance()->AddNewOperation(m_pAddObjOp);
			m_pAddObjOp=NULL;
			MouseMsgInterface::GetInstance()->SwitchToSelectEditor();
		}
	}
}

void AddObjectHandler::MouseMove(CPoint pt,UINT nFlags)
{
	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	if(m_bBeginAdd&&pRenderInterface->IsIntersectTerrain())
	{
		if(m_pObj==NULL) AddSceneObject();
		
		float x,y,z;
		pRenderInterface->GetIntersectPoint(x,z);
		y=pRenderInterface->GetTerrainHeight(x,z);
		SceneObjectManager::GetInstance()->MoveSceneObject(CommonObject(m_pObj),x,y,z,TRUE);
	}
}

void AddObjectHandler::RButtonUp(CPoint pt,UINT nFlags)
{
	if(WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain())
	{
		if(m_bBeginAdd&&m_pObj!=NULL)
		{
			float rotAngle;
			if(nFlags&MK_SHIFT) rotAngle=(float)float_Pi*5/180;
			else rotAngle=(float)float_Pi*45/180;
			SceneObjectManager::GetInstance()->RotateSceneObject(CommonObject(m_pObj),0.0f,rotAngle,0.0f,FALSE);
		}
	}
}

void AddObjectHandler::SetObjectAttribute(CString absolutePath,float fScale)
{
	m_MF1Path=absolutePath;
	m_fScale=fScale;
}

void AddObjectHandler::CheckUncompleteOp()
{
	if(m_pObj!=NULL)
	{
		SceneObjectManager::GetInstance()->EngineDeleteSceneObject(CommonObject(m_pObj));
		delete m_pObj;
		m_pObj=NULL;
		EndAdd();
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
	}
	if(m_pAddObjOp!=NULL)
	{
		OperationManager::GetInstance()->AddNewOperation(m_pAddObjOp);
		m_pAddObjOp=NULL;
	}
	m_bBeginAdd=false;
}

void AddObjectHandler::BeginAdd()
{
	CheckUncompleteOp();
	m_bBeginAdd=true;
}
