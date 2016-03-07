#pragma once

class CParticleInterpolatorGraphProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleInterpolatorGraphProperty);
public:
	CParticleInterpolatorGraphProperty(const CString& strName,const COleVariant& varValue);
	virtual ~CParticleInterpolatorGraphProperty();
	virtual BOOL OnDblClk(CPoint point);
	virtual BOOL OnClickValue(UINT uiMsg,CPoint point);
	DWORD m_dwStart;
};