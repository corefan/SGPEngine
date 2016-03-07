#include "stdafx.h"
#include "afxcontrolbarutil.h"
#include "afxglobals.h"
#include "afxdrawmanager.h"
#include "afxcaptionbutton.h"
#include "afxtabctrl.h"
#include "afxvisualmanagervs2005.h"
#include "afxautohidebutton.h"
#include "afxtoolbar.h"
#include "afxtoolbarmenubutton.h"
#include "afxstatusbar.h"
#include "afxdockingmanager.h"
#include "afxtabbedpane.h"
#include "afxpropertygridctrl.h"

#include "CommonVisualManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCommonVisualManager,CMFCVisualManagerVS2005)

CCommonVisualManager::CCommonVisualManager()
{

}

CCommonVisualManager::~CCommonVisualManager()
{

}

COLORREF CCommonVisualManager::OnFillMiniFrameCaption(CDC* pDC, CRect rectCaption, CPaneFrameWnd* pFrameWnd, BOOL bActive)
{
//	if(bActive) pDC->FillSolidRect(rectCaption,RGB(233,235,237));
//	else pDC->FillSolidRect(rectCaption,RGB(212,214,216));
	CDrawingManager dm(*pDC);
	COLORREF col1,col2;
	if(bActive)
	{
		//	col1=RGB(31,117,241);
		//	col2=RGB(4,88,229);
		col1=col2=RGB(140,170,220);
	}
	else
	{
	//	col1=col2=RGB(157,185,235);	
		col2=RGB(225,238,255);
		col1=RGB(141,178,230);
	}
	dm.FillGradient(rectCaption, col1, col2, FALSE);

	return RGB(0,0,0);
}

COLORREF CCommonVisualManager::OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons)
{
//	if(bActive) pDC->FillSolidRect(rectCaption,RGB(233,235,237));
//	else pDC->FillSolidRect(rectCaption,RGB(212,214,216));
	
	CDrawingManager dm(*pDC);
	COLORREF col1,col2;
	if(bActive)
	{
	//	col1=RGB(31,117,241);
	//	col2=RGB(4,88,229);
		col1=col2=RGB(140,170,220);
	}
	else
	{
	//	col1=col2=RGB(157,185,235);	
		col2=RGB(225,238,255);
		col1=RGB(141,178,230);
	}
	dm.FillGradient(rectCaption, col1, col2, FALSE);

//	if(bActive) pDC->FillSolidRect(rectCaption,RGB(5,89,230));
//	else pDC->FillSolidRect(rectCaption,RGB(157,185,235),);
	return RGB(0,0,0);
}

void CCommonVisualManager::OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd)
{
/*	pDC->FillSolidRect(rectTab,RGB(77,141,171));
	pDC->SetTextColor(RGB(0,0,0));*/
	CMFCVisualManagerVS2005::OnDrawTab(pDC,rectTab,iTab,TRUE,pTabWnd);
	if(!bIsActive)
	{
		CRect rcTab;
		rcTab.DeflateRect(2,2,2,0);
		CBrush brush(RGB(220,230,232));
		pDC->FillRect(rectTab,&brush);
	}
	OnDrawTabContent(pDC, rectTab, iTab, bIsActive, pTabWnd, RGB(0,0,0));
}

COLORREF CCommonVisualManager::GetPropertyGridGroupColor(CMFCPropertyGridCtrl* pPropList)
{
//	return RGB(220,222,224);
	return CMFCVisualManagerVS2005::GetPropertyGridGroupColor(pPropList);
}

void CCommonVisualManager::OnDrawSeparator(CDC* pDC, CBasePane* pBar, CRect rect, BOOL bIsHoriz)
{
//	pDC->FillSolidRect(rect,RGB(0,255,0));
	CMFCVisualManagerVS2005::OnDrawSeparator(pDC,pBar,rect,bIsHoriz);
}

int CCommonVisualManager::GetDockingTabsBordersSize()
{
//	return 3;
	return CMFCVisualManagerVS2005::GetDockingTabsBordersSize();
}

void CCommonVisualManager::OnDrawToolBoxFrame(CDC* pDC, const CRect& rect)
{
//	pDC->FillSolidRect(rect,RGB(0,0,0));
	CMFCVisualManagerVS2005::OnDrawToolBoxFrame(pDC,rect);
}

