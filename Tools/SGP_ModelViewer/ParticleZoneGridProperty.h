#pragma once

#include <vector>

namespace sgp
{
	struct ParticleZoneParam;
}

class CParticleTypesGridProperty;
class CParticleCheckGridProperty;

class CParticleZoneGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleZoneGridProperty)
public:
	CParticleZoneGridProperty(const CString& strName);
	void SetZoneType(CString sType,BOOL bNotify=TRUE);
	void Init();
private:
	std::vector<CMFCPropertyGridProperty*> m_PropertyVector;
	CString m_sZoneType;
	CParticleTypesGridProperty* m_pTypeProperty;
	CParticleCheckGridProperty* m_pOptionProperty;
public:
	void LoadFromData(sgp::ParticleZoneParam& data);
	void SaveToData(sgp::ParticleZoneParam& data);
};