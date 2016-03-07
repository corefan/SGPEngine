#pragma once

class CPropertyGroupGridProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CPropertyGroupGridProperty)
public:
	CPropertyGroupGridProperty(const CString& strName, const COleVariant& varValue=_T(""));
	virtual ~CPropertyGroupGridProperty();
	virtual void OnDrawValue(CDC* pDC, CRect rect);

	CString m_Text;
};
