// SceneObjectTree.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "SceneObjectTree.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include <afxcontextmenumanager.h>
#include "SceneObjectManager.h"
#include "SelectObjHandler.h"
#include "MouseMsgInterface.h"
#include "CloneObjHandler.h"

CSceneObjectTree* CSceneObjectTree::s_pSceneObjTree=NULL;

// CSceneObjectTree

extern CSGP_WorldEditorApp theApp;

IMPLEMENT_DYNAMIC(CSceneObjectTree, CTreeCtrl)

CSceneObjectTree::CSceneObjectTree()
{
	m_hItemFirstSel=NULL;
}

CSceneObjectTree::~CSceneObjectTree()
{
}

BEGIN_MESSAGE_MAP(CSceneObjectTree, CTreeCtrl)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CSceneObjectTree::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CSceneObjectTree::OnNMDblclk)
END_MESSAGE_MAP()

#define IDM_DELETE_OBJECT 1000
#define IDM_RELOAD_OBJECT 1001
#define IDM_CLONE_OBJECT  1002
#define IDM_SHOW_MODEL    1003
#define IDM_SHOW_PROPERTY 1004

// CSceneObjectTree 消息处理程序

void CSceneObjectTree::Init()
{
	s_pSceneObjTree=this;

	CBitmap bmp;
	bmp.LoadBitmap(IDB_OBJICON);
	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);
	UINT nFlags=ILC_MASK|ILC_COLOR24;
	m_ImageList.Create(16,bmpObj.bmHeight,nFlags,0,0);
	m_ImageList.Add(&bmp,RGB(255,0,255));
	SetImageList(&m_ImageList, TVSIL_NORMAL);

	ModifyStyle(0,TVS_SHOWSELALWAYS);
	SetLineColor(RGB(141,178,230));
	m_Font.CreatePointFont(88,"微软雅黑");
	SetFont(&m_Font);

	ResetAllItems();
}

void CSceneObjectTree::ResetAllItems()
{
	DeleteAllItems();

	m_RootItem=InsertItem("Scene Objects",0,0);
	m_StaticMeshItem=NULL;
	m_LightItem=NULL;
//	m_StaticMeshItem=InsertItem("Static Mesh",1,1,m_RootItem);
//	m_LightItem=InsertItem("Lights",1,1,m_RootItem);
//	Expand(m_RootItem,TVE_EXPAND);
	ClearSelection();
	SelectItem(NULL);
	m_hItemFirstSel=NULL;
}

