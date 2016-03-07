// ImageCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "ImageCtrl.h"

#include "TerrainTextureDlg.h"
#include "WorldEditorConfig.h"
#include "TextureInfoDlg.h"
#include "TerrainTextureHandler.h"
#include "ThumbnailGenerator.h"
#include <io.h>

#define IDM_DELETE_TEXTURE 1000
// CImageCtrl

IMPLEMENT_DYNAMIC(CImageCtrl, CStatic)

CImageCtrl::CImageCtrl()
{
	m_ImagePath="";
	m_TexturePath="";
	m_bSel=false;
}

CImageCtrl::~CImageCtrl()
{
}

BEGIN_MESSAGE_MAP(CImageCtrl, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_DROPFILES()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_DELETE_TEXTURE,OnDeleteTexture)
END_MESSAGE_MAP()

// CImageCtrl 消息处理程序

void CImageCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	Graphics graph(lpDrawItemStruct->hDC);
	CRect rc=lpDrawItemStruct->rcItem;
	if(!m_ImagePath.IsEmpty())
	{
		CheckImageExist();
		BSTR imagePath=m_ImagePath.AllocSysString();
		Image image(imagePath);
		graph.DrawImage(&image,rc.left,rc.top,rc.Width(),rc.Height());
		Pen pen(Color(0,255,255),6);
		if(m_bSel) graph.DrawRectangle(&pen,rc.left,rc.top,rc.Width(),rc.Height());
		SysFreeString(imagePath);
	}
	else
	{
		SolidBrush brush(Color(255,255,255));
		graph.FillRectangle(&brush,rc.left,rc.top,rc.Width(),rc.Height());
		Pen pen(Color(0,255,255),6);
		if(m_bSel) graph.DrawRectangle(&pen,rc.left,rc.top,rc.Width(),rc.Height());
	}
}

void CImageCtrl::SetImage(CString imgPath,CString texPath)
{
	m_ImagePath=imgPath;
	m_TexturePath=texPath;
	Invalidate();
}

void CImageCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_nLayer>=VertexLayer0&&m_nLayer<=VertexLayer3)
		((CTerrainTextureDlg*)GetParent())->SetCurrentLayer(m_nLayer);
	CStatic::OnLButtonDown(nFlags, point);
}

void CImageCtrl::SetLayerSel(bool bSel)
{
	m_bSel=bSel;
	Invalidate();
}

void CImageCtrl::OnDropFiles(HDROP hDropInfo)
{
/*	UINT count;
	char filePath[200];

	count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if(count==1)
	{
		DragQueryFile(hDropInfo, 0, filePath, sizeof(filePath));
		CString texturePath=filePath;
		CString textureFolder=texturePath.Left(texturePath.ReverseFind('\\'));
		if(textureFolder==WorldEditorConfig::GetInstance()->GetTerrainTextureDir())
		{
			CString textureName=texturePath.Right(texturePath.GetLength()-texturePath.ReverseFind('\\')-1);
			CFileStatus status;
			CFile::GetStatus(texturePath,status);
			CString imageName;
			imageName.Format("%s-%d.bmp",textureName,status.m_size);
			CString imagePath=WorldEditorConfig::GetInstance()->GetTerrainTextureThumbnailDir()+'\\'+imageName;
			BOOL bExist=TRUE;
			if(_access((LPCTSTR)imagePath,0)==-1)// the image don't exist
			{
				ThumbnailGenerator generator(this);
				bExist=generator.GenerateThumbnail(texturePath,imagePath);
			}
			if(bExist)
			{
				SetImage(imagePath,texturePath);
				if(m_nLayer>=VertexLayer0&&m_nLayer<=VertexLayer3)
				{
					CString texRelativePath=WorldEditorConfig::GetInstance()->GetTerrainTextureRelativeDir()+"\\"+textureName;
					TerrainTextureHandler::GetInstance()->SetLayerImgPath(m_nLayer,texRelativePath);
				}
				if(m_nLayer>=ChunkLayer0&&m_nLayer<=ChunkSlopeMap)
				{
					CString texRelativePath=WorldEditorConfig::GetInstance()->GetTerrainTextureRelativeDir()+"\\"+textureName;
					TerrainTextureHandler::GetInstance()->SetChunkTexture(m_nLayer-ChunkLayer0,texRelativePath);
				}
			}	
		}
		else
			MessageBox("You can only use texture that is in terrain texture directory!","WorldEditor",MB_OK|MB_ICONSTOP);
	}

	DragFinish(hDropInfo);*/

	CStatic::OnDropFiles(hDropInfo);
}

void CImageCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(!m_TexturePath.IsEmpty())
	{
		CString textureName=m_TexturePath.Right(m_TexturePath.GetLength()-m_TexturePath.ReverseFind('\\')-1);
		CTextureInfoDlg dlg(m_TexturePath,textureName);
		dlg.DoModal();
	}

	CStatic::OnLButtonDblClk(nFlags, point);
}


void CImageCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(m_nLayer>=ChunkLayer1&&m_nLayer<=ChunkSlopeMap)
	{
		if((!m_TexturePath.IsEmpty())&&TerrainTextureHandler::GetInstance()->CanSetTexture())
		{
			CPoint pt;
			GetCursorPos(&pt);
			CMenu menu;
			menu.CreatePopupMenu();
			menu.AppendMenu(MF_STRING,IDM_DELETE_TEXTURE,"Delete Texture");
			menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
		}
	}

	CStatic::OnRButtonDown(nFlags, point);
}

void CImageCtrl::OnDeleteTexture()
{
	if(m_nLayer>=ChunkLayer1&&m_nLayer<=ChunkSlopeMap)
		TerrainTextureHandler::GetInstance()->SetChunkTexture(m_nLayer-ChunkLayer0,"");
}

void CImageCtrl::CheckImageExist()
{
	if(_access(m_ImagePath,0)==-1)// the image don't exist
	{
		ThumbnailGenerator generator(this);
		generator.GenerateThumbnail(m_TexturePath,m_ImagePath);
	}
}