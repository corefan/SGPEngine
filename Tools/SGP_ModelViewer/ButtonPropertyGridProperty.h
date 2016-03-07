#pragma once

class CButtonPropertyGridProperty: public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CButtonPropertyGridProperty)
public:
	CButtonPropertyGridProperty(const CString& strName, const COleVariant& varValue);
	virtual ~CButtonPropertyGridProperty();
public:
	virtual void OnDrawButton(CDC* pDC,CRect rectButton);
	virtual void OnClickButton(CPoint point);
};
