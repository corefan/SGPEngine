#pragma once

#include <afxvisualmanagervs2005.h>

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CCommonVisualManager: public CMFCVisualManagerVS2005
{
	DECLARE_DYNCREATE(CCommonVisualManager)
public:
	CCommonVisualManager();
	~CCommonVisualManager();
public:
	virtual COLORREF OnFillMiniFrameCaption(CDC* pDC, CRect rectCaption, CPaneFrameWnd* pFrameWnd, BOOL bActive);
	virtual COLORREF OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons);
	virtual void OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual COLORREF GetPropertyGridGroupColor(CMFCPropertyGridCtrl* pPropList);
	virtual void OnDrawSeparator(CDC* pDC, CBasePane* pBar, CRect rect, BOOL bIsHoriz);
	virtual int GetDockingTabsBordersSize();
	virtual void OnDrawToolBoxFrame(CDC* pDC, const CRect& rect);
	virtual void GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack, COLORREF& clrHighlight,
		COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack);
	virtual void OnFillHighlightedArea(CDC* pDC, CRect rect, CBrush* pBrush, CMFCToolBarButton* pButton);
	virtual void OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnDrawCaptionButton(CDC* pDC, CMFCCaptionButton* pButton, BOOL bActive, BOOL bHorz, BOOL bMaximized, BOOL bDisabled, int nImageID=-1);
	virtual void OnDrawPaneBorder(CDC* pDC, CBasePane* pBar, CRect& rect);
	virtual void OnDrawBarGripper(CDC* pDC, CRect rectGripper, BOOL bHorz, CBasePane* pBar);
	virtual void OnDrawCaptionBarBorder(CDC* pDC, CMFCCaptionBar* pBar, CRect rect, COLORREF clrBarBorder, BOOL bFlatBorder);
	virtual void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);
	virtual int GetMDITabsBordersSize() { return 0; }
	virtual void OnDrawHeaderCtrlBorder(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
	virtual void OnDrawFloatingToolbarBorder(	CDC* pDC, CMFCBaseToolBar* pToolBar, CRect rectBorder, CRect rectBorderSize);
	// Auto Hide Button
	virtual COLORREF GetAutoHideButtonTextColor(CMFCAutoHideButton* pButton){return RGB(0,0,0);}
	virtual void OnDrawAutoHideButtonBorder(CDC* pDC, CRect rectBounds, CRect rectBorderSize, CMFCAutoHideButton* pButton);
	virtual void OnFillAutoHideButtonBackground(CDC* pDC, CRect rect, CMFCAutoHideButton* pButton);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif