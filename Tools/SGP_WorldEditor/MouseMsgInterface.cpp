#include "stdafx.h"
#include "MouseMsgInterface.h"
#include "MouseMsgHandler.h"

#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "TerrainHeightHandler.h"
#include "TerrainTextureHandler.h"
#include "TerrainGrassHandler.h"
#include "TerrainWaterHandler.h"
#include "AddObjectHandler.h"
#include "SelectObjHandler.h"
#include "AddLightObjectHandler.h"
#include "CloneObjHandler.h"
#include "SceneObjectTree.h"
#include "MainFrame.h"

IMPLEMENT_SINGLETON(MouseMsgInterface)

MouseMsgInterface::MouseMsgInterface(void)
{
	m_CurrHandler=NULL;
	m_bRBtnDown=false;
	m_Hwnd=NULL;
}

MouseMsgInterface::~MouseMsgInterface(void)
{
}

void MouseMsgInterface::LButtonDown(CPoint pt,UINT nFlags)
{
	if(m_CurrHandler!=NULL) m_CurrHandler->LButtonDown(pt,nFlags);
}

void MouseMsgInterface::LButtonUp(CPoint pt,UINT nFlags)
{
	if(m_CurrHandler!=NULL) m_CurrHandler->LButtonUp(pt,nFlags);
}

void MouseMsgInterface::MouseMove(CPoint pt,UINT nFlags)
{
	if(WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain())
	{
		float x,y,z;
		WorldEditorRenderInterface::GetInstance()->GetIntersectPoint(x,z);
		y=WorldEditorRenderInterface::GetInstance()->GetIntersectHeight();
		
		CString str;
		str.Format("Current Pos : (%.1f,%.1f) , Current Height : %.2f",x,z,y);
		CMainFrame::GetInstance()->m_StatusBar.SetPaneText(0,str); 
	}
	else
	{
		CMainFrame::GetInstance()->m_StatusBar.SetPaneText(0,"Ready");
	}

	if(m_CurrHandler!=NULL) m_CurrHandler->MouseMove(pt,nFlags);
}

void MouseMsgInterface::RButtonDown(CPoint pt,UINT nFlags)
{
	m_bRBtnDown=true;

	if(m_CurrHandler!=NULL) m_CurrHandler->RButtonDown(pt,nFlags);
}

void MouseMsgInterface::RButtonUp(CPoint pt,UINT nFlags)
{
	m_bRBtnDown=false;

	if(m_CurrHandler!=NULL) m_CurrHandler->RButtonUp(pt,nFlags);
}

void MouseMsgInterface::DrawArea()
{
	if(m_bRBtnDown)
		WorldEditorRenderInterface::GetInstance()->RenderRotateCenter();

	if(m_CurrHandler!=NULL) m_CurrHandler->DrawArea();
}

void MouseMsgInterface::DrawFlag()
{
	if(m_CurrHandler!=NULL) m_CurrHandler->DrawFlag();
}

BOOL MouseMsgInterface::CursorInRenderWindow()
{
	if(m_Hwnd==NULL) return FALSE;
	CRect rc;
	::GetWindowRect(m_Hwnd,&rc);
	CPoint cursorPos;
	GetCursorPos(&cursorPos);
	return rc.PtInRect(cursorPos);
}

void MouseMsgInterface::CheckUncompleteOp()
{
	if(m_CurrHandler!=NULL) m_CurrHandler->CheckUncompleteOp();
}

void MouseMsgInterface::SwitchToHeightEditor()
{
	CheckUncompleteOp();
	m_CurrHandler = TerrainHeightHandler::GetInstance();
}

void MouseMsgInterface::SwitchToTextureEditor()
{
	CheckUncompleteOp();
	m_CurrHandler = TerrainTextureHandler::GetInstance();
}

void MouseMsgInterface::SwitchToGrassEditor()
{
	CheckUncompleteOp();
	m_CurrHandler = TerrainGrassHandler::GetInstance();
}

void MouseMsgInterface::SwitchToWaterEditor()
{
	CheckUncompleteOp();
	m_CurrHandler = TerrainWaterHandler::GetInstance();
}

void MouseMsgInterface::SwitchToAddObjEditor()
{
	CheckUncompleteOp();
	m_CurrHandler=AddObjectHandler::GetInstance();
}

void MouseMsgInterface::SwitchToSelectEditor()
{
	CheckUncompleteOp();
	CSceneObjectTree::GetInstance()->ClearSelection();
	m_CurrHandler=SelectObjHandler::GetInstance();

}

void MouseMsgInterface::SwitchToAddLightEditor()
{
	CheckUncompleteOp();
	m_CurrHandler=AddLightObjHandler::GetInstance();
}

void MouseMsgInterface::SwitchToCloneObjEditor()
{
	CheckUncompleteOp();
	m_CurrHandler=CloneObjHandler::GetInstance();
}

bool MouseMsgInterface::SetCursor()
{
	if(m_CurrHandler!=NULL) return m_CurrHandler->SetCursor();
	else return false;
}

bool MouseMsgInterface::IsSelectObjHandler()
{
	return m_CurrHandler==SelectObjHandler::GetInstance();
}

void MouseMsgInterface::StopInsertObjects()
{
	if(m_CurrHandler==AddObjectHandler::GetInstance()||m_CurrHandler==AddLightObjHandler::GetInstance()||\
		m_CurrHandler==CloneObjHandler::GetInstance())
	{
		SwitchToSelectEditor();
	}
}