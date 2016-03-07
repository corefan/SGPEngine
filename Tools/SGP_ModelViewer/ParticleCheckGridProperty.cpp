#include "stdafx.h"

#include "ParticleCheckGridProperty.h"
#include "ParticleModifierGridProperty.h"

IMPLEMENT_DYNAMIC(CParticleCheckGridProperty,CMFCPropertyGridProperty)

CParticleCheckGridProperty::CParticleCheckGridProperty(const CString& strName,bool bChecked):\
	CMFCPropertyGridProperty(strName,""),\
	m_bCheck(bChecked)
{
	m_bAllowEdit=FALSE;
	m_dwStart=GetTickCount();
	m_CheckType=Particle_CheckNone;
}

void CParticleCheckGridProperty::OnDrawValue(CDC* pDC, CRect rect)
{
	CPen pen(PS_SOLID,1,RGB(28,81,128));
	CPen *pOldPen=(CPen*)pDC->SelectObject(&pen);
	CRect rc=rect;
	rc.left+=3;
	rc.right=rc.left+rc.Height()-4;
	rc.bottom-=2;
	rc.top+=3;
	pDC->Rectangle(rc);
	if(m_bCheck)
	{
		CPen pen2(PS_SOLID,2,RGB(33,161,33));
		pDC->SelectObject(&pen2);
		CPoint pt1,pt2,pt3;
		pt1.x=rc.left+2;
		pt1.y=rc.top+rc.Height()*3/8;

		pt2.x=rc.left+rc.Width()*5/12;
		pt2.y=rc.top+rc.Height()*4/5-1;

		pt3.x=rc.right-2;
		pt3.y=rc.top+rc.Height()/4-1;

		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
		pDC->LineTo(pt3);
	}
	pDC->SelectObject(pOldPen);
}

BOOL CParticleCheckGridProperty::OnEdit(LPPOINT lptClick)
{
	if(GetTickCount()-m_dwStart>=200)
	{
		m_bCheck=!m_bCheck;
		CheckAction();
		m_dwStart=GetTickCount();
	}

	return TRUE;
}

void CParticleCheckGridProperty::CheckAction()
{
	switch(m_CheckType)
	{
	case Particle_CheckZone:
		((CParticleModifierGridProperty*)GetParent())->ShowZoneProperty(m_bCheck);
		break;
	default:
		break;
	}
}

BOOL CParticleCheckGridProperty::PushChar(UINT nChar)
{
	return TRUE;
}

void CParticleCheckGridProperty::SetCheck(bool bCheck)
{
	m_bCheck=bCheck;
	Redraw();
}
