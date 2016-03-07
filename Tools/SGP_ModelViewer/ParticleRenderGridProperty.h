#pragma once

#include <vector>

namespace sgp
{
	struct ParticleRenderParam;
}

class CParticleRenderGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleRenderGridProperty);
public:
	CParticleRenderGridProperty(const CString& strName);
	void SetRenderType(CString sType,BOOL bNotify=TRUE);
	void SetOrientationType(CString sType,BOOL bNotify=TRUE);
	void Init();
private:
	std::vector<CMFCPropertyGridProperty*> m_PropertyVector;
	std::vector<CMFCPropertyGridProperty*> m_OrientationVector;
	CString m_sRenderType;
	CString m_sOrientType;
	CMFCPropertyGridProperty* m_BlendModeProperty;
public:
	void LoadFromData(sgp::ParticleRenderParam& data);
	void SaveToData(sgp::ParticleRenderParam& data);
};