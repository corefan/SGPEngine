// ResourceTreeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ResourceTreeCtrl.h"
#include "ResourcePane.h"
// CResourceTreeCtrl
#include "ModelViewerRenderInterface.h"
#include "ModelControlDlg.h"
#include "ModelDetailPane.h"
#include "TextureInfoDlg.h"
#include "MainFrame.h"

#include <io.h>

IMPLEMENT_DYNAMIC(CResourceTreeCtrl, CTreeCtrl)

CResourceTreeCtrl::CResourceTreeCtrl():\
	m_PrevModel("")
{
}

CResourceTreeCtrl::~CResourceTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CResourceTreeCtrl, CTreeCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, &CResourceTreeCtrl::OnTvnItemexpanding)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CResourceTreeCtrl::OnNMDblclk)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_OPEN_FOLDER,OnMenuOpenFolder)
END_MESSAGE_MAP()

// CResourceTreeCtrl 消息处理程序

int CResourceTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	CBitmap bmp;
	bmp.LoadBitmap(IDB_RESOURCE_BMP);
	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);
	UINT nFlags=ILC_MASK|ILC_COLOR24;
	m_ImageList.Create(16,bmpObj.bmHeight,nFlags,0,0);
	m_ImageList.Add(&bmp,RGB(255,0,255));
	SetImageList(&m_ImageList, TVSIL_NORMAL);

	Insert(m_DirPath);

	m_Font.CreatePointFont(95,_T("微软雅黑"));
	SetFont(&m_Font);

	ModifyStyle(0,TVS_SHOWSELALWAYS);

	return 0;
}

void CResourceTreeCtrl::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	TVITEM tvItem=pNMTreeView->itemNew;
	if(pNMTreeView->action==TVE_EXPAND)//expand a node
	{
		DeleteChild(tvItem.hItem);
		Insert(GetItemPath(tvItem.hItem),tvItem.hItem);
		SetItemImage(tvItem.hItem,0,0);
	}
	if(pNMTreeView->action==TVE_COLLAPSE)
	{
		SetItemImage(tvItem.hItem,1,1);
	}

	*pResult = 0;
}

void CResourceTreeCtrl::SetPath(CString strPath)
{
	m_DirPath=strPath;
}

void CResourceTreeCtrl::Insert(CString path,HTREEITEM hItem)
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
					CResourcePane* pPane=(CResourcePane*)(GetParent()->GetParent());
					std::vector<CString>& v=pPane->m_FileFilter;
					CString fileName=subFinder.GetFileName();
					int pos=fileName.ReverseFind('.');
					if(pos!=-1)
					{
						CString fileExt=fileName.Right(fileName.GetLength()-pos).MakeLower();
						int num=v.size();
						for(int i=0;i<num;++i)
						{
							if(fileExt==v[i])
							{
								bExisted=TRUE;
								break;
							}
						}
					}
				}
				if(bExisted) break;
			}
			HTREEITEM hNewItem=MyInsertItem(finder.GetFileName(),hItem,bExisted,1);
			SetItemData(hNewItem,(DWORD_PTR)1);
		}
		else//file
		{
			CResourcePane* pPane=(CResourcePane*)(GetParent()->GetParent());
			std::vector<CString>& v=pPane->m_FileFilter;
			CString fileName=finder.GetFileName();
			int pos=fileName.ReverseFind('.');
			if(pos!=-1)
			{
				CString fileExt=fileName.Right(fileName.GetLength()-pos).MakeLower();
				int num=v.size();
				for(int i=0;i<num;++i)
				{
					if(v[i]==fileExt)
					{
						int imgIndex=0;

						if(fileExt==".mf1") imgIndex=3;
						else if(fileExt==".bf1") imgIndex=4;
						else if(fileExt==".dds") imgIndex=5;
						else if(fileExt==".tga") imgIndex=6;
						else imgIndex=2;//normal file,no specific icon
						HTREEITEM hNewItem=MyInsertItem(finder.GetFileName(),hItem,FALSE,imgIndex);
						SetItemData(hNewItem,(DWORD_PTR)0);
						break;
					}
				}
			}
		}
	}
}

