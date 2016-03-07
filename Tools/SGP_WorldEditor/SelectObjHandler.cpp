#include "stdafx.h"
#include "WorldEditor.h"
#include "SelectObjHandler.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include <math.h>
#include "ObjAttributePane.h"
#include "DragMoveHandler.h"
#include "DragRotateHandler.h"
#include "DragScaleHandler.h"
#include "HelpFunction.h"
#include "SceneObjectManager.h"
#include "SceneObjectTree.h"
#include "WorldMapManager.h"

IMPLEMENT_SINGLETON(SelectObjHandler)

SelectObjHandler::SelectObjHandler()
{
	m_bLBtnDown=false;
	m_pCurrAxis=NULL;
	m_pDragHandler=NULL;
	m_LastObj.SetNULL();

	m_XAxis.SetDirection(Vector3D(1.0f,0.0f,0.0f));
	m_YAxis.SetDirection(Vector3D(0.0f,1.0f,0.0f));
	m_ZAxis.SetDirection(Vector3D(0.0f,0.0f,1.0f));
	m_bInDragMode=false;
	m_DragMode=Drag_None;
	m_fRatio=1.0f;
}

SelectObjHandler::~SelectObjHandler()
{

}

void SelectObjHandler::LButtonDown(CPoint pt,UINT nFlags)
{
	m_bLBtnDown=true;
	m_BtnDownPt=pt;

	GetCurrAxis(pt);
	m_bInDragMode=(m_pDragHandler!=NULL);

	if(m_bInDragMode) m_pDragHandler->BeginDrag(pt);
}

void SelectObjHandler::LButtonUp(CPoint pt,UINT nFlags)
{
	if(m_bLBtnDown)
	{
		if(m_bInDragMode)
		{
			m_pDragHandler->EndDrag(pt);
		}
		else
		{
			m_Left=jmin(pt.x,m_BtnDownPt.x);
			m_Right=jmax(pt.x,m_BtnDownPt.x);
			m_Top=jmin(pt.y,m_BtnDownPt.y);
			m_Bottom=jmax(pt.y,m_BtnDownPt.y);
			
			if(m_Right-m_Left>2&&m_Bottom-m_Top>2)// Box Select
			{
				if(!(nFlags&MK_CONTROL))
				{
					m_SelObjectVector.clear();
					m_LastObj.SetNULL();
				}
				// Lights
				std::vector<ISGPLightObject*>& lightVec=WorldMapManager::GetInstance()->GetLightObjects();
				for(uint32 j=0;j<lightVec.size();++j)
				{
					OBBox obbox;
					GetObjectOBBox(CommonObject(lightVec[j]),obbox);
					if(BoxInRect(obbox,m_Left,m_Right,m_Top,m_Bottom))
					{
						m_SelObjectVector.push_back(CommonObject(lightVec[j]));
						m_LastObj=CommonObject(lightVec[j]);
					}
				}
				// Objects
				Array<ISGPObject*>& objArray=((COpenGLWorldSystemManager*)(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()))->getVisibleObjectArray();
				for(int i=0;i<objArray.size();++i)
				{
					if(BoxInRect(objArray[i]->getBoundingBox(),m_Left,m_Right,m_Top,m_Bottom))
					{
						m_SelObjectVector.push_back(CommonObject(objArray[i]));
						m_LastObj=CommonObject(objArray[i]);
					}
				}
				SelectedObjectsChanged();
			}
			else// Point Select
			{
				CommonObject obj;
				bool bFind=WorldEditorRenderInterface::GetInstance()->GetCurrPosObject(pt.x,pt.y,obj);
				if(nFlags&MK_CONTROL)
				{
					if(bFind)
					{
						std::vector<CommonObject>::iterator iter;
						for(iter=m_SelObjectVector.begin();iter!=m_SelObjectVector.end();++iter)
							if(*iter==obj) break;
						if(iter==m_SelObjectVector.end()) m_SelObjectVector.push_back(obj);
						else m_SelObjectVector.erase(iter);
					}
				}
				else
				{
					m_SelObjectVector.clear();
					if(bFind) m_SelObjectVector.push_back(obj);
				}

				if(!m_SelObjectVector.empty()) m_LastObj=m_SelObjectVector[m_SelObjectVector.size()-1];
				else m_LastObj.SetNULL();

				SelectedObjectsChanged();
			}
		}
	}

	m_pDragHandler=NULL;
	m_bInDragMode=false;
	m_bLBtnDown=false;
	m_Left=m_Right=m_Top=m_Bottom=0;
}

