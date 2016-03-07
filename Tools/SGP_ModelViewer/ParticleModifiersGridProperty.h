#pragma once

#include <vector>

namespace sgp
{
	struct ParticleGroupParam;
}

class CParticleModifierGridProperty;

class CParticleModifiersGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleModifiersGridProperty)
public:
	CParticleModifiersGridProperty(const CString& strName);
	virtual void OnRClickName(CPoint C);
	void AddNewModifier();
	void DeleteModifier(CParticleModifierGridProperty* pProperty);
	void Init();
private:
	std::vector<CParticleModifierGridProperty*> m_ModifierVector;
	int m_nNewModifierID;
public:
	void LoadFromData(sgp::ParticleGroupParam& data);
	void SaveToData(sgp::ParticleGroupParam& data);
};