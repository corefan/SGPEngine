#pragma once

enum ParticleCheckType
{
	Particle_CheckNone,
	Particle_CheckZone
};

class CParticleCheckGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleCheckGridProperty)
public:
	CParticleCheckGridProperty(const CString& strName,bool bChecked);
	virtual void OnDrawValue(CDC* pDC,CRect rect);
	virtual BOOL OnEdit(LPPOINT lptClick);
	virtual BOOL PushChar(UINT nChar);
public:
	void SetCheck(bool bCheck);
	void SetCheckType(ParticleCheckType type){m_CheckType=type;}
	bool GetCheck(){return m_bCheck;}
	void CheckAction();
private:
	bool m_bCheck;
	DWORD m_dwStart;
	ParticleCheckType m_CheckType;
};
