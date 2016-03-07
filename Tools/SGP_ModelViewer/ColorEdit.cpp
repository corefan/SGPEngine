// ColorEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ColorEdit.h"

// CColorEdit

IMPLEMENT_DYNAMIC(CColorEdit, CEdit)

CColorEdit::CColorEdit()
{
	m_Color=RGB(167,167,222);
}

CColorEdit::~CColorEdit()
{
}

BEGIN_MESSAGE_MAP(CColorEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_SETFOCUS, &CColorEdit::OnEnSetfocus)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, &CColorEdit::OnEnKillfocus)
END_MESSAGE_MAP()

// CColorEdit 消息处理程序

HBRUSH CColorEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	CRect rc;
	CDC* pdc=GetDC();
	GetClientRect(rc);
	rc.InflateRect(1,1,1,1);
	CBrush brush(m_Color);
	pdc->FrameRect(rc,&brush);
	ReleaseDC(pdc);

	return NULL;
}

void CColorEdit::OnEnSetfocus()
{
	m_Color=RGB(80,80,222);
	Invalidate();
}

void CColorEdit::OnEnKillfocus()
{
	m_Color=RGB(167,167,222);
	Invalidate();
}

