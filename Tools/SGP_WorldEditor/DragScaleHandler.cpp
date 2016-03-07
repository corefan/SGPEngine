#include "stdafx.h"
#include "WorldEditor.h"
#include "DragScaleHandler.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "SelectObjHandler.h"
#include "Render Interface\WorldEditorRenderInterface.h"
#include "SceneObjectManager.h"
#include "HelpFunction.h"
#include "ObjAttributePane.h"
#include "ObjAttributeOperation.h"
#include "OperationManager.h"

IMPLEMENT_SINGLETON(DragScaleHandler)

DragScaleHandler::DragScaleHandler()
{
	m_pAxis=NULL;
	m_pObjAttrOp=NULL;
}

DragScaleHandler::~DragScaleHandler()
{

}

void DragScaleHandler::BeginDrag(CPoint pt)
{
	ASSERT(m_pObjAttrOp==NULL);
	m_LastPtPos=pt;
}

void DragScaleHandler::DragMove(CPoint pt)
{
	if(pt.x!=m_LastPtPos.x||pt.y!=m_LastPtPos.y)
	{
		std::vector<CommonObject>& objVector=SelectObjHandler::GetInstance()->GetSelectedObj();
		if(m_pObjAttrOp==NULL)
		{
			if(ContainMF1(objVector))
			{
				m_pObjAttrOp = new ObjAttributeOperation;
				m_pObjAttrOp->AddMF1Objects(objVector);
				m_pObjAttrOp->SaveAllPrevState();
			}
		}
		
		Vector2D dir=m_pAxis->Get2DEnd()-m_pAxis->Get2DStart();
		float dirLen=dir.GetLength();
		float deltaX=(float)(pt.x-m_LastPtPos.x);
		float deltaY=(float)(pt.y-m_LastPtPos.y);
		float dotLen=(dir.x*deltaX+dir.y*deltaY)/dirLen;
		float ratio=dotLen/dirLen;
		ratio=1.0f+ratio;
		if(ratio<0.2f) ratio=0.2f;
		
		SceneObjectManager::GetInstance()->ScaleSceneObject(objVector,ratio,FALSE);
		CObjAttributePane::GetInstance()->NotifyScaleChanged(objVector);

		m_LastPtPos=pt;
	}
}

void DragScaleHandler::EndDrag(CPoint pt)
{
	if(m_pObjAttrOp!=NULL)
	{
		std::vector<CommonObject>& objVector=SelectObjHandler::GetInstance()->GetSelectedObj();
		if(ContainMF1(objVector))
			WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();

		m_pObjAttrOp->SaveAllCurrState();
		OperationManager::GetInstance()->AddNewOperation(m_pObjAttrOp);
		m_pObjAttrOp = NULL;
	}
}