void CCommonVisualManager::GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack, COLORREF& clrHighlight,
	COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack)
{
/*	clrDark=RGB(0,0,0);
	clrBlack=RGB(0,0,0);
	clrHighlight=RGB(0,0,0);
	clrFace=RGB(0,0,0);
	clrDarkShadow=RGB(0,0,0);
	clrLight=RGB(0,0,0);
	pbrFace=new CBrush;
	pbrFace->CreateSolidBrush(RGB(0,0,0));
	pbrBlack=new CBrush;
	pbrBlack->CreateSolidBrush(RGB(0,0,0));*/
	CMFCVisualManagerVS2005::GetTabFrameColors(pTabWnd,clrDark,clrBlack,clrHighlight,clrFace,clrDarkShadow,clrLight,pbrFace,pbrBlack);
}

void CCommonVisualManager::OnDrawAutoHideButtonBorder(CDC* pDC, CRect rectBounds, CRect rectBorderSize, CMFCAutoHideButton* pButton)
{
	COLORREF col=RGB(100,100,100);
	if (rectBorderSize.left > 0)
		pDC->FillSolidRect(rectBounds.left, rectBounds.top, rectBounds.left + rectBorderSize.left, rectBounds.bottom, col);
	if (rectBorderSize.top > 0)
		pDC->FillSolidRect(rectBounds.left, rectBounds.top, rectBounds.right, rectBounds.top + rectBorderSize.top, col);
	if (rectBorderSize.right > 0)
		pDC->FillSolidRect(rectBounds.right - rectBorderSize.right, rectBounds.top, rectBounds.right, rectBounds.bottom, col);
	if (rectBorderSize.bottom > 0)
		pDC->FillSolidRect(rectBounds.left, rectBounds.bottom - rectBorderSize.bottom, rectBounds.right, rectBounds.bottom, col);
	pDC->SetTextColor(RGB(0,0,0));
}

void CCommonVisualManager::OnFillAutoHideButtonBackground(CDC* pDC, CRect rect, CMFCAutoHideButton* pButton)
{
//	CMFCVisualManagerOffice2003::OnFillAutoHideButtonBackground(pDC,rect,pButton);
	CDrawingManager dm(*pDC);

//	COLORREF col1=RGB(248,228,207),col2=RGB(249,200,104);
//	COLORREF col1=RGB(236,233,216),col2=RGB(236,233,216);
	COLORREF col2=RGB(225,238,255);
	COLORREF col1=RGB(141,178,230);
	dm.FillGradient(rect, col1, col2, pButton->IsHorizontal());
}

void CCommonVisualManager::OnFillHighlightedArea(CDC* pDC, CRect rect, CBrush* pBrush, CMFCToolBarButton* pButton)
{
//	pDC->FillSolidRect(rect,RGB(255,255,255));
	CMFCVisualManagerVS2005::OnFillHighlightedArea(pDC,rect,pBrush,pButton);
}

void CCommonVisualManager::OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd)
{
//	pDC->FillSolidRect(rect,RGB(222,224,226));
	pDC->FillSolidRect(rect,RGB(236,233,216));
}

void CCommonVisualManager::OnDrawCaptionButton(CDC* pDC, CMFCCaptionButton* pButton, BOOL bActive, BOOL bHorz, BOOL bMaximized, BOOL bDisabled, int nImageID)
{
	CMFCVisualManagerVS2005::OnDrawCaptionButton(pDC,pButton,bActive,bHorz,bMaximized,bDisabled,nImageID);
}

void CCommonVisualManager::OnDrawPaneBorder(CDC* pDC, CBasePane* pBar, CRect& rect)
{
//	pDC->FillSolidRect(rect,RGB(0,0,0));
	pDC->FillSolidRect(rect,RGB(22,106,238));
//	CMFCVisualManagerVS2005::OnDrawPaneBorder(pDC,pBar,rect);
}

void CCommonVisualManager::OnDrawBarGripper(CDC* pDC, CRect rectGripper, BOOL bHorz, CBasePane* pBar)
{
//	pDC->FillSolidRect(rectGripper,RGB(0,0,0));
	CMFCVisualManagerVS2005::OnDrawBarGripper(pDC,rectGripper,bHorz,pBar);
}

