#pragma once

#include <vector>
namespace sgp
{
	struct ParticleModifierParam;
}

class CParticleCheckGridProperty;
class CParticleZoneGridProperty;

class CParticleModifierGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleModifierGridProperty)
public:
	CParticleModifierGridProperty(const CString& strName);
	virtual void OnRClickName(CPoint C);

	void Init();
	void SetModifierType(CString sType,BOOL bNotify=TRUE);
	void ShowZoneProperty(bool bShow);
private:
	std::vector<CMFCPropertyGridProperty*> m_PropertyVector;
	CString m_sModifierType;
	CParticleCheckGridProperty* m_CustomZoneProperty;
	CParticleZoneGridProperty* m_ZoneProperty;
public:
	void LoadFromData(sgp::ParticleModifierParam& data);
	void SaveToData(sgp::ParticleModifierParam& data);
};