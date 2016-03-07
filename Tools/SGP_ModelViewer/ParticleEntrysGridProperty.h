#pragma once

#include <vector>

namespace sgp
{
	struct ParticleSelfDefInterpolatorData;
}

class CParticleEntryGridProperty;

class CParticleEntrysGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleEntrysGridProperty)
public:
	CParticleEntrysGridProperty(const CString& strName);
	void Init();
	virtual void OnRClickName(CPoint C);
	void AddNewEntry();
	void DeleteEntry(CParticleEntryGridProperty* pEntry);
private:
	std::vector<CParticleEntryGridProperty*> m_EntryVector;
	int m_nNewEntryID;
public:
	// Data Transmission
	void LoadFromData(sgp::ParticleSelfDefInterpolatorData& data);
	void SaveToData(sgp::ParticleSelfDefInterpolatorData& data);
	bool m_bRGBA;
	bool m_bAngle;
	void SetIsRGBA(){m_bRGBA=true;}
	void SetIsAngle(){m_bAngle=true;}
};