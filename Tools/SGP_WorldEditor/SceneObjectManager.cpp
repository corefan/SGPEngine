#include "stdafx.h"
#include "WorldEditor.h"
#include "SceneObjectManager.h"
#include "SceneObjectTree.h"
#include "WorldMapManager.h"
#include "Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "HelpFunction.h"
#include "WorldEditorConfig.h"
#include "OperationManager.h"
#include "AddObjOperation.h"
#include "WorldEditorConfig.h"
#include "MainFrame.h"
#include "SelectObjHandler.h"
#include "ObjAttributePane.h"
#include <set>

IMPLEMENT_SINGLETON(SceneObjectManager)

SceneObjectManager::SceneObjectManager()
{

}

SceneObjectManager::~SceneObjectManager()
{

}

void SceneObjectManager::AddSceneObject(const std::vector<CommonObject>& objVector)
{
	CWaitCursor cursor;
	for(uint32 i=0;i<objVector.size();++i) AddSceneObject(objVector[i]);
}

void SceneObjectManager::DeleteSceneObject(const std::vector<CommonObject>& objVector)
{
	for(uint32 i=0;i<objVector.size();++i) DeleteSceneObject(objVector[i]);
}

void SceneObjectManager::AddSceneObject(const CommonObject& obj)
{
	EditorAddSceneObject(obj);
	EngineAddSceneObject(obj);
}

void SceneObjectManager::EditorAddSceneObject(const CommonObject& obj)
{
	CSceneObjectTree::GetInstance()->AddSceneObject(obj);
	WorldMapManager::GetInstance()->AddSceneObject(obj);
}

void SceneObjectManager::EngineAddSceneObject(const CommonObject& obj)
{
	if(obj.IsMF1())
	{
		ISGPObject* pObj=obj.m_pObj;
		pObj->m_iObjectInChunkIndexNum=0;
		if(pObj->m_pObjectInChunkIndex!=NULL)
		{
			delete pObj->m_pObjectInChunkIndex;
			pObj->m_pObjectInChunkIndex=NULL;
		}
		pObj->m_bRefreshed=true;
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->addSceneObject(pObj,pObj->m_iConfigIndex);
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushSceneObject(pObj,1,false);
	}
	else if(obj.IsLight())
	{
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->addLightObject(obj.m_pLightObj);
	}
	else ASSERT(FALSE);
}

void SceneObjectManager::DeleteSceneObject(const CommonObject& obj)
{
	EditorDeleteSceneObject(obj);
	EngineDeleteSceneObject(obj);
}

void SceneObjectManager::EditorDeleteSceneObject(const CommonObject& obj)
{
	CSceneObjectTree::GetInstance()->DeleteSceneObject(obj);
	WorldMapManager::GetInstance()->DeleteSceneObject(obj);
}

void SceneObjectManager::EngineDeleteSceneObject(const CommonObject& obj)
{
	if(obj.IsMF1())
	{
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->deleteSceneObject(obj.m_pObj);
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushSceneObject(obj.m_pObj,1,true);
	}
	else if(obj.IsLight())
	{
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->deleteLightObject(obj.m_pLightObj);
	}
	else ASSERT(FALSE);
}

sgp::ISGPObject* SceneObjectManager::CreateObject(CString& strFile,float fScale)
{
	CString relativePath=GetRelativePath(strFile,WorldEditorConfig::GetInstance()->GetDataFileDir());
	BSTR objPath=relativePath.AllocSysString();

	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	CString sceneName;
	sceneName=WorldMapManager::GetInstance()->GetObjSceneNameByPath(relativePath);
	ISGPObject* pObj=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->createObject(String(objPath),String((LPCTSTR)sceneName));
	SysFreeString(objPath);
	pObj->m_fScale=fScale;

	return pObj;
}

sgp::ISGPLightObject* SceneObjectManager::CreateLightObject()
{
	WorldEditorRenderInterface *pRenderInterface=WorldEditorRenderInterface::GetInstance();
	CString sceneName=WorldMapManager::GetInstance()->GetLightObjSceneName();
	BSTR lightName=sceneName.AllocSysString();
	ISGPLightObject* pLightObj=pRenderInterface->GetWorldSystemManager()->createLightObject(String(lightName));
	SysFreeString(lightName);
//	pRenderInterface->GetWorldSystemManager()->addLightObject(pLightObj);
	return pLightObj;
}

