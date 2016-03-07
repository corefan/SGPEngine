#include "stdafx.h"

#include "AttributeGroupGridProperty.h"

IMPLEMENT_DYNAMIC(CAttributeGroupGridProperty,CMFCPropertyGridProperty)

CAttributeGroupGridProperty::CAttributeGroupGridProperty(const CString& strName,DWORD_PTR dwData,BOOL bIsValueList):\
	CMFCPropertyGridProperty(strName,dwData,bIsValueList)
{

}

CAttributeGroupGridProperty::~CAttributeGroupGridProperty()
{

}

void CAttributeGroupGridProperty::RedrawProperty()
{
	if(m_pWndList!=NULL) m_pWndList->InvalidateRect(m_Rect);
}