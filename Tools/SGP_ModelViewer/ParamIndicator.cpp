// ParamIndicator.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ParamIndicator.h"

#include "CommonMacroDef.h"
// CParamIndicator

IMPLEMENT_DYNAMIC(CParamIndicator, CStatic)

CParamIndicator::CParamIndicator()
{
	m_CurPos=0.0f;
	m_bLButtonDown=FALSE;
}
CParamIndicator::~CParamIndicator()
{
}

BEGIN_MESSAGE_MAP(CParamIndicator, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CParamIndicator 消息处理程序

void CParamIndicator::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcClient=lpDrawItemStruct->rcItem;
//	rcClient.OffsetRect(1,1);

	CDC memDC;
	CBitmap bitmap;
	memDC.CreateCompatibleDC(pDC);
	bitmap.CreateCompatibleBitmap(pDC,rcClient.Width()+1,rcClient.Height()+1);
	memDC.SelectObject(&bitmap);

	Graphics graph(memDC.m_hDC);
	graph.SetSmoothingMode(SmoothingModeHighQuality);

	SolidBrush brushBk(Color(DLG_BKCOLOR_RED,DLG_BKCOLOR_GREEN,DLG_BKCOLOR_BLUE));
	graph.FillRectangle(&brushBk,rcClient.left,rcClient.top,rcClient.Width()+1,rcClient.Height()+1);

	CRect rcTmp=rcClient;
	rcTmp.left+=10;
	rcTmp.right-=10;
	int center=rcClient.bottom-BUTTON_RADIUS-2;
	rcTmp.top=center-RULER_HEIGHT/2;
	rcTmp.bottom=center+RULER_HEIGHT/2;
	SolidBrush brushRuler(Color(28,81,128));
	graph.FillRectangle(&brushRuler,rcTmp.left+1+RULER_HEIGHT/2,rcTmp.top+1,rcTmp.Width()-RULER_HEIGHT,rcTmp.Height());

	rcTmp.top=center-BUTTON_RADIUS;
	rcTmp.bottom=center+BUTTON_RADIUS;
	SolidBrush brushButton(Color(0,125,125));
	int currPos=(int)(m_CurPos*(rcTmp.Width()-BUTTON_RADIUS*2)+rcTmp.left+BUTTON_RADIUS);
	graph.FillEllipse(&brushButton,currPos-BUTTON_RADIUS+1,rcTmp.top+1,BUTTON_RADIUS*2,BUTTON_RADIUS*2);

/*	RectF rcText;
	rcText.X=(Gdiplus::REAL)(currPos-20);
	rcText.Y=(Gdiplus::REAL)rcClient.top;
	rcText.Width=40;
	rcText.Height=20;
	CStringW str;
	str.Format(L"%.3f",m_CurPos);
	Gdiplus::Font font(L"微软雅黑",9);
	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentCenter);
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	graph.DrawString(str,str.GetLength(),&font,rcText,&stringFormat,&brushButton);*/

	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),&memDC,1,1,SRCCOPY);
}


void CParamIndicator::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rc;
	GetClientRect(rc);
	
	m_CurPos=(float)(point.x-BUTTON_RADIUS-10)/(rc.Width()-BUTTON_RADIUS*2-10);
	if(m_CurPos<=0.0f) m_CurPos=0.0f;
	if(m_CurPos>=1.0f) m_CurPos=1.0f;

	Invalidate();
	SetCapture();
//	ClientToScreen(rc);
//	::ClipCursor(&rc);
	m_bLButtonDown=TRUE;

	CStatic::OnLButtonDown(nFlags, point);
}


void CParamIndicator::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bLButtonDown&&(nFlags&MK_LBUTTON))
	{
		CRect rc;
		GetClientRect(rc);
		m_CurPos=(float)(point.x-BUTTON_RADIUS)/(rc.Width()-BUTTON_RADIUS*2);
		if(m_CurPos<=0.0f) m_CurPos=0.0f;
		if(m_CurPos>=1.0f) m_CurPos=1.0f;
		Invalidate();
	}

	CStatic::OnMouseMove(nFlags, point);
}


void CParamIndicator::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown=FALSE;
//	ClipCursor(NULL);
	ReleaseCapture();

	CStatic::OnLButtonUp(nFlags, point);
}
