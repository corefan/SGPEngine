// AnimProgressBar.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "AnimProgressBar.h"

#include "ModelControlDlg.h"
#include "CommonMacroDef.h"
// CAnimProgressBar

IMPLEMENT_DYNAMIC(CAnimProgressBar, CStatic)

CAnimProgressBar::CAnimProgressBar()
{
	m_CurrPos=0;
	m_bLBtnDown=FALSE;
}

CAnimProgressBar::~CAnimProgressBar()
{
}

BEGIN_MESSAGE_MAP(CAnimProgressBar, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CAnimProgressBar 消息处理程序

void CAnimProgressBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;
	GetClientRect(rect);
	m_CurrPos=point.x;
	if(m_CurrPos<=rect.Height()/2) m_CurrPos=rect.Height()/2;
	if(m_CurrPos>=rect.Width()-rect.Height()/2) m_CurrPos=rect.Width()-rect.Height()/2;
	Invalidate();

	m_bLBtnDown=TRUE;
	SetCapture();
	NotifyPosChanged();
	CStatic::OnLButtonDown(nFlags, point);
}

void CAnimProgressBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bLBtnDown&&(nFlags&MK_LBUTTON))
	{
		CRect rect;
		GetClientRect(rect);
		m_CurrPos=point.x;
		if(m_CurrPos<=rect.Height()/2) m_CurrPos=rect.Height()/2;
		if(m_CurrPos>=rect.Width()-rect.Height()/2) m_CurrPos=rect.Width()-rect.Height()/2;
		Invalidate();
		NotifyPosChanged();
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CAnimProgressBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bLBtnDown=FALSE;
	ReleaseCapture();
	CStatic::OnLButtonUp(nFlags, point);
}

int CAnimProgressBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rect;
	GetClientRect(rect);
	CString str;
	str.Format("%d%d\n%d%d",rect.left,rect.top,rect.Width(),rect.Height());
	MessageBox(str);

	return 0;
}

void CAnimProgressBar::SetCurrPos(float fPos)
{
	CRect rc;
	GetClientRect(rc);
	if(rc.Height()%2!=0) rc.bottom--;
	int length=(int)(fPos*(rc.Width()-rc.Height()));
	m_CurrPos=length+rc.Height()/2;
	Invalidate();
}

void CAnimProgressBar::NotifyPosChanged()
{
	CRect rc;
	GetClientRect(rc);
	if(rc.Height()%2!=0) rc.bottom--;
	float fPos=(float)(m_CurrPos-rc.Height()/2)/(rc.Width()-rc.Height());
	((CModelControlDlg*)GetParent())->NotifyCurrentFrame(fPos);
}

void CAnimProgressBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rect=lpDrawItemStruct->rcItem;
	if(rect.Height()%2!=0) rect.bottom--;
	if(m_CurrPos<=rect.Height()/2) m_CurrPos=rect.Height()/2;
	if(m_CurrPos>=rect.Width()-rect.Height()/2) m_CurrPos=rect.Width()-rect.Height()/2;

/*	Graphics graph(lpDrawItemStruct->hDC);
	graph.SetSmoothingMode(SmoothingModeHighQuality);

	SolidBrush brushGround(Color(DLG_BKCOLOR_RED,DLG_BKCOLOR_GREEN,DLG_BKCOLOR_BLUE));
	graph.FillRectangle(&brushGround,rect.left,rect.top,rect.Width(),rect.Height());

	Color colBk,colProcess,colPos;
	if(lpDrawItemStruct->itemState&ODS_DISABLED)//the processbar is disabled
	{
		colBk.SetFromCOLORREF(RGB(201,199,186));
		colProcess.SetFromCOLORREF(RGB(139,190,0));
		colPos.SetFromCOLORREF(RGB(180,180,63));
	}
	else//the window is in normal state
	{
		colBk.SetFromCOLORREF(RGB(180,180,180));
		colProcess.SetFromCOLORREF(RGB(119,160,0));
		colPos.SetFromCOLORREF(RGB(200,211,63));
	}

	SolidBrush brushBk(colBk);
	graph.FillRectangle(&brushBk,m_CurrPos,rect.top,rect.Width()-m_CurrPos-rect.Height()/2,rect.Height());
	graph.FillEllipse(&brushBk,rect.right-rect.Height(),rect.top,rect.Height(),rect.Height());

	SolidBrush brushProcess(colProcess);
	graph.FillEllipse(&brushProcess,rect.left,rect.top,rect.Height(),rect.Height());
	graph.FillRectangle(&brushProcess,rect.left+rect.Height()/2,rect.top,m_CurrPos-rect.Height()/2,rect.Height());

	SolidBrush brushPos(colPos);
	graph.FillEllipse(&brushPos,m_CurrPos-rect.Height()/2,rect.top,rect.Height(),rect.Height());*/

	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	CDC memDC;
	CBitmap memBmp;
	memDC.CreateCompatibleDC(pDC);
	memBmp.CreateCompatibleBitmap(pDC,rect.Width()+2,rect.Height()+2);
	memDC.SelectObject(&memBmp);

	Graphics graph(memDC.m_hDC);
	graph.SetSmoothingMode(SmoothingModeHighQuality);

//	SolidBrush brushGround(Color(DLG_BKCOLOR_RED,DLG_BKCOLOR_GREEN,DLG_BKCOLOR_BLUE));
	DWORD color=GetSysColor(COLOR_BTNFACE);
	SolidBrush brushGround(Color(GetRValue(color),GetGValue(color),GetBValue(color)));
	graph.FillRectangle(&brushGround,rect.left,rect.top,rect.Width()+2,rect.Height()+2);

	rect.OffsetRect(1,1);

	SolidBrush brushBk(Color(180,180,180));
	if(lpDrawItemStruct->itemState&ODS_DISABLED)//the processbar is disabled
	{
		brushBk.SetColor(Color(201,199,186));
	}
	graph.FillRectangle(&brushBk,m_CurrPos,rect.top,rect.Width()-m_CurrPos-rect.Height()/2,rect.Height());
	graph.FillEllipse(&brushBk,rect.right-rect.Height(),rect.top,rect.Height(),rect.Height());
	
	SolidBrush brushProcess(Color(119,160,0));
	if(lpDrawItemStruct->itemState&ODS_DISABLED)//the processbar is disabled
	{
		brushProcess.SetColor(Color(139,190,0));
	}
	graph.FillEllipse(&brushProcess,rect.left,rect.top,rect.Height(),rect.Height());
	graph.FillRectangle(&brushProcess,rect.left+rect.Height()/2,rect.top,m_CurrPos-rect.Height()/2,rect.Height());

	SolidBrush brushPos(Color(200,211,63));
	if(lpDrawItemStruct->itemState&ODS_DISABLED)//the processbar is disabled
	{
		brushPos.SetColor(Color(180,180,63));
	}
	graph.FillEllipse(&brushPos,m_CurrPos-rect.Height()/2,rect.top,rect.Height(),rect.Height());
	
	rect.OffsetRect(-1,-1);

	pDC->BitBlt(rect.left,rect.top,rect.Width()+1,rect.Height()+1,&memDC,rect.left+1,rect.top+1,SRCCOPY);
}
