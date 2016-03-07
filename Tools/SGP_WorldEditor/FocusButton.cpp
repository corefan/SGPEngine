// FocusButton.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FocusButton.h"

// FocusButton

IMPLEMENT_DYNAMIC(CFocusButton, CButton)

CFocusButton::CFocusButton()
{
	m_bFocused=TRUE;
}

CFocusButton::~CFocusButton()
{
}

BEGIN_MESSAGE_MAP(CFocusButton, CButton)
END_MESSAGE_MAP()

// FocusButton ��Ϣ�������

void CFocusButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rc=lpDrawItemStruct->rcItem;
	pDC->FillSolidRect(rc,RGB(248,248,246));
	CBrush brush(RGB(0,60,116));
	if(m_bFocused) pDC->FrameRect(rc,&brush);
//	pDC->DrawFocusRect(rc);
	CString strText;
	GetWindowText(strText);
	CFont font;
	font.CreatePointFont(90,"΢���ź�");
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(strText,rc,DT_CENTER|DT_VCENTER);
}

void CFocusButton::PreSubclassWindow()
{
	ModifyStyle(0,BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}