void SelectObjHandler::MouseMove(CPoint pt,UINT nFlags)
{
	if((nFlags&MK_LBUTTON)&&m_bLBtnDown)
	{
		if(m_bInDragMode)
		{
			m_pDragHandler->DragMove(pt);
		}
		else
		{
			m_Left=jmin(pt.x,m_BtnDownPt.x);
			m_Right=jmax(pt.x,m_BtnDownPt.x);
			m_Top=jmin(pt.y,m_BtnDownPt.y);
			m_Bottom=jmax(pt.y,m_BtnDownPt.y);
		}	
	}
	else
	{
		if(!m_bInDragMode) GetCurrAxis(pt);
	}
}

void SelectObjHandler::DrawArea()
{
	if(m_bLBtnDown&&m_Left!=m_Right&&m_Top!=m_Bottom)
		WorldEditorRenderInterface::GetInstance()->RenderBoxSelectArea(m_Left,m_Top,m_Right,m_Bottom,0.0f,1.0f,0.0f);

	for(uint32 i=0;i<m_SelObjectVector.size();++i)
	{
		OBBox obbox;
		GetObjectOBBox(m_SelObjectVector[i],obbox);
		AABBox aabbox;
		GetObjectAABox(m_SelObjectVector[i],aabbox);
		WorldEditorRenderInterface::GetInstance()->RenderOBBox(obbox,0,255,255);
	}
}

