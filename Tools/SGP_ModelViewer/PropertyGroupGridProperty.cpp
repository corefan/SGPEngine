#include "stdafx.h"

#include "PropertyGroupGridProperty.h"

IMPLEMENT_DYNAMIC(CPropertyGroupGridProperty,CMFCPropertyGridProperty)

CPropertyGroupGridProperty::CPropertyGroupGridProperty(const CString& strName, const COleVariant& varValue):\
	CMFCPropertyGridProperty(strName,varValue)
{
	m_bGroup=TRUE;
	m_Text=varValue;
}

CPropertyGroupGridProperty::~CPropertyGroupGridProperty()
{

}

void CPropertyGroupGridProperty::OnDrawValue(CDC* pDC, CRect rect)
{
	pDC->DrawText(m_Text,rect,DT_VCENTER|DT_LEFT);
}