void CSceneObjectTree::AddSceneObject(const CommonObject& obj)
{
	Expand(m_RootItem,TVE_EXPAND);
	if(obj.IsMF1())
	{
		if(m_StaticMeshItem==NULL)
			m_StaticMeshItem=InsertItem("Static Mesh",1,1,m_RootItem);
		HTREEITEM hItem=InsertItem(obj.m_pObj->m_SceneObjectName,2,2,m_StaticMeshItem);
		SetItemData(hItem,(DWORD_PTR)(obj.m_pObj));
		Expand(m_StaticMeshItem,TVE_EXPAND);
		SortChildren(m_StaticMeshItem);
	}
	else if(obj.IsLight())
	{
		if(m_LightItem==NULL)
			m_LightItem=InsertItem("Lights",1,1,m_RootItem);
		HTREEITEM hItem=InsertItem(obj.m_pLightObj->m_SceneObjectName,3,3,m_LightItem);
		SetItemData(hItem,(DWORD_PTR)(obj.m_pLightObj));
		Expand(m_LightItem,TVE_EXPAND);
		SortChildren(m_LightItem);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CSceneObjectTree::DeleteSceneObject(const CommonObject& obj)
{
	HTREEITEM item=ItemFromObject(obj);
	if(item!=NULL) DeleteItem(item);
}

void CSceneObjectTree::SceneObjNameChanged(const CommonObject& obj)
{
	HTREEITEM item=ItemFromObject(obj);
	if(item!=NULL)
	{
		if(obj.IsMF1()) SetItemText(item,obj.m_pObj->m_SceneObjectName);
		else if(obj.IsLight()) SetItemText(item,obj.m_pLightObj->m_SceneObjectName);
		else ASSERT(FALSE);
	}
}

HTREEITEM CSceneObjectTree::ItemFromObject(const CommonObject& obj)
{
	if(obj.IsMF1())
	{
		DWORD_PTR dwData=(DWORD_PTR)(obj.m_pObj);
		HTREEITEM hItem=GetChildItem(m_StaticMeshItem);
		while(hItem!=NULL)
		{
			if(GetItemData(hItem)==dwData) return hItem;
			hItem=GetNextItem(hItem,TVGN_NEXT);
		}
	}
	else if(obj.IsLight())
	{
		DWORD_PTR dwData=(DWORD_PTR)(obj.m_pLightObj);
		HTREEITEM hItem=GetChildItem(m_LightItem);
		while(hItem!=NULL)
		{
			if(GetItemData(hItem)==dwData) return hItem;
			hItem=GetNextItem(hItem,TVGN_NEXT);
		}
	}
	ASSERT(FALSE);
	return NULL;
}

void CSceneObjectTree::OnRButtonDown(UINT nFlags, CPoint point)
{
	bool bHaveMF1,bHaveLight;
	std::vector<CommonObject> objVector;

	bool bSelectObjHandler=MouseMsgInterface::GetInstance()->IsSelectObjHandler();

	if(CanTrackPopupMenu(point,bHaveMF1,bHaveLight,objVector))
	{
		CMenu menu;
		menu.CreatePopupMenu();
		// Clone
		bool enableClone=(objVector.size()==1)&&bSelectObjHandler;
		if(enableClone) menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_CLONE_OBJECT,"Clone");
		else menu.AppendMenu(MF_STRING|MF_DISABLED|MF_GRAYED,IDM_CLONE_OBJECT,"Clone");
		menu.AppendMenu(MF_SEPARATOR);
		// Delete
		bool enableDelete=(objVector.size()>0)&&bSelectObjHandler;
		if(enableDelete) menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_DELETE_OBJECT,"Delete");
		else menu.AppendMenu(MF_STRING|MF_DISABLED|MF_GRAYED,IDM_DELETE_OBJECT,"Delete");
		menu.AppendMenu(MF_SEPARATOR);
		// Reload
		bool enableReload=(!bHaveLight)&&(objVector.size()>0)&&bSelectObjHandler;
		if(enableReload) menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_RELOAD_OBJECT,"Reload");
		else menu.AppendMenu(MF_STRING|MF_DISABLED|MF_GRAYED,IDM_RELOAD_OBJECT,"Reload");
		menu.AppendMenu(MF_SEPARATOR);
		// Open Model
		bool enableOpen=bHaveMF1&&(objVector.size()==1)&&bSelectObjHandler;
		if(enableOpen) menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_SHOW_MODEL,"Open Model");
		else menu.AppendMenu(MF_STRING|MF_DISABLED|MF_GRAYED,IDM_SHOW_MODEL,"Open Model");
		menu.AppendMenu(MF_SEPARATOR);
		// Property
		bool enableProperty=(objVector.size()==1)&&bSelectObjHandler;
		if(enableProperty) menu.AppendMenu(MF_STRING|MF_ENABLED,IDM_SHOW_PROPERTY,"Attributes");
		else menu.AppendMenu(MF_STRING|MF_DISABLED|MF_GRAYED,IDM_SHOW_PROPERTY,"Attributes");
		menu.AppendMenu(MF_SEPARATOR);

		CPoint pt=point;
		ClientToScreen(&pt);
		UINT nMenuResult=theApp.GetContextMenuManager()->TrackPopupMenu(menu.m_hMenu, pt.x, pt.y, this);
		switch(nMenuResult)
		{
		case IDM_CLONE_OBJECT:
			MouseMsgInterface::GetInstance()->SwitchToCloneObjEditor();
			CloneObjHandler::GetInstance()->BeginAdd(objVector[0]);
			break;
		case IDM_DELETE_OBJECT:
			SceneObjectManager::GetInstance()->DeleteObjectOperation(objVector);
			break;
		case IDM_RELOAD_OBJECT:
			SceneObjectManager::GetInstance()->ReloadSceneObject(objVector);
			break;
		case IDM_SHOW_MODEL:
			SceneObjectManager::GetInstance()->OpenSceneObject(objVector[0]);
			break;
		case IDM_SHOW_PROPERTY:
			SceneObjectManager::GetInstance()->ShowSceneObjectAttr(objVector[0]);
			break;
		}
	}

	CTreeCtrl::OnRButtonDown(nFlags, point);
}


