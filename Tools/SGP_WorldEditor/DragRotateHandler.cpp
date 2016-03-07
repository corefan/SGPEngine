#include "stdafx.h"
#include "WorldEditor.h"
#include "DragRotateHandler.h"
#include "SelectObjHandler.h"
#include <vector>
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "CommonObjDef.h"
#include "SceneObjectManager.h"
#include "HelpFunction.h"
#include "ObjAttributePane.h"
#include "ObjAttributeOperation.h"
#include "OperationManager.h"

IMPLEMENT_SINGLETON(DragRotateHandler)

DragRotateHandler::DragRotateHandler()
{
	m_pObjAttrOp = NULL;
}

DragRotateHandler::~DragRotateHandler()
{

}

void DragRotateHandler::BeginDrag(CPoint pt)
{
	ASSERT(m_pObjAttrOp==NULL);
//	::SetCursor(AfxGetApp()->LoadCursor(IDC_ROTATE_CUR));
	m_LastPtPos=pt;
}

void DragRotateHandler::DragMove(CPoint pt)
{
	if(pt.x!=m_LastPtPos.x||pt.y!=m_LastPtPos.y)
	{
		std::vector<CommonObject>& objVector=SelectObjHandler::GetInstance()->GetSelectedObj();
		
		if(m_pObjAttrOp==NULL)
		{
			m_pObjAttrOp=new ObjAttributeOperation;
			m_pObjAttrOp->AddMF1Objects(objVector);
			m_pObjAttrOp->SaveAllPrevState();
		}

		float deltaX=(float)(pt.x-m_LastPtPos.x);
		float deltaY=(float)(pt.y-m_LastPtPos.y);
		float dot=(m_DirX*deltaX+m_DirY*deltaY)*0.005f;

		
		float rotateData[3]={0.0f,0.0f,0.0f};
		rotateData[m_nAxisIndex]=dot;
		SceneObjectManager::GetInstance()->RotateSceneObject(objVector,rotateData[0],rotateData[1],rotateData[2],FALSE);
		CObjAttributePane::GetInstance()->NotifyRotationChanged(objVector);
		
		m_LastPtPos=pt;
	}
}

void DragRotateHandler::EndDrag(CPoint pt)
{
//	::SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));

	if(m_pObjAttrOp!=NULL)
	{
		std::vector<CommonObject>& objVector=SelectObjHandler::GetInstance()->GetSelectedObj();
		if(ContainMF1(objVector))
			WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();

		m_pObjAttrOp->SaveAllCurrState();
		OperationManager::GetInstance()->AddNewOperation(m_pObjAttrOp);
		m_pObjAttrOp=NULL;
	}
}