void CCommonVisualManager::OnDrawCaptionBarBorder(CDC* pDC, CMFCCaptionBar* pBar, CRect rect, COLORREF clrBarBorder, BOOL bFlatBorder)
{
//	pDC->FillSolidRect(rect,RGB(0,0,0));
	CMFCVisualManagerVS2005::OnDrawCaptionBarBorder(pDC,pBar,rect,clrBarBorder,bFlatBorder);
}

void CCommonVisualManager::OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea)
{
	CMFCVisualManagerVS2005::OnFillBarBackground(pDC,pBar,rectClient,rectClip,bNCArea);

//	pDC->FillSolidRect(rectClient,RGB(222,224,226));
//	pDC->FillSolidRect(rectClient,RGB(236,233,216));
//	pDC->FillSolidRect(rectClient,RGB(195,205,220));

	CDrawingManager dm(*pDC);

//	CRect rcUp,rcDown;
//	rcUp=rcDown=rectClient;
//	rcUp.bottom=(rectClient.top+rectClient.bottom)/2;
//	rcDown.top=rcUp.bottom;
	COLORREF col1=RGB(202,211,226);
	COLORREF col2=RGB(180,190,210);
	dm.FillGradient(rectClient, col1, col2, FALSE);
//	dm.FillGradient(rcDown,col1,col2,TRUE);

//	pDC->FillSolidRect(rectClient,RGB(174,185,205));

//	rectClip.top-=10;
//	rectClip.right+=10;
//	pDC->FillSolidRect(rectClip,RGB(222,224,226));
}

void CCommonVisualManager::OnDrawHeaderCtrlBorder(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted)
{
	COLORREF clrStart  = RGB(246,246,246);
	COLORREF clrFinish = RGB(223,223,223);
	COLORREF clrBorder = RGB(182,182,182);

	if (bIsPressed||bIsHighlighted)
	{
		clrStart  = RGB(248,214,154);
		clrFinish = RGB(241,194,98);
		clrBorder = RGB(242,149,54);
	}

	{
		CDrawingManager dm(*pDC);
		dm.FillGradient(rect, clrFinish, clrStart);
	}

	CPen pen(PS_SOLID, 0, clrBorder);
	CPen* pOldPen = pDC->SelectObject(&pen);

	if (bIsPressed || bIsHighlighted)
	{
		pDC->MoveTo(rect.right - 1, rect.top);
		pDC->LineTo(rect.right - 1, rect.bottom - 1);
		pDC->LineTo(rect.left, rect.bottom - 1);
		pDC->LineTo(rect.left, rect.top - 1);
	}
	else
	{
		pDC->MoveTo(rect.right - 1, rect.top);
		pDC->LineTo(rect.right - 1, rect.bottom - 1);
		pDC->LineTo(rect.left - 1, rect.bottom - 1);
	}

	pDC->SelectObject(pOldPen);
}

void CCommonVisualManager::OnDrawFloatingToolbarBorder(	CDC* pDC, CMFCBaseToolBar* pToolBar, CRect rectBorder, CRect rectBorderSize)
{
/*	CBrush brush;
	brush.CreateSolidBrush(RGB(237,237,237));
	CBrush* pOldBrush=pDC->SelectObject(&brush);
	ENSURE(pOldBrush != NULL);

	pDC->PatBlt(rectBorder.left, rectBorder.top, rectBorderSize.left, rectBorder.Height(), PATCOPY);
	pDC->PatBlt(rectBorder.left, rectBorder.top, rectBorder.Width(), rectBorderSize.top, PATCOPY);
	pDC->PatBlt(rectBorder.right - rectBorderSize.right, rectBorder.top, rectBorderSize.right, rectBorder.Height(), PATCOPY);
	pDC->PatBlt(rectBorder.left, rectBorder.bottom - rectBorderSize.bottom, rectBorder.Width(), rectBorderSize.bottom, PATCOPY);
	pDC->SelectObject(&pOldBrush);*/
	CMFCVisualManagerVS2005::OnDrawFloatingToolbarBorder(pDC,pToolBar,rectBorder,rectBorderSize);
}

