// ObjectTreeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "ObjectTreeCtrl.h"
#include "MainFrame.h"
// CObjectTreeCtrl

#include "MouseMsgInterface.h"
#include "AddObjectHandler.h"
#include "MainFrame.h"
#include "WorldEditorView.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"

#define IDM_OPEN_FOLDER 1222
#define IDM_OPEN_MODEL  1223

extern CSGP_WorldEditorApp theApp;

IMPLEMENT_DYNAMIC(CObjectTreeCtrl, CTreeCtrl)

CObjectTreeCtrl::CObjectTreeCtrl()
{
	m_SelectedItem=NULL;
}

CObjectTreeCtrl::~CObjectTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CObjectTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, &CObjectTreeCtrl::OnTvnItemexpanding)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CObjectTreeCtrl::OnNMDblclk)
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CObjectTreeCtrl::OnTvnBegindrag)
	ON_COMMAND(IDM_OPEN_FOLDER,OnOpenFolder)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CObjectTreeCtrl 消息处理程序

void CObjectTreeCtrl::Init()
{
	CBitmap bmp;
	bmp.LoadBitmap(IDB_FILEICON);
	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);
	UINT nFlags=ILC_MASK|ILC_COLOR24;
	m_ImageList.Create(16,bmpObj.bmHeight,nFlags,0,0);
	m_ImageList.Add(&bmp,RGB(255,0,255));
	SetImageList(&m_ImageList, TVSIL_NORMAL);

	SetLineColor(RGB(141,178,230));

	Insert(m_DirPath);
}

void CObjectTreeCtrl::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	TVITEM tvItem=pNMTreeView->itemNew;
	if(pNMTreeView->action==TVE_EXPAND)//expand a node
	{
		DeleteChild(tvItem.hItem);
		Insert(GetItemPath(tvItem.hItem),tvItem.hItem);
		SetItemImage(tvItem.hItem,0,0);
	}
	if(pNMTreeView->action==TVE_COLLAPSE)
		SetItemImage(tvItem.hItem,1,1);

	*pResult = 0;
}

void CObjectTreeCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
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

	CString strPath = GetItemPath(hItem);
	int pos = strPath.ReverseFind('.');
	if(pos!=-1)
	{
		if(strPath.Right(strPath.GetLength()-pos).MakeLower()==".mf1")
			CMainFrame::OpenModelViewer(strPath);
	}
}

void CObjectTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	HTREEITEM hSelItem=HitTest(point);

	if(hSelItem!=NULL)
	{
		CRect rect;
		GetItemRect(hSelItem,&rect,TRUE);
		rect.left-=18;
		if(!rect.PtInRect(point)) return ;
		SelectItem(hSelItem);

		CMenu menu;
		menu.CreatePopupMenu();
		
		CString strText=GetItemText(hSelItem);
		CString strExt=PathFindExtension(strText);
		if(strExt.MakeLower()==".mf1")
		{
			menu.AppendMenu(MF_STRING,IDM_OPEN_MODEL,"Open Model");
			menu.AppendMenu(MF_SEPARATOR);
		}
		menu.AppendMenu(MF_STRING,IDM_OPEN_FOLDER,"Open Folder...");
		CPoint pt=point;
		ClientToScreen(&pt);
	/*	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);*/
		UINT nMenuResult=theApp.GetContextMenuManager()->TrackPopupMenu(menu.m_hMenu,pt.x,pt.y,this);
		switch(nMenuResult)
		{
		case IDM_OPEN_FOLDER:
			OnOpenFolder();
			break;
		case IDM_OPEN_MODEL:
			{
				CString filePath=GetItemPath(hSelItem);
				CMainFrame::OpenModelViewer(filePath);
			}
			break;
		}
	}

	CTreeCtrl::OnRButtonDown(nFlags, point);
}

HTREEITEM CObjectTreeCtrl::MyInsertItem(CString text,HTREEITEM hParent,BOOL bExpand,int index)
{
	TV_INSERTSTRUCT tvinsert;
	tvinsert.hParent=hParent;
	tvinsert.hInsertAfter=TVI_LAST;
	tvinsert.item.pszText=(LPTSTR)(LPCTSTR)text;
	if(bExpand)
	{
		tvinsert.item.mask=TVIF_TEXT|TVIF_CHILDREN|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		tvinsert.item.cChildren=1;
	}
	else
	{
		tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;

	}
	tvinsert.item.iImage=index;
	tvinsert.item.iSelectedImage=index;
	return InsertItem(&tvinsert);
}

