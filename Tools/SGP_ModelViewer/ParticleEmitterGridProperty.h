#pragma once

#include <vector>

namespace sgp
{
	struct ParticleEmitterParam;
}

class CParticleZoneGridProperty;

class CParticleEmitterGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleEmitterGridProperty);
public:
	CParticleEmitterGridProperty(const CString& strName);
	virtual void OnRClickName(CPoint C);
	void Init();
	void SetEmitterType(CString sType,BOOL bNotify=TRUE);
private:
	std::vector<CMFCPropertyGridProperty*> m_PropertyVector;
	CString m_sEmitterType;
	CMFCPropertyGridProperty* m_FlowProperty;
	CMFCPropertyGridProperty* m_TankProperty;
	CMFCPropertyGridProperty* m_MinForceProperty;
	CMFCPropertyGridProperty* m_MaxForceProperty;
	CParticleZoneGridProperty* m_ZoneProperty;
public:
	// Data Transmission
	void LoadFromData(sgp::ParticleEmitterParam& data);
	void SaveToData(sgp::ParticleEmitterParam& data);
};
