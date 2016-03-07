#include "stdafx.h"
#include "WorldEditor.h"
#include "DragMoveHandler.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "SelectObjHandler.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "CommonObjDef.h"
#include "SceneObjectManager.h"
#include "HelpFunction.h"
#include "ObjAttributePane.h"
#include "ObjAttributeOperation.h"
#include "OperationManager.h"

IMPLEMENT_SINGLETON(DragMoveHandler)

DragMoveHandler::DragMoveHandler()
{
	m_pAxis=NULL;
	m_pObjAttrOp=NULL;
}

DragMoveHandler::~DragMoveHandler()
{

}

void DragMoveHandler::SetAxis(EditorExis* axis)
{
	m_pAxis=axis;
}

void DragMoveHandler::BeginDrag(CPoint pt)
{
	ASSERT(m_pObjAttrOp==NULL);
//	::SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE_CUR));
	m_LastPtPos=pt;
}

void DragMoveHandler::DragMove(CPoint pt)
{
	if(pt.x!=m_LastPtPos.x||pt.y!=m_LastPtPos.y)
	{
		std::vector<CommonObject>& objVector=SelectObjHandler::GetInstance()->GetSelectedObj();
		if(m_pObjAttrOp==NULL)
		{
			m_pObjAttrOp = new ObjAttributeOperation;
			m_pObjAttrOp->AddObjects(objVector);
			m_pObjAttrOp->SaveAllPrevState();
		}

		Vector2D dir=m_pAxis->Get2DEnd()-m_pAxis->Get2DStart();
		float dirLen=dir.GetLength();
		float deltaX=(float)(pt.x-m_LastPtPos.x);
		float deltaY=(float)(pt.y-m_LastPtPos.y);
		float dotLen=(dir.x*deltaX+dir.y*deltaY)/dirLen;
		float ratio=dotLen/dirLen;
		Vector3D dragVector=(m_pAxis->Get3DEnd()-m_pAxis->Get3DStart())*ratio;
		
		SceneObjectManager::GetInstance()->MoveSceneObject(objVector,dragVector.x,dragVector.y,dragVector.z,FALSE);
		CObjAttributePane::GetInstance()->NotifyPositionChanged(objVector);
		m_LastPtPos=pt;
	}
}

void DragMoveHandler::EndDrag(CPoint pt)
{
//	::SetCursor(::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));

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