void CObjectTreeCtrl::Insert(CString path,HTREEITEM hItem)
{
	CFileFind finder;
	BOOL bFinded=finder.FindFile(path+_T("\\*.*"));
	while(bFinded)
	{
		bFinded=finder.FindNextFile();
		if(finder.IsDots()||finder.IsHidden()||finder.IsSystem()) continue;
		if(finder.IsDirectory())//folder
		{
			CFileFind subFinder;
			BOOL bExisted=FALSE;
			BOOL bSubFinded=subFinder.FindFile(finder.GetFilePath()+_T("\\*.*"));
			while(bSubFinded)
			{
				bSubFinded=subFinder.FindNextFile();
				if(subFinder.IsDots()||subFinder.IsHidden()||subFinder.IsSystem()) continue;
				if(subFinder.IsDirectory())
				{
					bExisted=TRUE;
					break;
				}
				else
				{
					//check whether the supported files exist
					CString fileName=subFinder.GetFileName();
					int pos=fileName.ReverseFind('.');
					if(pos!=-1)
					{
						CString fileExt=fileName.Right(fileName.GetLength()-pos).MakeLower();
						if(fileExt==".mf1") bExisted=TRUE;
					}
				}
				if(bExisted) break;
			}
			HTREEITEM hNewItem=MyInsertItem(finder.GetFileName(),hItem,bExisted,1);
			SetItemData(hNewItem,(DWORD_PTR)1);
		}
		else//file
		{
			CString fileName=finder.GetFileName();
			int pos=fileName.ReverseFind('.');
			if(pos!=-1)
			{
				CString fileExt=fileName.Right(fileName.GetLength()-pos).MakeLower();
				if(fileExt==".mf1")
				{
					HTREEITEM hNewItem=MyInsertItem(finder.GetFileName(),hItem,FALSE,3);
					SetItemData(hNewItem,(DWORD_PTR)0);
				}
			}
		}
	}
}

void CObjectTreeCtrl::DeleteChild(HTREEITEM hItem)
{
	HTREEITEM childItem=GetChildItem(hItem);
	while(childItem!=NULL)
	{
		HTREEITEM hNext=GetNextItem(childItem,TVGN_NEXT);
		DeleteItem(childItem);
		childItem=hNext;
	}
}

CString CObjectTreeCtrl::GetItemPath(HTREEITEM hItem)
{
	CString strPath=GetItemText(hItem);

	while((hItem=GetParentItem(hItem))!=NULL)
		strPath = GetItemText(hItem) + _T("\\") + strPath;
	strPath = m_DirPath +"\\"+ strPath;

	return strPath;
}

void CObjectTreeCtrl::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
/*	if(m_SelectedItem!=NULL&&WorldEditorRenderInterface::GetInstance()->m_bHaveMap)
	{
		CString itemPath=GetItemPath(m_SelectedItem);
		CString fileExt=PathFindExtension(itemPath);
		if(fileExt.MakeLower()==".mf1")
		{
			CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
			CSGP_WorldEditorView* pView=(CSGP_WorldEditorView*)(pMainFrame->GetActiveView());
			MouseMsgInterface::GetInstance()->SwitchToAddObjEditor();
			AddObjectHandler::GetInstance()->AddSceneObject(itemPath);
			pView->OnLButtonDown(0,CPoint(0,0));
		}
	}*/

	*pResult = 0;
}

void CObjectTreeCtrl::OnOpenFolder()
{
	HTREEITEM hItem=GetSelectedItem();
	CString filePath=GetItemPath(hItem);
	if(GetItemData(hItem)==0)//file
	{
		CString strParameter="/Select, "+filePath;
		ShellExecute(NULL,_T("open"),_T("Explorer.exe"),strParameter,NULL,SW_SHOWDEFAULT);
		return ;
	}
	if(GetItemData(hItem)==1)//folder
	{
		ShellExecute(NULL,_T("open"),filePath,NULL,NULL,SW_SHOWDEFAULT);
		return ;
	}
}

void CObjectTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnLButtonDown(nFlags, point);

	m_SelectedItem=GetSelectedItem();
	
	CRect rect;
	GetItemRect(m_SelectedItem,&rect,TRUE);
	rect.left-=18;
	if(!rect.PtInRect(point)) m_SelectedItem=NULL ;
	
}

CString CObjectTreeCtrl::GetMF1FilePath()
{
	HTREEITEM hSelectedItem=GetSelectedItem();
	if(hSelectedItem==NULL) return CString("");

	CString itemPath=GetItemPath(hSelectedItem);
	CString fileExt=PathFindExtension(itemPath);
	if(fileExt.MakeLower()==".mf1") return itemPath;
	else return CString("");
}