void SelectObjHandler::DrawFlag()
{
	if(!m_LastObj.IsNULL())
	{
		Vector3D center;
		if(m_LastObj.IsMF1())
			center.Set(m_LastObj.m_pObj->m_fPosition[0],m_LastObj.m_pObj->m_fPosition[1],m_LastObj.m_pObj->m_fPosition[2]);
		else if(m_LastObj.IsLight())
			center.Set(m_LastObj.m_pLightObj->m_fPosition[0],m_LastObj.m_pLightObj->m_fPosition[1],m_LastObj.m_pLightObj->m_fPosition[2]);
		else ASSERT(FALSE);

		m_XAxis.Set3DStart(center);
		m_YAxis.Set3DStart(center);
		m_ZAxis.Set3DStart(center);
		
		Vector4D CamPos;
		WorldEditorRenderInterface::GetInstance()->GetRenderDevice()->getCamreaPosition(&CamPos);
		Vector3D distancePos=Vector3D(CamPos.x,CamPos.y,CamPos.z) - center;
		float distance=distancePos.GetLength();
		m_fRatio=distance/50.0f;
		float length=10.0f*m_fRatio;
		m_XAxis.SetLength(length);
		m_YAxis.SetLength(length);
		m_ZAxis.SetLength(length);

		BOOL bDragFlag=FALSE;

		WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
		ISGPVertexCacheManager* pVertexManager=pRenderInterface->GetRenderDevice()->GetVertexCacheManager();
		if(m_DragMode==Drag_Move)
		{
			Matrix4x4 matX,matY,matZ;
			matX.Identity();
			matY.Identity();
			matZ.Identity();
			matX.RotationZ(-float_Pi/2);
			matZ.RotationX(float_Pi/2);
			matX.Translate(m_XAxis.Get3DEnd().x,m_XAxis.Get3DEnd().y,m_XAxis.Get3DEnd().z);
			matY.Translate(m_YAxis.Get3DEnd().x,m_YAxis.Get3DEnd().y,m_YAxis.Get3DEnd().z);
			matZ.Translate(m_ZAxis.Get3DEnd().x,m_ZAxis.Get3DEnd().y,m_ZAxis.Get3DEnd().z);
			
			float radius,height;
			radius=0.2f*m_fRatio;
			height=1.2f*m_fRatio;

			if(m_pCurrAxis==&m_XAxis)
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_XAxis.Get3DStart(),m_XAxis.Get3DEnd(),1.0f,1.0f,0.0f);
				pVertexManager->RenderCone(matX,radius,height,5,Colour(255,255,0));
			}
			else
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_XAxis.Get3DStart(),m_XAxis.Get3DEnd(),1.0f,0.0f,0.0f);
				pVertexManager->RenderCone(matX,radius,height,5,Colour(255,0,0));
			}
			if(m_pCurrAxis==&m_YAxis)
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_YAxis.Get3DStart(),m_YAxis.Get3DEnd(),1.0f,1.0f,0.0f);
				pVertexManager->RenderCone(matY,radius,height,5,Colour(255,255,0));
			}
			else
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_YAxis.Get3DStart(),m_YAxis.Get3DEnd(),0.0f,1.0f,0.0f);
				pVertexManager->RenderCone(matY,radius,height,5,Colour(0,255,0));
			}
			if(m_pCurrAxis==&m_ZAxis)
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_ZAxis.Get3DStart(),m_ZAxis.Get3DEnd(),1.0f,1.0f,0.0f);
				pVertexManager->RenderCone(matZ,radius,height,5,Colour(255,255,0));
			}
			else
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_ZAxis.Get3DStart(),m_ZAxis.Get3DEnd(),0.0f,0.0f,1.0f);
				pVertexManager->RenderCone(matZ,radius,height,5,Colour(0,0,255));
			}
			bDragFlag=TRUE;
		}
		else if(m_DragMode==Drag_Rotate&&m_LastObj.IsMF1())
		{
			bDragFlag=TRUE;
			float radius=6.0f*m_fRatio;
			if(m_pCurrAxis==&m_XAxis)
			{
				pRenderInterface->RenderLine(m_XAxis.Get3DStart(),m_XAxis.Get3DEnd(),1.0f,1.0f,0.0f);
				pVertexManager->RenderCircle(center,radius,Vector3D(1.0f,0.0f,0.0f),Colour(255,255,0));
			}
			else
			{
				pRenderInterface->RenderLine(m_XAxis.Get3DStart(),m_XAxis.Get3DEnd(),1.0f,0.0f,0.0f);
				pVertexManager->RenderCircle(center,radius,Vector3D(1.0f,0.0f,0.0f),Colour(255,0,0));
			}

			if(m_pCurrAxis==&m_YAxis)
			{
				pRenderInterface->RenderLine(m_YAxis.Get3DStart(),m_YAxis.Get3DEnd(),1.0f,1.0f,0.0f);
				pVertexManager->RenderCircle(center,radius,Vector3D(0.0f,1.0f,0.0f),Colour(255,255,0));
			}
			else
			{
				pRenderInterface->RenderLine(m_YAxis.Get3DStart(),m_YAxis.Get3DEnd(),0.0f,1.0f,0.0f);
				pVertexManager->RenderCircle(center,radius,Vector3D(0.0f,1.0f,0.0f),Colour(0,255,0));
			}

			if(m_pCurrAxis==&m_ZAxis)
			{
				pRenderInterface->RenderLine(m_ZAxis.Get3DStart(),m_ZAxis.Get3DEnd(),1.0f,1.0f,0.0f);
				pVertexManager->RenderCircle(center,radius,Vector3D(0.0f,0.0f,1.0f),Colour(255,255,0));
			}
			else
			{
				pRenderInterface->RenderLine(m_ZAxis.Get3DStart(),m_ZAxis.Get3DEnd(),0.0f,0.0f,1.0f);
				pVertexManager->RenderCircle(center,radius,Vector3D(0.0f,0.0f,1.0f),Colour(0,0,255));
			}
		}
		else if(m_DragMode==Drag_Scale&&m_LastObj.IsMF1())
		{
			bDragFlag=TRUE;
			float radius=0.2f*m_fRatio;
			if(m_pCurrAxis==&m_XAxis)
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_XAxis.Get3DStart(),m_XAxis.Get3DEnd(),1.0f,1.0f,0.0f);
				pVertexManager->RenderDetailSphere(m_XAxis.Get3DEnd(),radius,5,5,Colour(255,255,0));
			}
			else
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_XAxis.Get3DStart(),m_XAxis.Get3DEnd(),1.0f,0.0f,0.0f);
				pVertexManager->RenderDetailSphere(m_XAxis.Get3DEnd(),radius,5,5,Colour(255,0,0));
			}
			if(m_pCurrAxis==&m_YAxis)
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_YAxis.Get3DStart(),m_YAxis.Get3DEnd(),1.0f,1.0f,0.0f);
				pVertexManager->RenderDetailSphere(m_YAxis.Get3DEnd(),radius,5,5,Colour(255,255,0));
			}
			else
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_YAxis.Get3DStart(),m_YAxis.Get3DEnd(),0.0f,1.0f,0.0f);
				pVertexManager->RenderDetailSphere(m_YAxis.Get3DEnd(),radius,5,5,Colour(0,255,0));
			}
			if(m_pCurrAxis==&m_ZAxis)
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_ZAxis.Get3DStart(),m_ZAxis.Get3DEnd(),1.0f,1.0f,0.0f);
				pVertexManager->RenderDetailSphere(m_ZAxis.Get3DEnd(),radius,5,5,Colour(255,255,0));
			}
			else
			{
				WorldEditorRenderInterface::GetInstance()->RenderLine(m_ZAxis.Get3DStart(),m_ZAxis.Get3DEnd(),0.0f,0.0f,1.0f);
				pVertexManager->RenderDetailSphere(m_ZAxis.Get3DEnd(),radius,5,5,Colour(0,0,255));
			}
		}
		if(!bDragFlag)
		{			
			pRenderInterface->RenderLine(m_XAxis.Get3DStart(),m_XAxis.Get3DEnd(),1.0f,0.0f,0.0f);
			pRenderInterface->RenderLine(m_YAxis.Get3DStart(),m_YAxis.Get3DEnd(),0.0f,1.0f,0.0f);
			pRenderInterface->RenderLine(m_ZAxis.Get3DStart(),m_ZAxis.Get3DEnd(),0.0f,0.0f,1.0f);
		}
	}
}

