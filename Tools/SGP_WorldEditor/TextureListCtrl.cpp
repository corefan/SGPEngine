// TextureListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "TextureListCtrl.h"

#include "TerrainTextureDlg.h"
#include "TextureInfoDlg.h"
#include "WorldEditorConfig.h"
#include "TerrainTextureHandler.h"
#include <io.h>
// CTextureListCtrl

#define IDM_SHOW_RELATED_CHUNK 1211

IMPLEMENT_DYNAMIC(CTextureListCtrl, CListCtrl)

CTextureListCtrl::CTextureListCtrl()
{
	m_bDraging=FALSE;
	m_nSelectedItem=-1;
	m_bHaveItem=FALSE;
}

CTextureListCtrl::~CTextureListCtrl()
{
}

BEGIN_MESSAGE_MAP(CTextureListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CTextureListCtrl::OnNMDblclk)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, &CTextureListCtrl::OnLvnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_SHOW_RELATED_CHUNK,OnShowRelatedChunk)
END_MESSAGE_MAP()

// CTextureListCtrl 消息处理程序

void CTextureListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonDown(nFlags, point);

	UINT uSelectedCount=GetSelectedCount();
	if(uSelectedCount==1)
	{
		m_nSelectedItem = GetNextItem(-1, LVNI_SELECTED);
		ASSERT(m_nSelectedItem != -1);

		CRect rc;
		if(GetItemRect(m_nSelectedItem,rc,LVIR_BOUNDS))
		{
			if(rc.PtInRect(point))
			{
				m_bHaveItem=TRUE;
			}
		}
	}
}

void CTextureListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);



	*pResult = 0;
}

void CTextureListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	UINT uSelectedCount = GetSelectedCount();
	if(uSelectedCount>0)
	{
		int nItem=GetNextItem(-1,LVNI_SELECTED);
		ASSERT(nItem!=-1);
		CRect rc;
		if(GetItemRect(nItem,rc,LVIR_BOUNDS))
		{
			if(rc.PtInRect(point))
			{
				CString fileName=GetItemText(nItem,0);
				CString filePath=WorldEditorConfig::GetInstance()->GetTerrainTextureDir()+"\\"+fileName;
				if(_access(filePath,0)!=-1)
				{
					CTextureInfoDlg dlg(filePath,fileName);
					dlg.DoModal();
				}
				
			}
		}
	}

	CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CTextureListCtrl::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if(m_bHaveItem&&m_nSelectedItem!=-1)
	{
		m_bDraging=TRUE;
		SetCapture();
		SetCursor(AfxGetApp()->LoadCursor(IDC_DRAG_CURSOR));
	}

	*pResult = 0;
}

void CTextureListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bDraging)
	{
		m_bDraging=FALSE;
		CTerrainTextureDlg* pTextureDlg=(CTerrainTextureDlg*)GetParent();
		CPoint pt;
		GetCursorPos(&pt);
		CRect rc;
		int i;
		for(i=0;i<4;++i)
		{
			
			pTextureDlg->m_LayerImageCtrl[i].GetWindowRect(rc);
			if(rc.PtInRect(pt))
			{
				CString sTextureName=GetItemText(m_nSelectedItem,0);
				DWORD textureSize=GetItemData(m_nSelectedItem);
				CString sImgName;
				sImgName.Format("%s-%lu.bmp",sTextureName,textureSize);
				pTextureDlg->SetLayerImage(i,sTextureName,sImgName);
				break;
			}
		}
		if(i==4&&TerrainTextureHandler::GetInstance()->CanSetTexture())
		{
			for(i=0;i<7;++i)
			{
				pTextureDlg->m_ChunkImageCtrl[i].GetWindowRect(rc);
				if(rc.PtInRect(pt))
				{
					CString sTextureName=GetItemText(m_nSelectedItem,0);
					DWORD textureSize=GetItemData(m_nSelectedItem);
					CString sImgName;
					sImgName.Format("%s-%lu.bmp",sTextureName,textureSize);
					pTextureDlg->SetChunkImage(i,sTextureName,sImgName);
					break;
				}
			}
		}
		m_bHaveItem=FALSE;
		m_nSelectedItem=-1;
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		ReleaseCapture();
	}

	CListCtrl::OnLButtonUp(nFlags, point);
}

void CTextureListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bDraging&&(!TerrainTextureHandler::GetInstance()->CanSetTexture()))
	{
		CPoint pt;
		int i;
		GetCursorPos(&pt);
		CTerrainTextureDlg* pTextureDlg=(CTerrainTextureDlg*)GetParent();
		for(i=0;i<7;++i)
		{
			CRect rc;
			pTextureDlg->m_ChunkImageCtrl[i].GetWindowRect(rc);
			if(rc.PtInRect(pt))
			{
				SetCursor(LoadCursor(NULL,IDC_NO));
				break;
			}
		}
		if(i == 7) SetCursor(AfxGetApp()->LoadCursor(IDC_DRAG_CURSOR));
	}

	CListCtrl::OnMouseMove(nFlags, point);
}


void CTextureListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnRButtonDown(nFlags, point);
	
	BOOL bInArea =FALSE;
	UINT uSelectedCount=GetSelectedCount();
	if(uSelectedCount==1)
	{
		m_nSelectedItem = GetNextItem(-1, LVNI_SELECTED);
		ASSERT(m_nSelectedItem != -1);

		CRect rc;
		if(GetItemRect(m_nSelectedItem,rc,LVIR_BOUNDS))
		{
			if(rc.PtInRect(point))
			{
				bInArea = TRUE;
			}
		}
	}

	if(bInArea)
	{
		CPoint pt;
		GetCursorPos(&pt);
		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING,IDM_SHOW_RELATED_CHUNK,"Show Related Chunk");
		menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
	}
	
}

void CTextureListCtrl::OnShowRelatedChunk()
{
	CString texPath = WorldEditorConfig::GetInstance()->GetTerrainTextureRelativeDir()\
		+"\\"+GetItemText(m_nSelectedItem,0);
	TerrainTextureHandler::GetInstance()->RetrieveRelatedChunk(texPath);
}