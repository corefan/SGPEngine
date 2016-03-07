// ImageStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "ImageStatic.h"

#include "TextureInfoDlg.h"
#include "ImageShowDlg.h"
// CImageStatic

IMPLEMENT_DYNAMIC(CImageStatic, CStatic)

CImageStatic::CImageStatic()
{
	m_bLbtnDown=FALSE;
}

CImageStatic::~CImageStatic()
{
}

BEGIN_MESSAGE_MAP(CImageStatic, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CImageStatic 消息处理程序

void CImageStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	CTextureInfoDlg* pDlg=(CTextureInfoDlg*)(GetParent()->GetParent());
	pDlg->DrawTexture(pDC);
}

void CImageStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
//	m_PrevPos=point;
	GetCursorPos(&m_PrevPos);
	GetParent()->GetParent()->ScreenToClient(&m_PrevPos);

	m_bLbtnDown=TRUE;
	SetCapture();

	CStatic::OnLButtonDown(nFlags, point);
}

void CImageStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
/*	if(m_bLbtnDown)
	{
		((CImageShowDlg*)GetParent())->SliderScroll(point.x-m_PrevPos.x,point.y-m_PrevPos.y);
		m_PrevPos=point;
	}*/

	m_bLbtnDown=FALSE;
	ReleaseCapture();

	CStatic::OnLButtonUp(nFlags, point);
}


void CImageStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bLbtnDown&&(nFlags&MK_LBUTTON))
	{
		CPoint pt;
		GetCursorPos(&pt);
		GetParent()->GetParent()->ScreenToClient(&pt);
		if(pt.x-m_PrevPos.x>=2||pt.x-m_PrevPos.x<=-2||pt.y-m_PrevPos.y>=2||pt.y-m_PrevPos.y<=-2)
		{
			((CImageShowDlg*)GetParent())->SliderScroll(pt.x-m_PrevPos.x,pt.y-m_PrevPos.y);
			m_PrevPos=pt;
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}
