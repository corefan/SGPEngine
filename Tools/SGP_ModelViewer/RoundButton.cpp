// RoundButton.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "RoundButton.h"

#include "CommonMacroDef.h"
// CRoundButton

IMPLEMENT_DYNAMIC(CRoundButton, CButton)

CRoundButton::CRoundButton()
{
	m_bText=FALSE;
	m_nImageType=NotAnyButton;
	m_bRound=FALSE;
	m_bCustomBkColor=FALSE;
}

CRoundButton::~CRoundButton()
{
}

BEGIN_MESSAGE_MAP(CRoundButton, CButton)
END_MESSAGE_MAP()

// CRoundButton 消息处理程序

void CRoundButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc=lpDrawItemStruct->rcItem;
	if(rc.Height()/2!=0) rc.right--;
	Graphics graph(lpDrawItemStruct->hDC);
	graph.SetSmoothingMode(SmoothingModeHighQuality);

	if(m_bCustomBkColor)
	{
		SolidBrush brushBk(Color(GetRValue(m_BkColor),GetGValue(m_BkColor),GetBValue(m_BkColor)));
		graph.FillRectangle(&brushBk,rc.left-1,rc.top-1,rc.Width()+2,rc.Height()+2);
	}
	else
	{
	//	SolidBrush brushBk(Color(DLG_BKCOLOR_RED,DLG_BKCOLOR_GREEN,DLG_BKCOLOR_BLUE));
		DWORD color=GetSysColor(COLOR_BTNFACE);
		SolidBrush brushBk(Color(GetRValue(color),GetGValue(color),GetBValue(color)));
		graph.FillRectangle(&brushBk,rc.left-1,rc.top-1,rc.Width()+2,rc.Height()+2);
	}
	
	SolidBrush brush(Color(147,147,222));
	Pen pen(Color(47,47,47),2);
	if(lpDrawItemStruct->itemState&ODS_DISABLED)//the button is disabled
	{
		brush.SetColor(Color(201,199,186));
		pen.SetColor(Color(150,150,150));
	}
	else if(lpDrawItemStruct->itemState&ODS_SELECTED)//the button have keyboard focus
	{
		brush.SetColor(Color(80,80,222));
		pen.SetColor(Color(12,12,12));
	}

	if(m_bRound)
	{
		int r=rc.Width()<rc.Height()?rc.Width():rc.Height();
		graph.FillEllipse(&brush,rc.left,rc.top,r,r);
	}
	else
	{		
		graph.FillEllipse(&brush,rc.left,rc.top,rc.Height(),rc.Height());
		graph.FillEllipse(&brush,rc.right-rc.Height(),rc.top,rc.Height(),rc.Height());
		graph.FillRectangle(&brush,rc.left+rc.Height()/2,rc.top,rc.Width()-rc.Height(),rc.Height());
	}

	switch(m_nImageType)
	{
	case PlayRoundButton:
		{
			Point pt[3]={Point(rc.Width()/3+4,6),Point(rc.Width()/3+4,rc.Height()-6),Point(rc.Width()*2/3,rc.Height()/2)};
			graph.DrawPolygon(&pen,pt,3);
		}
		break;
	case StopRoundButton:
		{
			graph.DrawLine(&pen,Point(rc.Width()/3+2,6),Point(rc.Width()/3+2,rc.Height()-6));
			graph.DrawLine(&pen,Point(rc.Width()*2/3-2,6),Point(rc.Width()*2/3-2,rc.Height()-6));
			graph.DrawLine(&pen,Point(rc.Width()/3+2,6),Point(rc.Width()*2/3-2,6));
			graph.DrawLine(&pen,Point(rc.Width()/3+2,rc.Height()-6),Point(rc.Width()*2/3-2,rc.Height()-6));
		}
		break;
	case PauseRoundButton:
		graph.DrawLine(&pen,Point(rc.Width()/3+6,6),Point(rc.Width()/3+6,rc.Height()-6));
		graph.DrawLine(&pen,Point(rc.Width()*2/3-2,6),Point(rc.Width()*2/3-2,rc.Height()-6));
		break;
	case NextFrameButton:
		graph.DrawLine(&pen,Point(rc.Width()/3,6),Point(rc.Width()*2/3,rc.Height()/2));
		graph.DrawLine(&pen,Point(rc.Width()/3,rc.Height()-6),Point(rc.Width()*2/3,rc.Height()/2));
		break;
	case PrevFrameButton:
		graph.DrawLine(&pen,Point(rc.Width()*2/3,6),Point(rc.Width()/3,rc.Height()/2));
		graph.DrawLine(&pen,Point(rc.Width()*2/3,rc.Height()-6),Point(rc.Width()/3,rc.Height()/2));
		break;
	case PrevAnimationButton:
		graph.DrawLine(&pen,Point(rc.Width()/2,6),Point(rc.Width()/3,rc.Height()/2));
		graph.DrawLine(&pen,Point(rc.Width()/2,rc.Height()-6),Point(rc.Width()/3,rc.Height()/2));
		graph.DrawLine(&pen,Point(rc.Width()*2/3,6),Point(rc.Width()/2,rc.Height()/2));
		graph.DrawLine(&pen,Point(rc.Width()*2/3,rc.Height()-6),Point(rc.Width()/2,rc.Height()/2));
		break;
	case NextAnimationButton:
		graph.DrawLine(&pen,Point(rc.Width()/3,6),Point(rc.Width()/2,rc.Height()/2));
		graph.DrawLine(&pen,Point(rc.Width()/3,rc.Height()-6),Point(rc.Width()/2,rc.Height()/2));
		graph.DrawLine(&pen,Point(rc.Width()/2,6),Point(rc.Width()*2/3,rc.Height()/2));
		graph.DrawLine(&pen,Point(rc.Width()/2,rc.Height()-6),Point(rc.Width()*2/3,rc.Height()/2));
		break;
	case CloseDlgButton:
		graph.DrawLine(&pen,Point(rc.Width()/4,rc.Height()/4),Point(rc.Width()*3/4,rc.Height()*3/4));
		graph.DrawLine(&pen,Point(rc.Width()*3/4,rc.Height()/4),Point(rc.Width()/4,rc.Height()*3/4));
	default:
		break;
	}

	//whether show text
	if(m_bText)
	{
		Gdiplus::Font font(L"微软雅黑",10);
		RectF rcF((REAL)rc.left,(REAL)rc.top,(REAL)rc.Width(),(REAL)rc.Height());
		StringFormat stringFormat;
		stringFormat.SetAlignment(StringAlignmentCenter);
		stringFormat.SetLineAlignment(StringAlignmentCenter);
		SolidBrush brushText(Color(0,0,0));
		WCHAR text[20];
		::GetWindowTextW(m_hWnd,text,sizeof(text)/sizeof(WCHAR));
		graph.DrawString(text,wcslen(text),&font,rcF,&stringFormat,&brushText);
	}

}

void CRoundButton::SetHaveText(BOOL bText)
{
	m_bText=bText;
}

void CRoundButton::PreSubclassWindow()
{
	ModifyStyle(0,BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}