void SceneObjectManager::SetObjectConfigSetting(const CommonObject& obj,unsigned int index)
{
	ASSERT(obj.IsMF1());

	ISGPObject* pObj=obj.m_pObj;
	if(pObj->m_iConfigIndex!=index)
	{
		pObj->m_iConfigIndex=index;

		CStaticMeshInstance* pMeshInstance=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getMeshInstanceBySceneID(pObj->getSceneObjectID());
		CMF1FileResource* pMF1Res=WorldEditorRenderInterface::GetInstance()->GetRenderDevice()->GetModelManager()->getModelByID(pMeshInstance->getMF1ModelResourceID());
		CSGPModelMF1* pMF1Model=(pMF1Res!=NULL)?pMF1Res->pModelMF1:NULL;
		ASSERT(pMF1Model!=NULL);

		if(index>0&&index<=pMF1Model->m_Header.m_iNumConfigs)
			pMeshInstance->setConfigSetting(&(pMF1Model->m_pConfigSetting[index-1]));
		else
			pMeshInstance->setConfigSetting(NULL);
	}
}

void SceneObjectManager::SceneObjNameChanged(const CommonObject& obj)
{
	CSceneObjectTree::GetInstance()->SceneObjNameChanged(obj);
}

void SceneObjectManager::MoveSceneObject(const std::vector<CommonObject>& objVector,float dx,float dy,float dz,BOOL bSet)
{
	for(uint32 i=0;i<objVector.size();++i)
		MoveSceneObject(objVector[i],dx,dy,dz,bSet);
}

void SceneObjectManager::RotateSceneObject(const std::vector<CommonObject>& objVector,float dx,float dy,float dz,BOOL bSet)
{
	for(uint32 i=0;i<objVector.size();++i)
		RotateSceneObject(objVector[i],dx,dy,dz,bSet);
}

void SceneObjectManager::ScaleSceneObject(const std::vector<CommonObject>& objVector,float fScale,BOOL bSet)
{
	for(uint32 i=0;i<objVector.size();++i)
		ScaleSceneObject(objVector[i],fScale,bSet);
}

void SceneObjectManager::MoveSceneObject(const CommonObject& obj,float dx,float dy,float dz,BOOL bSet)
{
	if(obj.IsMF1())
	{
		if(bSet)
		{
			obj.m_pObj->m_fPosition[0]=dx;
			obj.m_pObj->m_fPosition[1]=dy;
			obj.m_pObj->m_fPosition[2]=dz;
		}
		else
		{
			obj.m_pObj->m_fPosition[0]+=dx;
			obj.m_pObj->m_fPosition[1]+=dy;
			obj.m_pObj->m_fPosition[2]+=dz;
		}
		
		CStaticMeshInstance* pMeshInstance=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getMeshInstanceBySceneID(obj.m_pObj->getSceneObjectID());
		pMeshInstance->setPosition(obj.m_pObj->m_fPosition[0],obj.m_pObj->m_fPosition[1],obj.m_pObj->m_fPosition[2]);
		obj.m_pObj->m_bRefreshed=true;
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushSceneObject(obj.m_pObj,1,false);
		WorldMapManager::GetInstance()->NotifyRefreshCollSet();
	}
	else if(obj.IsLight())
	{
		if(bSet)
		{
			obj.m_pLightObj->m_fPosition[0]=dx;
			obj.m_pLightObj->m_fPosition[1]=dy;
			obj.m_pLightObj->m_fPosition[2]=dz;
		}
		else
		{
			obj.m_pLightObj->m_fPosition[0]+=dx;
			obj.m_pLightObj->m_fPosition[1]+=dy;
			obj.m_pLightObj->m_fPosition[2]+=dz;
		}
	}
}

