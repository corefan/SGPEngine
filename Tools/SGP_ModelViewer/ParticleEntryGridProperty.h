#pragma once

namespace sgp
{
	struct ParticleEntryParam;
}

class CParticleEntryGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleEntryGridProperty)
public:
	CParticleEntryGridProperty(const CString strName);
	virtual ~CParticleEntryGridProperty(){}
	void Init();
	virtual void OnRClickName(CPoint C);
private:
	CMFCPropertyGridProperty* m_EntryX;
	CMFCPropertyGridProperty* m_EntryY0;
	CMFCPropertyGridProperty* m_EntryY1;
public:
	// Data Transmission
	void LoadFromData(sgp::ParticleEntryParam& data);
	void SaveToData(sgp::ParticleEntryParam& data);
	bool m_bRGBA;
	bool m_bAngle;
	void SetIsRGBA(){m_bRGBA=true;}
	void SetIsAngle(){m_bAngle=true;}
};