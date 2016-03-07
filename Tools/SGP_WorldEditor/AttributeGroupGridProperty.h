#pragma once

class CAttributeGroupGridProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CAttributeGroupGridProperty)
public:
	CAttributeGroupGridProperty(const CString& strName,DWORD_PTR dwData=0,BOOL bIsValueList=FALSE);
	virtual ~CAttributeGroupGridProperty();
public:
	void RedrawProperty();
};