void CSceneObjectTree::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(HitTest(point)==NULL) return ;

	if(nFlags&MK_CONTROL)
	{
		UINT flag;
		HTREEITEM hItem=HitTest(point,&flag);
		if(hItem==NULL) return;
		UINT uNewSelState=GetItemState(hItem,TVIS_SELECTED)&TVIS_SELECTED?0:TVIS_SELECTED;

		HTREEITEM hItemOld=GetSelectedItem();
		UINT uOldSelState=hItemOld?GetItemState(hItemOld,TVIS_SELECTED):0;

		if(GetSelectedItem()==hItem) SelectItem(NULL);

		CTreeCtrl::OnLButtonDown(nFlags, point);
		SetItemState(hItem, uNewSelState,  TVIS_SELECTED);
		if (hItemOld && hItemOld != hItem)
			SetItemState(hItemOld, uOldSelState, TVIS_SELECTED);
		m_hItemFirstSel = NULL;

		NotifyRetrieveSelectedItems();
		return;
	}
	else if(nFlags & MK_SHIFT)
	{
		UINT flag;
		HTREEITEM hItem = HitTest( point, &flag );
		if( !m_hItemFirstSel ) m_hItemFirstSel = GetSelectedItem();

		if( GetSelectedItem() == hItem ) SelectItem( NULL );
		CTreeCtrl::OnLButtonDown(nFlags, point);
		if( m_hItemFirstSel )
		{
			SelectItems( m_hItemFirstSel, hItem );
			NotifyRetrieveSelectedItems();
			return;
		}
	}
	else
	{
		ClearSelection();
		m_hItemFirstSel = NULL;
	}

	CTreeCtrl::OnLButtonDown(nFlags, point);
	HTREEITEM hItem=GetSelectedItem();
	SetItemState(hItem,TVIS_SELECTED,TVIS_SELECTED);
	NotifyRetrieveSelectedItems();
//	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CSceneObjectTree::ClearSelection(HTREEITEM hItem)
{
	if(GetItemState(hItem,TVIS_SELECTED)&TVIS_SELECTED)
		SetItemState(hItem,0,TVIS_SELECTED);
	if(ItemHasChildren(hItem))
	{
		HTREEITEM childItem=GetChildItem(hItem);
		while(childItem!=NULL)
		{
			ClearSelection(childItem);
			childItem=GetNextItem(childItem,TVGN_NEXT);
		}
	}
}

void CSceneObjectTree::ClearSelection()
{
	HTREEITEM hItem;
	for(hItem=GetRootItem();hItem!=NULL;hItem=GetNextItem(hItem,TVGN_NEXT))
	{
		ClearSelection(hItem);
	}
}

BOOL CSceneObjectTree::SelectItems(HTREEITEM hItemFrom,HTREEITEM hItemTo)
{
	HTREEITEM hItem=GetRootItem();
	while(hItem&&hItem!=hItemFrom&&hItem!=hItemTo)
	{
		hItem=GetNextVisibleItem(hItem);
		SetItemState(hItem,0,TVIS_SELECTED);
	}
	if(!hItem) return FALSE;
	SelectItem(hItemTo);

	if(hItem==hItemTo)
	{
		hItemTo=hItemFrom;
		hItemFrom=hItem;
	}

	BOOL bSelect=TRUE;
	while(hItem)
	{
		SetItemState(hItem,bSelect?TVIS_SELECTED:0,TVIS_SELECTED);
		if(hItem==hItemTo) bSelect=FALSE;
		hItem=GetNextVisibleItem(hItem);
	}
	return TRUE;
}

