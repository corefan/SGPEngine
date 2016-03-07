#pragma once

class CParticleCheckGridProperty;
class CParticleModelParamGridProperty;

namespace sgp
{
	struct ParticleModelParam;
}

#include <vector>

class CParticleModelGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleModelGridProperty);
public:
	CParticleModelGridProperty(const CString& strName);
	void Init();
private:
	CParticleCheckGridProperty* m_ImmortalProperty;
	CMFCPropertyGridProperty* m_MinLifeProperty;
	CMFCPropertyGridProperty* m_MaxLifeProperty;
	std::vector<CParticleModelParamGridProperty*> m_ParamVector;
public:
	// Data Transmission
	void LoadFromData(sgp::ParticleModelParam& data);
	void SaveToData(sgp::ParticleModelParam& data);
};