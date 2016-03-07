// ColorButton.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ColorButton.h"

#include "ModelViewerConfig.h"
// CColorButton

IMPLEMENT_DYNAMIC(CColorButton, CButton)

CColorButton::CColorButton()
{
	m_bText=FALSE;
}

CColorButton::~CColorButton()
{
}

BEGIN_MESSAGE_MAP(CColorButton, CButton)
END_MESSAGE_MAP()

// CColorButton 消息处理程序

void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc=lpDrawItemStruct->rcItem;

	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	pDC->FillSolidRect(&rc,m_Col);
	
	if(m_bText)
	{
		CString strText;
		GetWindowText(strText);
		pDC->DrawText(strText,&rc,DT_CENTER|DT_VCENTER);
	}
}

void CColorButton::SetColor(float r,float g,float b)
{
	m_Col=RGB(r*255.0,g*255.0,b*255.0);
	Invalidate(TRUE);
}

void CColorButton::SetColor(COLORREF col)
{
	m_Col=col;
	Invalidate(TRUE);
}

void CColorButton::SetColor(float* pRGB)
{
	m_Col=RGB(pRGB[0]*255.0,pRGB[1]*255.0,pRGB[2]*255.0);
	Invalidate(TRUE);
}

void CColorButton::ShowText(BOOL bShow)
{
	m_bText=bShow;
}