bool SelectObjHandler::InLine(float x0,float y0,float x1,float y1,float x,float y)
{
	float length=sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
	float ray=sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0));
	float dot=(x-x0)*(x1-x0)+(y-y0)*(y1-y0);
	if(length<2) return false;
	dot=dot/length;
	if(dot<0.0f) return false;
	if(dot>length) return false;
	float distance=sqrt(ray*ray-dot*dot);

	if(distance<7) return true;
	else return false;
}

void SelectObjHandler::GetCurrAxis(CPoint pt)
{
	m_pCurrAxis=NULL;
	m_pDragHandler=NULL;
	if(!m_LastObj.IsNULL())
	{
		Vector3D center;
		if(m_LastObj.IsMF1())
			center.Set(m_LastObj.m_pObj->m_fPosition[0],m_LastObj.m_pObj->m_fPosition[1],m_LastObj.m_pObj->m_fPosition[2]);
		else if(m_LastObj.IsLight())
			center.Set(m_LastObj.m_pLightObj->m_fPosition[0],m_LastObj.m_pLightObj->m_fPosition[1],m_LastObj.m_pLightObj->m_fPosition[2]);
		else ASSERT(FALSE);
		m_XAxis.Set3DStart(center);
		m_YAxis.Set3DStart(center);
		m_ZAxis.Set3DStart(center);
		
		if(m_DragMode==Drag_Move)
		{
			m_XAxis.SetLength(m_fRatio*11.2f);
			m_YAxis.SetLength(m_fRatio*11.2f);
			m_ZAxis.SetLength(m_fRatio*11.2f);
			if(InLine(m_XAxis.Get2DStart().x,m_XAxis.Get2DStart().y,m_XAxis.Get2DEnd().x,m_XAxis.Get2DEnd().y,(float)pt.x,(float)pt.y))
				m_pCurrAxis=&m_XAxis;
			else if(InLine(m_YAxis.Get2DStart().x,m_YAxis.Get2DStart().y,m_YAxis.Get2DEnd().x,m_YAxis.Get2DEnd().y,(float)pt.x,(float)pt.y))
				m_pCurrAxis=&m_YAxis;
			else if(InLine(m_ZAxis.Get2DStart().x,m_ZAxis.Get2DStart().y,m_ZAxis.Get2DEnd().x,m_ZAxis.Get2DEnd().y,(float)pt.x,(float)pt.y))
				m_pCurrAxis=&m_ZAxis;
			if(m_pCurrAxis!=NULL)
			{
				m_pDragHandler=DragMoveHandler::GetInstance();
				DragMoveHandler::GetInstance()->SetAxis(m_pCurrAxis);
			}		
		}
		else if(m_DragMode==Drag_Rotate&&m_LastObj.IsMF1())
		{
			Vector4D vcOrig,vcDir;
			WorldEditorRenderInterface::GetInstance()->GetRenderDevice()->Transform2Dto3D(SDimension2D(pt.x,pt.y),&vcOrig,&vcDir);
			int nAxis;

			Vector3D startpos,endpos;
			if(vcDir.x>0.00001f||vcDir.x<-0.00001f)
			{
				float ratio=(center.x-vcOrig.x)/vcDir.x;
				float cross_y=vcOrig.y+ratio*vcDir.y;
				float cross_z=vcOrig.z+ratio*vcDir.z;
				float distance=sqrt((cross_y-center.y)*(cross_y-center.y)+(cross_z-center.z)*(cross_z-center.z));
				if(distance>5.5f*m_fRatio&&distance<6.5f*m_fRatio)
				{
					startpos.Set(center.x,cross_y,cross_z);
					endpos.Set(center.x,cross_y+(cross_z-center.z),cross_z+(center.y-cross_y));
					m_pCurrAxis=&m_XAxis;
					nAxis=0;
				}
			}

			if(m_pCurrAxis==NULL)
			{
				if(vcDir.y>0.00001f||vcDir.y<-0.00001f)
				{
					float ratio=(center.y-vcOrig.y)/vcDir.y;
					float cross_x=vcOrig.x+ratio*vcDir.x;
					float cross_z=vcOrig.z+ratio*vcDir.z;
					float distance=sqrt((cross_x-center.x)*(cross_x-center.x)+(cross_z-center.z)*(cross_z-center.z));
					if(distance>5.5f*m_fRatio&&distance<6.5f*m_fRatio)
					{
						startpos.Set(cross_x,center.y,cross_z);
						endpos.Set(cross_x+(center.z-cross_z),center.y,cross_z+(cross_x-center.x));
						m_pCurrAxis=&m_YAxis;
						nAxis=1;
					}
				}
			}
			if(m_pCurrAxis==NULL)
			{
				if(vcDir.z>0.00001f||vcDir.z<-0.00001f)
				{
					float ratio=(center.z-vcOrig.z)/vcDir.z;
					float cross_x=vcOrig.x+ratio*vcDir.x;
					float cross_y=vcOrig.y+ratio*vcDir.y;
					float distance=sqrt((cross_x-center.x)*(cross_x-center.x)+(cross_y-center.y)*(cross_y-center.y));
					if(distance>5.5f*m_fRatio&&distance<6.5f*m_fRatio)
					{
						startpos.Set(cross_x,cross_y,center.z);
						endpos.Set(cross_x+(cross_y-center.y),cross_y+(center.x-cross_x),center.z);
						m_pCurrAxis=&m_ZAxis;
						nAxis=2;
					}
				}
			}
			if(m_pCurrAxis!=NULL)
			{
				Vector2D start=Transform3DTo2D(startpos);
				Vector2D end=Transform3DTo2D(endpos);
				Vector2D v2d=end-start;
				v2d.Normalize();
				m_DirX=v2d.x;
				m_DirY=v2d.y;
				m_pDragHandler=DragRotateHandler::GetInstance();
				DragRotateHandler::GetInstance()->SetDir(m_DirX,m_DirY);
				DragRotateHandler::GetInstance()->SetAxis(nAxis);
			}
		}
		else if(m_DragMode==Drag_Scale&&m_LastObj.IsMF1())
		{
			m_XAxis.SetLength(m_fRatio*10.4f);
			m_YAxis.SetLength(m_fRatio*10.4f);
			m_ZAxis.SetLength(m_fRatio*10.4f);
			if(InLine(m_XAxis.Get2DStart().x,m_XAxis.Get2DStart().y,m_XAxis.Get2DEnd().x,m_XAxis.Get2DEnd().y,(float)pt.x,(float)pt.y))
				m_pCurrAxis=&m_XAxis;
			else if(InLine(m_YAxis.Get2DStart().x,m_YAxis.Get2DStart().y,m_YAxis.Get2DEnd().x,m_YAxis.Get2DEnd().y,(float)pt.x,(float)pt.y))
				m_pCurrAxis=&m_YAxis;
			else if(InLine(m_ZAxis.Get2DStart().x,m_ZAxis.Get2DStart().y,m_ZAxis.Get2DEnd().x,m_ZAxis.Get2DEnd().y,(float)pt.x,(float)pt.y))
				m_pCurrAxis=&m_ZAxis;
			if(m_pCurrAxis!=NULL)
			{
				m_pDragHandler=DragScaleHandler::GetInstance();
				DragScaleHandler::GetInstance()->SetAxis(m_pCurrAxis);
			}
		}
	}
}

