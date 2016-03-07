// GrassImageCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "WaterImageCtrl.h"
#include "TextureInfoDlg.h"
#include "ThumbnailGenerator.h"
// CGrassImageCtrl

IMPLEMENT_DYNAMIC(CWaterImageCtrl, CStatic)

CWaterImageCtrl::CWaterImageCtrl()
{
}

CWaterImageCtrl::~CWaterImageCtrl()
{
}

BEGIN_MESSAGE_MAP(CWaterImageCtrl, CStatic)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CGrassImageCtrl 消息处理程序

void CWaterImageCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	Graphics graph(lpDrawItemStruct->hDC);
	CRect rc=lpDrawItemStruct->rcItem;
	if(m_ImgPath != "")
	{
		CheckExist();
		BSTR imagePath=m_ImgPath.AllocSysString();
		Image image(imagePath);
		graph.DrawImage(&image,rc.left,rc.top,rc.Width(),rc.Height());
		SysFreeString(imagePath);
	}
	else
	{
		SolidBrush brush(Color(255,255,255));
		graph.FillRectangle(&brush,rc.left,rc.top,rc.Width(),rc.Height());
	}
}

void CWaterImageCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(!m_TexPath.IsEmpty())
	{
		CString texName = m_TexPath.Right(m_TexPath.GetLength()-m_TexPath.ReverseFind('\\')-1);
		CTextureInfoDlg dlg(m_TexPath,texName);
		dlg.DoModal();
	}

	CStatic::OnLButtonDblClk(nFlags, point);
}

void CWaterImageCtrl::SetImage(CString imgPath,CString texPath)
{
	m_ImgPath = imgPath;
	m_TexPath = texPath;
	Invalidate();
}

void CWaterImageCtrl::CheckExist()
{
	if(!PathFileExists(m_ImgPath))
	{
		CRect rc;
		GetClientRect(rc);
		ThumbnailGenerator generator(this);
		generator.GenerateThumbnail(m_TexPath,m_ImgPath,rc.Width(),rc.Height());
	}
}