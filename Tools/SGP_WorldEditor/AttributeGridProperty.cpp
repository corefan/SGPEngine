#include "stdafx.h"

#include "AttributeGridProperty.h"

IMPLEMENT_DYNAMIC(CAttributeGridProperty,CMFCPropertyGridProperty)

CAttributeGridProperty::CAttributeGridProperty(const CString& strName,const _variant_t& varValue,LPCTSTR lpszDescr,DWORD_PTR dwData):\
	CMFCPropertyGridProperty(strName,varValue,lpszDescr,dwData)
{

}

CAttributeGridProperty::~CAttributeGridProperty()
{

}

void CAttributeGridProperty::SetGridValue(const COleVariant& varValue)
{
	m_varValue = varValue;

	if(m_pWndList != NULL)
		m_pWndList->InvalidateRect(m_Rect);
}