void SelectObjHandler::CheckUncompleteOp()
{
	ClearSelection();
}

bool SelectObjHandler::SetCursor()
{
	if(m_pDragHandler==DragMoveHandler::GetInstance())
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE_CUR));
		return true;
	}
	if(m_pDragHandler==DragScaleHandler::GetInstance())
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_SCALE_CURSOR));
		return true;
	}
	if(m_pDragHandler==DragRotateHandler::GetInstance())
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_ROTATE_CUR));
		return true;
	}
	return false;
}

void SelectObjHandler::SetSelectedObjects(std::vector<CommonObject>& objVector)
{
	m_SelObjectVector.swap(objVector);
	if(m_SelObjectVector.size()!=0) m_LastObj=m_SelObjectVector[m_SelObjectVector.size()-1];
	else m_LastObj.SetNULL();
	CObjAttributePane::GetInstance()->LoadAttribute(m_LastObj);
	if(!m_LastObj.IsNULL()) CObjAttributePane::GetInstance()->ShowPane(TRUE,FALSE,FALSE);
}

void SelectObjHandler::SetLastObj(const CommonObject& obj)
{
	m_LastObj=obj;
	CObjAttributePane::GetInstance()->LoadAttribute(m_LastObj);
}

void SelectObjHandler::ClearSelection()
{
	m_SelObjectVector.clear();
	m_LastObj.SetNULL();
	SelectedObjectsChanged();
}

void SelectObjHandler::SelectedObjectsChanged()
{
	CSceneObjectTree::GetInstance()->SelectedObjectsChanged(m_SelObjectVector);
	CObjAttributePane::GetInstance()->LoadAttribute(m_LastObj);
	if(!m_LastObj.IsNULL()) CObjAttributePane::GetInstance()->ShowPane(TRUE,FALSE,FALSE);
}

void SelectObjHandler::FocusObject(const CommonObject& obj)
{
	m_SelObjectVector.clear();
	m_SelObjectVector.push_back(obj);
	m_LastObj=obj;
	CObjAttributePane::GetInstance()->LoadAttribute(m_LastObj);
	if(!m_LastObj.IsNULL()) CObjAttributePane::GetInstance()->ShowPane(TRUE,FALSE,FALSE);
}