HTREEITEM CSceneObjectTree::GetFirstSelectedItem()
{
	return NULL;
}

void CSceneObjectTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( (nChar==VK_UP || nChar==VK_DOWN) && (GetKeyState( VK_SHIFT )&0x8000))
	{
		if(!m_hItemFirstSel)
		{
			m_hItemFirstSel=GetSelectedItem();
			ClearSelection();
		}
		HTREEITEM hItemPrevSel=GetSelectedItem();
		HTREEITEM hItemNext;
		if(nChar==VK_UP) hItemNext=GetPrevVisibleItem(hItemPrevSel);
		else hItemNext=GetNextVisibleItem(hItemPrevSel);
		
		if(hItemNext)
		{
			BOOL bReSelect=!(GetItemState(hItemNext,TVIS_SELECTED)&TVIS_SELECTED);
			SelectItem(hItemNext);
			if(bReSelect) SetItemState(hItemPrevSel,TVIS_SELECTED,TVIS_SELECTED);
		}
		NotifyRetrieveSelectedItems();
		return ;
	}
	else if( nChar >= VK_SPACE )
	{
		m_hItemFirstSel = NULL;
		ClearSelection();
	}
	NotifyRetrieveSelectedItems();

	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSceneObjectTree::SelectedObjectsChanged(const std::vector<CommonObject>& objVector)
{
	ClearSelection();
	HTREEITEM hLastItem=NULL;
	for(uint32 i=0;i<objVector.size();++i)
	{
		HTREEITEM hItem=ItemFromObject(objVector[i]);
		SetItemState(hItem,TVIS_SELECTED,TVIS_SELECTED);
		hLastItem=hItem;
	}
	if(hLastItem!=NULL) SelectItem(hLastItem);
}

void CSceneObjectTree::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	LPNMTVCUSTOMDRAW pDraw=(LPNMTVCUSTOMDRAW)pNMHDR;
	DWORD dwDrawStage=pDraw->nmcd.dwDrawStage;
	UINT uItemState =pDraw->nmcd.uItemState;

	*pResult=CDRF_NOTIFYITEMDRAW;
	 
	if(dwDrawStage & /*CDDS_ITEM*/CDDS_ITEMPOSTPAINT)
	{
		CDC* pdc=CDC::FromHandle(pDraw->nmcd.hdc);
		CRect rc;
		HTREEITEM hItem=(HTREEITEM) pDraw->nmcd.dwItemSpec;
		GetItemRect(hItem,&rc,TRUE);//FALSE);text only
		CString txt=GetItemText(hItem);
		if(uItemState & CDIS_SELECTED)
		{
			pdc->FillSolidRect(&rc,RGB(49,106,197));//clr);
			pdc->SetTextColor(RGB(255,255,255));//white
			pdc->SetBkColor(RGB(49,106,197));
			pdc->TextOut(rc.left+2,rc.top+2,txt);
		//	*pResult |= CDRF_SKIPDEFAULT;
		}
		else
		{
			pdc->FillSolidRect(&rc, GetSysColor(COLOR_WINDOW));
			pdc->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			pdc->TextOut(rc.left+2, rc.top+2, txt);
		}
	}
}

CommonObject CSceneObjectTree::ObjectFromItem(HTREEITEM hItem)
{
	if(GetParentItem(hItem)==m_StaticMeshItem)
	{
		ISGPObject* pObj=(ISGPObject*)GetItemData(hItem);
		return CommonObject(pObj);
	}
	else if(GetParentItem(hItem)==m_LightItem)
	{
		ISGPLightObject* pLightObj=(ISGPLightObject*)GetItemData(hItem);
		return CommonObject(pLightObj);
	}
	else
	{
		return CommonObject();
	}
}

