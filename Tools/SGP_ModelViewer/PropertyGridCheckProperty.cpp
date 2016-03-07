#include "stdafx.h"

#include "PropertyGridCheckProperty.h"
#include "DetailPropertyGridCtrl.h"
#include "ModelDetailPane.h"
#include "ModelViewerConfig.h"

IMPLEMENT_DYNAMIC(CPropertyGridCheckProperty,CMFCPropertyGridProperty)

CPropertyGridCheckProperty::CPropertyGridCheckProperty(const CString& strName, const COleVariant& varValue,CheckShowType type,int index):\
	CMFCPropertyGridProperty(strName,varValue)
{
	m_bChecked=TRUE;
	m_bAllowEdit=FALSE;
	m_dwStart=GetTickCount();
	m_Index=index;
	m_Type=type;
}

CPropertyGridCheckProperty::~CPropertyGridCheckProperty()
{

}

void CPropertyGridCheckProperty::OnDrawValue(CDC* pDC, CRect rect)
{
	CPen pen(PS_SOLID,1,RGB(28,81,128));
	CPen *pOldPen=(CPen*)pDC->SelectObject(&pen);
	CRect rc=rect;
	rc.left+=3;
	rc.right=rc.left+rc.Height()-4;
	rc.bottom-=2;
	rc.top+=3;
	pDC->Rectangle(rc);
	if(m_bChecked)
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

BOOL CPropertyGridCheckProperty::OnEdit(LPPOINT lptClick)
{
	if(GetTickCount()-m_dwStart>=200)
	{
		Check();
		m_dwStart=GetTickCount();
	}
	
	return TRUE;
}

BOOL CPropertyGridCheckProperty::PushChar(UINT nChar)
{
	return TRUE;
}

void CPropertyGridCheckProperty::SetCheck(BOOL bCheck)
{
	m_bChecked=bCheck;
	Redraw();
}

void CPropertyGridCheckProperty::Check()
{
	m_bChecked=!m_bChecked;

	// do actual action
/*	ModelViewerConfig* pConfig=ModelViewerConfig::GetInstance();
	((*pConfig).*m_ResponseFunc)(m_Index,m_bChecked);*/
	switch(m_Type)
	{
	case Check_ShowMesh:
		((CModelDetailPane*)(((CDetailPropertyGridCtrl*)m_pWndList)->GetParent()))->ShowMesh(m_Index,m_bChecked);
		break;
	case Check_ShowParticle:
		((CModelDetailPane*)(((CDetailPropertyGridCtrl*)m_pWndList)->GetParent()))->ShowParticle(m_Index,m_bChecked);
		break;
	case Check_ShowBoundBox:
		ModelViewerConfig::GetInstance()->SetBoxShow(m_Index,m_bChecked);
	}
	
}