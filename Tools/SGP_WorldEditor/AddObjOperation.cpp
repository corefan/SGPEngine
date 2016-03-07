#include "stdafx.h"
#include "AddObjOperation.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "SceneObjectManager.h"
#include "HelpFunction.h"

AddObjOperation::AddObjOperation()
{
	SetDeleteFlag(false);
}

AddObjOperation::~AddObjOperation()
{
	if(m_bDelete)
	{
		for(uint32 i=0;i<m_ObjVector.size();++i)
		{
			if(m_ObjVector[i].IsMF1()) delete m_ObjVector[i].m_pObj;
			else if(m_ObjVector[i].IsLight()) delete m_ObjVector[i].m_pLightObj;
			else ASSERT(FALSE);
		}
	}
}

void AddObjOperation::undo()
{
	SetDeleteFlag(true);
	// delete objects
	SceneObjectManager::GetInstance()->DeleteSceneObject(m_ObjVector);
	if(ContainMF1(m_ObjVector))
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
}

void AddObjOperation::redo()
{
	SetDeleteFlag(false);
	// add objects
	SceneObjectManager::GetInstance()->AddSceneObject(m_ObjVector);
	if(ContainMF1(m_ObjVector))
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
}

void AddObjOperation::AddObject(const std::vector<CommonObject>& objVector)
{
	uint32 size=objVector.size();
	for(uint32 i=0;i<size;++i) m_ObjVector.push_back(objVector[i]);
}