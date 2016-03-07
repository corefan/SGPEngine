#pragma once

class CParticleMenuGridProperty: public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleMenuGridProperty)
public:
	CParticleMenuGridProperty(const CString& strName, const COleVariant& varValue);
public:
	virtual void OnRClickValue(CPoint C,BOOL B);
	virtual BOOL OnEdit(LPPOINT lptClick);
	virtual BOOL PushChar(UINT nChar);
	void SetModifiedFlag();
	virtual BOOL OnDblClk(CPoint point);
};