void SceneObjectManager::RotateSceneObject(const CommonObject& obj,float dx,float dy,float dz,BOOL bSet)
{
	if(obj.IsMF1())
	{
		if(bSet)
		{
			obj.m_pObj->m_fRotationXYZ[0]=dx;
			obj.m_pObj->m_fRotationXYZ[1]=dy;
			obj.m_pObj->m_fRotationXYZ[2]=dz;
		}
		else
		{
			obj.m_pObj->m_fRotationXYZ[0]+=dx;
			obj.m_pObj->m_fRotationXYZ[1]+=dy;
			obj.m_pObj->m_fRotationXYZ[2]+=dz;
		}
		for(uint32 i=0;i<3;++i)
		{
			while(obj.m_pObj->m_fRotationXYZ[i]<0.0f) obj.m_pObj->m_fRotationXYZ[i]+=float_2pi;
			while(obj.m_pObj->m_fRotationXYZ[i]>=float_2pi) obj.m_pObj->m_fRotationXYZ[i]-=float_2pi;
		}

		CStaticMeshInstance* pMeshInstance=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getMeshInstanceBySceneID(obj.m_pObj->getSceneObjectID());
		pMeshInstance->setRotationXYZ(obj.m_pObj->m_fRotationXYZ[0],obj.m_pObj->m_fRotationXYZ[1],obj.m_pObj->m_fRotationXYZ[2]);
		obj.m_pObj->m_bRefreshed=true;
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushSceneObject(obj.m_pObj,1,false);
		WorldMapManager::GetInstance()->NotifyRefreshCollSet();
	}
}

void SceneObjectManager::ScaleSceneObject(const CommonObject& obj,float fScale,BOOL bSet)
{
	if(obj.IsMF1())
	{
		if(bSet) obj.m_pObj->m_fScale=fScale;
		else obj.m_pObj->m_fScale*=fScale;
		
		CStaticMeshInstance* pMeshInstance=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getMeshInstanceBySceneID(obj.m_pObj->getSceneObjectID());
		pMeshInstance->setScale(obj.m_pObj->m_fScale);
		obj.m_pObj->m_bRefreshed=true;
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushSceneObject(obj.m_pObj,1,false);
		WorldMapManager::GetInstance()->NotifyRefreshCollSet();
	}
}

void SceneObjectManager::SceneObjectSetAlpha(const CommonObject& obj,float fAlpha)
{
	if(obj.IsMF1())
	{
		obj.m_pObj->m_fAlpha=fAlpha;
		CStaticMeshInstance* pMeshInstance=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getMeshInstanceBySceneID(obj.m_pObj->getSceneObjectID());
		pMeshInstance->setInstanceAlpha(fAlpha);
	}
}

/*void SceneObjectManager::SelectedObjectsChanged(const std::vector<CommonObject>& objVector)
{
	CSceneObjectTree::GetInstance()->SelectedObjectsChanged(objVector);
}*/

void SceneObjectManager::DeleteObjectOperation(const std::vector<CommonObject>& objVector)
{
	DeleteObjOperation* pDeleteOp=new DeleteObjOperation;
	pDeleteOp->AddObject(objVector);
	pDeleteOp->redo();
	OperationManager::GetInstance()->AddNewOperation(pDeleteOp);

	SelectObjHandler::GetInstance()->ClearSelection();
}

void SceneObjectManager::DeleteObjectOperation(const CommonObject& obj)
{
	DeleteObjOperation* pDeleteOp=new DeleteObjOperation;
	pDeleteOp->AddObject(obj);
	pDeleteOp->redo();
	OperationManager::GetInstance()->AddNewOperation(pDeleteOp);

	SelectObjHandler::GetInstance()->ClearSelection();
}

void SceneObjectManager::ReloadSceneObject(const std::vector<CommonObject>& objVector)
{
	CWaitCursor cursor;

	uint32 i,j;
	std::set<CString> mf1Set;
	for(i=0;i<objVector.size();++i)
	{
		ASSERT(objVector[i].IsMF1());
		mf1Set.insert(CString(objVector[i].m_pObj->m_MF1FileName));
	}

	std::vector<ISGPObject*>& allObjVec=WorldMapManager::GetInstance()->GetWorldObjects();
	std::set<CString>::iterator iter;
	for(iter=mf1Set.begin();iter!=mf1Set.end();++iter)
	{
		CString mf1Name=*iter;
		std::vector<CommonObject> reloadObjVector;
		for(i=0;i<allObjVec.size();++i)
		{
			if(mf1Name==allObjVec[i]->m_MF1FileName) reloadObjVector.push_back(CommonObject(allObjVec[i]));	
		}
		for(j=0;j<reloadObjVector.size();++j) EngineDeleteSceneObject(reloadObjVector[j]);
		for(j=0;j<reloadObjVector.size();++j) EngineAddSceneObject(reloadObjVector[j]);
	}
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
}