HTREEITEM CResourceTreeCtrl::MyInsertItem(CString text,HTREEITEM hParent,BOOL bExpand,int index)
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

void CResourceTreeCtrl::DeleteChild(HTREEITEM hItem)
{
	CString strPath=GetItemPath(hItem);
	HTREEITEM childItem=GetChildItem(hItem);
	while(childItem!=NULL)
	{
		HTREEITEM hNext=GetNextItem(childItem,TVGN_NEXT);
		DeleteItem(childItem);
		childItem=hNext;
	}
}

CString CResourceTreeCtrl::GetItemPath(HTREEITEM hItem)
{
/*	HTREEITEM hParentItem=GetParentItem(hItem);
	if(hParentItem==NULL)
		return m_DirPath+GetItemText(hItem);
	else
		return GetItemPath(hParentItem)+"\\"+GetItemText(hItem);*/
	CString strPath=GetItemText(hItem);

	while((hItem=GetParentItem(hItem))!=NULL)
		strPath = GetItemText(hItem) + _T("\\") + strPath;
	strPath = m_DirPath + strPath;

	return strPath;
}

void CResourceTreeCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
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

	CString str=GetItemText(hItem);
	int index=-1;
	if((index=str.ReverseFind('.'))==-1) return ;

	//get the file extension
	CString strExt=str.Right(str.GetLength()-index);
	strExt.MakeLower();
	if(strExt==_T(".mf1"))
	{
		while(hItem!=NULL)
		{
			hItem=GetParentItem(hItem);
			if(hItem!=NULL) str=GetItemText(hItem)+"\\"+str;
		}
		str=m_DirPath+str;//retrieve absolute path
		
		OpenMF1File(str,GetItemText(GetSelectedItem()));
	}
	else if(strExt==_T(".tga"))
	{
		//display tga info
		CTextureInfoDlg dlg(GetItemPath(GetSelectedItem()),GetItemText(GetSelectedItem()));
		dlg.DoModal();
	}
	else if(strExt==_T(".dds"))
	{
		//display dds info
		CTextureInfoDlg dlg(GetItemPath(GetSelectedItem()),GetItemText(GetSelectedItem()));
		dlg.DoModal();
	}
	else if(strExt==_T(".bf1"))
	{
		ModelViewerRenderInterface::GetInstance()->ShowErrorLog(CString("Bone Files(.bf1) can't be opened!"));
	}
}

void CResourceTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
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
		menu.LoadMenu(IDR_RESOURE_MENU);
		CMenu *pSubMenu=menu.GetSubMenu(0);
		ClientToScreen(&point);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
	/*	UINT nMenuResult=afxContextMenuManager->TrackPopupMenu(pSubMenu->m_hMenu, point.x, point.y, this);
		if(nMenuResult==IDM_OPEN_FOLDER)
			OnMenuOpenFolder();*/
	}

	CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CResourceTreeCtrl::OnMenuOpenFolder()
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

void CResourceTreeCtrl::OpenMF1File(CString szPath,CString strName)
{
	// try to find bone file
	CString strBone=szPath.Left(szPath.ReverseFind('.'));
	strBone+=".bf1";
	BOOL bExist;
	if(_access((LPCTSTR)strBone,0)!=-1)
		bExist=TRUE;
	else
		bExist=FALSE;
	
	((CMainFrame*)AfxGetMainWnd())->PreCloseModel();
	//change model
	WCHAR wPath[MAX_PATH];
	wmemset(wPath,0,MAX_PATH);
	MultiByteToWideChar(CP_ACP,0,(LPCTSTR)szPath,szPath.GetLength(),wPath,MAX_PATH);
	ModelViewerRenderInterface::GetInstance()->ChangeModel(wPath,bExist);
	//show log
	ModelViewerRenderInterface::GetInstance()->ShowInfoLog(CString("Open ")\
		+strName+CString(" succeed"));
	CModelControlDlg::NotifyModelChanged(bExist);
	CModelDetailPane::NotifyModelChanged();

	((CMainFrame*)AfxGetMainWnd())->SetMF1File(szPath,strName);
	AfxGetMainWnd()->SetWindowText(strName+" - SGP_ModelViewer");
}