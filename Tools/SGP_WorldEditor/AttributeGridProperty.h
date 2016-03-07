#pragma once

class CAttributeGridProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CAttributeGridProperty)
public:
	CAttributeGridProperty(const CString& strName,const _variant_t& varValue,LPCTSTR lpszDescr=NULL,DWORD_PTR dwData=0);
	virtual ~CAttributeGridProperty();
public:
	void SetGridValue(const COleVariant& varValue);
};