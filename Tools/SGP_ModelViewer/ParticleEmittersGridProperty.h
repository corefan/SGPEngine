#pragma once

#include <vector>
class CParticleEmitterGridProperty;
namespace sgp
{
	struct ParticleGroupParam;
}

class CParticleEmittersGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleEmittersGridProperty)
public:
	CParticleEmittersGridProperty(const CString& strName);
	virtual void OnRClickName(CPoint C);
	void AddNewEmitter();
	void DeleteEmitter(CParticleEmitterGridProperty* pProperty);
	void Init();
private:
	std::vector<CParticleEmitterGridProperty*> m_EmitterVector;
	int m_nNewEmitterID;
public:
	//Data Transmission
	void LoadFromData(sgp::ParticleGroupParam& data);
	void SaveToData(sgp::ParticleGroupParam& data);
};