void CSceneObjectTree::NotifyRetrieveSelectedItems()
{
	if(!MouseMsgInterface::GetInstance()->IsSelectObjHandler()) return ;

	std::vector<CommonObject> objVector;
	HTREEITEM hItem=GetRootItem();
	while(hItem!=NULL)
	{
		if(GetItemState(hItem,TVIS_SELECTED)&TVIS_SELECTED)
		{
			CommonObject obj=ObjectFromItem(hItem);
			if(!obj.IsNULL()) objVector.push_back(obj);
		}
		hItem=GetNextVisibleItem(hItem);
	}
	SelectObjHandler::GetInstance()->SetSelectedObjects(objVector);
}

bool CSceneObjectTree::CanTrackPopupMenu(CPoint pt,bool& bHaveMF1,bool& bHaveLight,std::vector<CommonObject>& objVector)
{
	bHaveMF1=false;
	bHaveLight=false;
	bool bEnable=false;
	HTREEITEM hItem=GetRootItem();
	while(hItem!=NULL)
	{
		if(GetItemState(hItem,TVIS_SELECTED)&TVIS_SELECTED)
		{
			CommonObject obj=ObjectFromItem(hItem);
			if(!obj.IsNULL()) objVector.push_back(obj);
			if(obj.IsMF1()) bHaveMF1=true;
			else if(obj.IsLight()) bHaveLight=true;

			CRect rc;
			GetItemRect(hItem,&rc,TRUE);
			if(rc.PtInRect(pt)) bEnable=true;
		}
		hItem=GetNextVisibleItem(hItem);
	}
	return bEnable;
}

bool CSceneObjectTree::SearchObject(CString strKeyWord)
{	
	bool bFind=false;
	strKeyWord.MakeLower();
	ClearSelection();
	HTREEITEM hItem;
	for(hItem=GetRootItem();hItem!=NULL;hItem=GetNextVisibleItem(hItem))
	{
		if(hItem==m_RootItem||hItem==m_StaticMeshItem||hItem==m_LightItem)
			continue;
		CString str=GetItemText(hItem);
		str.MakeLower();
		if(str.Find(strKeyWord)!=-1)
		{
			SetItemState(hItem,TVIS_SELECTED,TVIS_SELECTED);
			bFind=true;
		}
	}
	NotifyRetrieveSelectedItems();
	return bFind;
}

bool CSceneObjectTree::SearchObject(int id)
{
	if(id < 0) return false;

	bool bFind=false;
	ClearSelection();
	HTREEITEM hItem=GetChildItem(m_StaticMeshItem);
	while(hItem!=NULL)
	{
		ISGPObject* pObj=(ISGPObject*)GetItemData(hItem);
		if(pObj->m_iSceneID==(uint32)id)
		{
			SetItemState(hItem,TVIS_SELECTED,TVIS_SELECTED);
			bFind=true;
			break;
		}
		hItem=GetNextItem(hItem,TVGN_NEXT);
	}

	hItem=GetChildItem(m_LightItem);
	while(hItem!=NULL)
	{
		ISGPLightObject* pLightObj=(ISGPLightObject*)GetItemData(hItem);
		if(pLightObj->m_iLightID==(uint32)id)
		{
			SetItemState(hItem,TVIS_SELECTED,TVIS_SELECTED);
			bFind=true;
			break;
		}
		hItem=GetNextItem(hItem,TVGN_NEXT);
	}
	NotifyRetrieveSelectedItems();

	return bFind;
}

void CSceneObjectTree::FocusObject(const CommonObject& obj)
{
	HTREEITEM hItem=ItemFromObject(obj);
	if(hItem!=NULL)
	{
		SetItemState(hItem,TVIS_SELECTED,TVIS_SELECTED);
		SelectItem(hItem);
	}
}

void CSceneObjectTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	
	HTREEITEM hItem=GetSelectedItem();
	if(hItem==NULL) return ;

	//check whether the mouse is in item rect
	CPoint pt;
	CRect rect;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	GetItemRect(hItem,&rect,TRUE);
	rect.left-=18;
	if(!rect.PtInRect(pt)) return ;

	CommonObject obj=ObjectFromItem(hItem);
	if(obj.IsNULL()) return ;
	WorldEditorRenderInterface::GetInstance()->FocusObject(obj);
}
