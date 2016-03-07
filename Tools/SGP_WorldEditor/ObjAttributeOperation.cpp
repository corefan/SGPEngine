#include "stdafx.h"

#include "ObjAttributeOperation.h"
#include "HelpFunction.h"
#include "Render Interface\WorldEditorRenderInterface.h"
#include "SceneObjectManager.h"
#include "ObjAttributePane.h"

ObjAttributeOperation::ObjAttributeOperation()
{

}

ObjAttributeOperation::~ObjAttributeOperation()
{
	uint32 size=m_DataVector.size();
	for(uint32 i=0;i<size;++i)
	{
		if(m_DataVector[i].m_Obj.IsMF1())
		{
			delete m_DataVector[i].m_pPrevObj;
			delete m_DataVector[i].m_pCurrObj;
		}
		else if(m_DataVector[i].m_Obj.IsLight())
		{
			delete m_DataVector[i].m_pPrevLightObj;
			delete m_DataVector[i].m_pCurrLightObj;
		}
	}
	m_DataVector.clear();
}

void ObjAttributeOperation::undo()
{
	bool bValidate=false;
	for(uint32 i=0;i<m_DataVector.size();++i)
	{
		ObjAttributeData& data=m_DataVector[i];
		if(data.m_Obj.IsMF1())
		{
			SetAttribute(data.m_Obj.m_pObj,data.m_pPrevObj);
			bValidate=true;
		}
		else if(data.m_Obj.IsLight())
		{
			SetAttribute(data.m_Obj.m_pLightObj,data.m_pPrevLightObj);
		}
		CObjAttributePane::GetInstance()->NotifyAttributeChanged(data.m_Obj);
	}
	if(bValidate) WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
}

void ObjAttributeOperation::redo()
{
	bool bValidate=false;
	for(uint32 i=0;i<m_DataVector.size();++i)
	{
		ObjAttributeData& data=m_DataVector[i];
		if(data.m_Obj.IsMF1())
		{
			SetAttribute(data.m_Obj.m_pObj,data.m_pCurrObj);
			bValidate=true;
		}
		else if(data.m_Obj.IsLight())
		{
			SetAttribute(data.m_Obj.m_pLightObj,data.m_pCurrLightObj);
		}
		CObjAttributePane::GetInstance()->NotifyAttributeChanged(data.m_Obj);
	}
	if(bValidate) WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
}

void ObjAttributeOperation::SetAttribute(ISGPObject* pDestObj,ISGPObject* pSourceObj)
{
	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	CStaticMeshInstance* pMeshInstance=pRenderInterface->GetWorldSystemManager()->getMeshInstanceBySceneID(pDestObj->getSceneObjectID());

	if(pDestObj->m_fPosition[0]!=pSourceObj->m_fPosition[0]||pDestObj->m_fPosition[1]!=pSourceObj->m_fPosition[1]\
		||pDestObj->m_fPosition[2]!=pSourceObj->m_fPosition[2])
		pMeshInstance->setPosition(pSourceObj->m_fPosition[0],pSourceObj->m_fPosition[1],pSourceObj->m_fPosition[2]);

	if(pDestObj->m_fRotationXYZ[0]!=pSourceObj->m_fRotationXYZ[0]||pDestObj->m_fRotationXYZ[1]!=pSourceObj->m_fRotationXYZ[1]\
		||pDestObj->m_fRotationXYZ[2]!=pSourceObj->m_fRotationXYZ[2])
		pMeshInstance->setRotationXYZ(pSourceObj->m_fRotationXYZ[0],pSourceObj->m_fRotationXYZ[1],pSourceObj->m_fRotationXYZ[2]);

	if(pDestObj->m_fScale!=pSourceObj->m_fScale)
		pMeshInstance->setScale(pSourceObj->m_fScale);

	if(pDestObj->m_fAlpha!=pSourceObj->m_fAlpha)
		SceneObjectManager::GetInstance()->SceneObjectSetAlpha(CommonObject(pDestObj),pSourceObj->m_fAlpha);

	if(pDestObj->m_iConfigIndex!=pSourceObj->m_iConfigIndex)
		SceneObjectManager::GetInstance()->SetObjectConfigSetting(CommonObject(pDestObj),pSourceObj->m_iConfigIndex);
	
	CopyObject(pDestObj,pSourceObj);
	pDestObj->m_bRefreshed=true;
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushSceneObject(pDestObj,1,false);
}

void ObjAttributeOperation::SetAttribute(ISGPLightObject* pDestObj,ISGPLightObject* pSourceObj)
{
	CopyObject(pDestObj,pSourceObj);
}

uint32 ObjAttributeOperation::AddObject(ISGPObject* pObj)
{
	ObjAttributeData data;
	data.m_Obj=CommonObject(pObj);
	data.m_pPrevObj=new ISGPObject();
	data.m_pCurrObj=new ISGPObject();
	uint32 size=m_DataVector.size();
	m_DataVector.push_back(data);
	return size;
}

uint32 ObjAttributeOperation::AddObject(ISGPLightObject* pLightObj)
{
	ObjAttributeData data;
	data.m_Obj=CommonObject(pLightObj);
	data.m_pPrevLightObj=new ISGPLightObject();
	data.m_pCurrLightObj=new ISGPLightObject();
	uint32 size=m_DataVector.size();
	m_DataVector.push_back(data);
	return size;
}

void ObjAttributeOperation::SavePrevState(uint32 index)
{
	ASSERT(index<m_DataVector.size());
	ObjAttributeData& data=m_DataVector[index];
	if(data.m_Obj.IsMF1()) CopyObject(data.m_pPrevObj,data.m_Obj.m_pObj);
	else if(data.m_Obj.IsLight()) CopyObject(data.m_pPrevLightObj,data.m_Obj.m_pLightObj);
	else ASSERT(FALSE);
}

void ObjAttributeOperation::SaveCurrState(uint32 index)
{
	ASSERT(index<m_DataVector.size());
	ObjAttributeData& data=m_DataVector[index];
	if(data.m_Obj.IsMF1()) CopyObject(data.m_pCurrObj,data.m_Obj.m_pObj);
	else if(data.m_Obj.IsLight()) CopyObject(data.m_pCurrLightObj,data.m_Obj.m_pLightObj);
	else ASSERT(FALSE);
}

uint32 ObjAttributeOperation::AddObject(const CommonObject& obj)
{
	if(obj.IsMF1()) return AddObject(obj.m_pObj);
	else if(obj.IsLight()) return AddObject(obj.m_pLightObj);
	else ASSERT(FALSE);
	return 0;
}

void ObjAttributeOperation::AddObjects(const std::vector<CommonObject>& objVector)
{
	for(uint32 i=0;i<objVector.size();++i) AddObject(objVector[i]);
}

void ObjAttributeOperation::AddMF1Objects(const std::vector<CommonObject>& objVector)
{
	for(uint32 i=0;i<objVector.size();++i)
	{
		if(objVector[i].IsMF1()) AddObject(objVector[i].m_pObj);
	}
}

void ObjAttributeOperation::AddLightObjects(const std::vector<CommonObject>& objVector)
{
	for(uint32 i=0;i<objVector.size();++i)
	{
		if(objVector[i].IsLight()) AddObject(objVector[i].m_pLightObj);
	}
}

void ObjAttributeOperation::SaveAllPrevState()
{
	for(uint32 i=0;i<m_DataVector.size();++i) SavePrevState(i);
}

void ObjAttributeOperation::SaveAllCurrState()
{
	for(uint32 i=0;i<m_DataVector.size();++i) SaveCurrState(i);
}