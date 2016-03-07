// ExpandButton.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ExpandButton.h"

#include "CommonMacroDef.h"
// CExpandButton

IMPLEMENT_DYNAMIC(CExpandButton, CButton)

CExpandButton::CExpandButton()
{
	m_bExpanded=TRUE;
}

CExpandButton::~CExpandButton()
{
}


BEGIN_MESSAGE_MAP(CExpandButton, CButton)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CExpandButton 消息处理程序

void CExpandButton::PreSubclassWindow()
{
	ModifyStyle(0,BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}

void CExpandButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

#ifdef EXPAND_BUTTON_USE_GDI_DRAW
	// Use GDI mode
	CRect rc=lpDrawItemStruct->rcItem;
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rcBox=rc;
	rcBox.right=rc.left+rcBox.Height()-6;
	rc.left=rcBox.right;

	// Fill background

	pDC->FillSolidRect(rcBox.left-1,rcBox.top-1,rcBox.Width()+2,rcBox.Height()+2,GetSysColor(COLOR_BTNFACE));
	rcBox.DeflateRect(0,4,3,5);
	//	pDC->FillSolidRect(rcBox.left,rcBox.top,rcBox.Width(),rcBox.Height(),RGB(255,255,255));
	// Draw Box
	CPen penBox(PS_SOLID,1,RGB(120,152,181));
	CBrush brush(RGB(255,255,255));
	pDC->SelectObject(&brush);
	pDC->SelectObject(&penBox);
	pDC->Rectangle(rcBox);

	// Draw "+" or "-"
	rcBox.DeflateRect(2,2);
	CPen penLine(PS_SOLID,1,RGB(0,0,0));
	pDC->SelectObject(&penLine);
	pDC->MoveTo(rcBox.left,(rcBox.top+rcBox.bottom)/2);
	pDC->LineTo(rcBox.right,(rcBox.top+rcBox.bottom)/2);
	if(!m_bExpanded)
	{
		pDC->MoveTo((rcBox.left+rcBox.right)/2,rcBox.top);
		pDC->LineTo((rcBox.left+rcBox.right)/2,rcBox.bottom);
	}
	// Draw Button
	CPen penBtn(PS_SOLID,2,RGB(0,85,234));
	pDC->SelectObject(&penBtn);
	if(lpDrawItemStruct->itemState&ODS_SELECTED)// the button have keyboard focus
	{	
	//	pDC->FillSolidRect(rc.left,rc.top,rc.Width(),rc.Height(),RGB(80,80,222));
	//	pDC->FillSolidRect(rc.left,rc.top,rc.Width(),rc.Height(),RGB(28,81,128));
	//	pDC->FillSolidRect(rc.left,rc.top,rc.Width(),rc.Height(),RGB(140,170,220));
		CDrawingManager dm(*pDC);
		COLORREF col2=RGB(140,170,220);
		COLORREF col1=RGB(140,170,220);
		dm.FillGradient(rc, col1, col2, FALSE);
	}
	else//the button is in normal state
	{
	//	pDC->FillSolidRect(rc.left,rc.top,rc.Width(),rc.Height(),RGB(147,147,222));
	//	pDC->FillSolidRect(rc.left,rc.top,rc.Width(),rc.Height(),RGB(50,140,190));
	//	pDC->FillSolidRect(rc.left,rc.top,rc.Width(),rc.Height(),RGB(157,185,235));
		CDrawingManager dm(*pDC);
		COLORREF col2=RGB(225,238,255);
		COLORREF col1=RGB(141,178,230);
		dm.FillGradient(rc, col1, col2, FALSE);
	}

/*	CPen penIndicator(PS_SOLID,2,RGB(255,255,255));
//	CPen penIndicator(PS_SOLID,2,RGB(28,81,128));
	pDC->SelectObject(&penIndicator);
	int mid=(rc.top+rc.bottom)/2;
	if(m_bExpanded)
	{
		pDC->MoveTo(rc.right-11,rc.bottom-4);
		pDC->LineTo(rc.right-18,mid);
		pDC->LineTo(rc.right-25,rc.bottom-4);

		pDC->MoveTo(rc.right-11,mid);
		pDC->LineTo(rc.right-18,rc.top+4);
		pDC->LineTo(rc.right-25,mid);
	}
	else
	{
		pDC->MoveTo(rc.right-11,mid);
		pDC->LineTo(rc.right-18,rc.bottom-4);
		pDC->LineTo(rc.right-25,mid);

		pDC->MoveTo(rc.right-11,rc.top+4);
		pDC->LineTo(rc.right-18,mid);
		pDC->LineTo(rc.right-25,rc.top+4);
	}*/

	CString str;
	GetWindowText(str);
	CFont font;
	font.CreatePointFont(100,_T("微软雅黑"));
	pDC->SelectObject(&font);
	//	pDC->SetTextAlign(TA_CENTER);
	//	pDC->DrawText(str,rc,DT_CENTER|DT_VCENTER);
	CSize size=pDC->GetTextExtent(str);
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut((rc.left+rc.right-size.cx)/2,(rc.top+rc.bottom-size.cy)/2,str);

	Graphics graph(lpDrawItemStruct->hDC);
	graph.SetSmoothingMode(SmoothingModeHighQuality);
	Pen penIndicator(Color(255,255,255),2);
	int mid=(rc.top+rc.bottom)/2;
	if(m_bExpanded)
	{
		graph.DrawLine(&penIndicator,Point(rc.right-11,rc.bottom-4),Point(rc.right-18,mid));
		graph.DrawLine(&penIndicator,Point(rc.right-18,mid),Point(rc.right-25,rc.bottom-4));
		graph.DrawLine(&penIndicator,Point(rc.right-11,mid),Point(rc.right-18,rc.top+4));
		graph.DrawLine(&penIndicator,Point(rc.right-18,rc.top+4),Point(rc.right-25,mid));
	}
	else
	{
		graph.DrawLine(&penIndicator,Point(rc.right-18,rc.bottom-4),Point(rc.right-11,mid));
		graph.DrawLine(&penIndicator,Point(rc.right-25,mid),Point(rc.right-18,rc.bottom-4));
		graph.DrawLine(&penIndicator,Point(rc.right-18,mid),Point(rc.right-11,rc.top+4));
		graph.DrawLine(&penIndicator,Point(rc.right-25,rc.top+4),Point(rc.right-18,mid));
	}
#else
	// Use GDI+ mode
	CRect rc=lpDrawItemStruct->rcItem;
//	if(rc.Height()/2!=0) rc.right--;
	Graphics graph(lpDrawItemStruct->hDC);
	graph.SetSmoothingMode(SmoothingModeHighQuality);

	// Draw Expand Box
	CRect rcBox=rc;
	rcBox.right=rc.left+rcBox.Height()-6;
	rc.left=rcBox.right;
	SolidBrush brushBk(Color(DLG_BKCOLOR_RED,DLG_BKCOLOR_GREEN,DLG_BKCOLOR_BLUE));
	graph.FillRectangle(&brushBk,rcBox.left-1,rcBox.top-1,rcBox.Width()+1,rcBox.Height()+1);
	rcBox.DeflateRect(0,5,4,5);
	SolidBrush brushBox(Color(255,255,255));
	graph.FillRectangle(&brushBox,rcBox.left,rcBox.top,rcBox.Width(),rcBox.Height());
	Pen penBox(Color(120,152,181),1);
	graph.DrawRectangle(&penBox,rcBox.left,rcBox.top,rcBox.Width(),rcBox.Height());
	rcBox.DeflateRect(2,2);
	Pen penLine(Color(0,0,0),1);
	graph.DrawLine(&penLine,Point(rcBox.left,(rcBox.top+rcBox.bottom)/2),Point(rcBox.right,(rcBox.top+rcBox.bottom)/2));
	if(!m_bExpanded)
		graph.DrawLine(&penLine,Point((rcBox.left+rcBox.right)/2,rcBox.top),Point((rcBox.left+rcBox.right)/2,rcBox.bottom));
	
	// Draw Button
	//the button is in normal state
	SolidBrush brushBtn(Color(147,147,222));
	// the button have keyboard focus
	if(lpDrawItemStruct->itemState&ODS_SELECTED)
		brushBtn.SetColor(Color(80,80,222));
	graph.FillRectangle(&brushBtn,rc.left,rc.top,rc.Width(),rc.Height());

	// Draw Border
	Pen penBtn(Color(0,85,234),2);
	graph.DrawRectangle(&penBtn,rc.left,rc.top,rc.Width()-1,rc.Height()-1);

	Pen penIndicator(Color(28,81,128),2);
	int mid=(rc.top+rc.bottom)/2;
	if(m_bExpanded)
	{
		graph.DrawLine(&penIndicator,Point(rc.right-11,rc.bottom-4),Point(rc.right-18,mid));
		graph.DrawLine(&penIndicator,Point(rc.right-18,mid),Point(rc.right-25,rc.bottom-4));
		graph.DrawLine(&penIndicator,Point(rc.right-11,mid),Point(rc.right-18,rc.top+4));
		graph.DrawLine(&penIndicator,Point(rc.right-18,rc.top+4),Point(rc.right-25,mid));
	}
	else
	{
		graph.DrawLine(&penIndicator,Point(rc.right-18,rc.bottom-4),Point(rc.right-11,mid));
		graph.DrawLine(&penIndicator,Point(rc.right-25,mid),Point(rc.right-18,rc.bottom-4));
		graph.DrawLine(&penIndicator,Point(rc.right-18,mid),Point(rc.right-11,rc.top+4));
		graph.DrawLine(&penIndicator,Point(rc.right-25,rc.top+4),Point(rc.right-18,mid));
	}

	Gdiplus::Font font(L"微软雅黑",10);
	RectF rcF((REAL)rc.left,(REAL)rc.top,(REAL)rc.Width(),(REAL)rc.Height());
	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentCenter);
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	SolidBrush brushText(Color(0,0,0));
	WCHAR text[40];
	::GetWindowTextW(m_hWnd,text,sizeof(text)/sizeof(WCHAR));
	graph.DrawString(text,wcslen(text),&font,rcF,&stringFormat,&brushText);
#endif
}

void CExpandButton::SetExpand(BOOL bExpand)
{
	m_bExpanded=bExpand;
	Invalidate();
}

BOOL CExpandButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	return CButton::OnEraseBkgnd(pDC);
}
