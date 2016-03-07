#include "stdafx.h"

#include "EditPropertyGridProperty.h"

IMPLEMENT_DYNAMIC(CEditPropertyGridProperty,CMFCPropertyGridProperty)

CEditPropertyGridProperty::CEditPropertyGridProperty(const CString& strName, const COleVariant& varValue,int index,EditPropertyType nType):\
	CMFCPropertyGridProperty(strName,varValue),\
	m_Index(index),\
	m_nType(nType)
{}

CEditPropertyGridProperty::~CEditPropertyGridProperty()
{}