void SceneObjectManager::ReloadSceneObject(sgp::ISGPObject* pObj)
{
	CWaitCursor cursor;

	std::vector<ISGPObject*>& objVector=WorldMapManager::GetInstance()->GetWorldObjects();
	CString sMF1FileName=pObj->m_MF1FileName;
	std::vector<ISGPObject*> reloadObjVector;
	uint32 i;
	for(i=0;i<objVector.size();++i)
	{
		if(sMF1FileName==objVector[i]->m_MF1FileName) reloadObjVector.push_back(objVector[i]);
	}
	for(i=0;i<reloadObjVector.size();++i) EngineDeleteSceneObject(CommonObject(reloadObjVector[i]));
	for(i=0;i<reloadObjVector.size();++i) EngineAddSceneObject(CommonObject(reloadObjVector[i]));
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
}

void SceneObjectManager::OpenSceneObject(const CommonObject& obj)
{
	ASSERT(obj.IsMF1());
	CString modelPath=obj.m_pObj->m_MF1FileName;
	modelPath=WorldEditorConfig::GetInstance()->GetDataFileDir()+"\\"+modelPath;
	CMainFrame::OpenModelViewer(modelPath);
}

void SceneObjectManager::ShowSceneObjectAttr(const CommonObject& obj)
{
	CObjAttributePane::GetInstance()->ShowPane(TRUE,FALSE,TRUE);
	CObjAttributePane::GetInstance()->LoadAttribute(obj);
}

void SceneObjectManager::DeleteSelectedObjOperation()
{
	std::vector<CommonObject>& objVector=SelectObjHandler::GetInstance()->GetSelectedObj();
	DeleteObjOperation* pDeleteOp=new DeleteObjOperation;
	pDeleteOp->AddObject(objVector);
	pDeleteOp->redo();
	OperationManager::GetInstance()->AddNewOperation(pDeleteOp);

	SelectObjHandler::GetInstance()->ClearSelection();
}

CommonObject SceneObjectManager::CloneSceneObject(const CommonObject& obj)
{
	CommonObject result;
	if(obj.IsMF1())
	{
		result.m_type = ObjType_MF1;
		result.m_pObj = new ISGPObject;
		memcpy(result.m_pObj,obj.m_pObj,sizeof(ISGPObject));
		result.m_pObj->m_iObjectInChunkIndexNum=0;
		result.m_pObj->m_pObjectInChunkIndex=NULL;
		CString strObjName=WorldMapManager::GetInstance()->GetObjSceneNameByPath(CString(result.m_pObj->m_MF1FileName));
		strcpy_s(result.m_pObj->m_SceneObjectName,sizeof(result.m_pObj->m_SceneObjectName),(LPCTSTR)strObjName);
	}
	else if(obj.IsLight())
	{
		result.m_type = ObjType_Light;
		result.m_pLightObj = new ISGPLightObject;
		memcpy(result.m_pLightObj,obj.m_pLightObj,sizeof(ISGPLightObject));
		CString strLightName=WorldMapManager::GetInstance()->GetLightObjSceneName();
		strcpy_s(result.m_pLightObj->m_SceneObjectName,sizeof(result.m_pLightObj->m_SceneObjectName),(LPCTSTR)strLightName);
	}
	else
	{
		ASSERT(FALSE);
	}

	return result;
}

void SceneObjectManager::FocusObject(const CommonObject& obj,void *pSender)
{
	if(pSender!=CSceneObjectTree::GetInstance())
		CSceneObjectTree::GetInstance()->FocusObject(obj);
	if(MouseMsgInterface::GetInstance()->IsSelectObjHandler())
	{
		if(pSender!=SelectObjHandler::GetInstance())
			SelectObjHandler::GetInstance()->FocusObject(obj);
	}
	if(pSender!=WorldEditorRenderInterface::GetInstance())
		WorldEditorRenderInterface::GetInstance()->FocusObject(obj);
}