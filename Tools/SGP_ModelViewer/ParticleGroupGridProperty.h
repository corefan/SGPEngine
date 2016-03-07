#pragma once

namespace sgp
{
	struct ParticleGroupParam;
}
class CParticleCheckGridProperty;
class CParticleModelGridProperty;
class CParticleRenderGridProperty;
class CParticleEmittersGridProperty;
class CParticleModifiersGridProperty;

class CParticleGroupGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleGroupGridProperty);
public:
	CParticleGroupGridProperty(const CString& strName);
	void Init();
	virtual void OnRClickName(CPoint C);
private:
	CParticleCheckGridProperty* m_ComputeBoundBoxProperty;
	CParticleCheckGridProperty* m_ComputeDistanceProperty;
	CParticleCheckGridProperty* m_EnableSortingProperty;
	CMFCPropertyGridProperty* m_GravityXProperty;
	CMFCPropertyGridProperty* m_GravityYProperty;
	CMFCPropertyGridProperty* m_GravityZProperty;
	CMFCPropertyGridProperty* m_CapacityProperty;
	CMFCPropertyGridProperty* m_FrictionProperty;
	CParticleModelGridProperty* m_ModelProperty;
	CParticleRenderGridProperty* m_RenderProperty;
	CParticleEmittersGridProperty* m_EmittersProperty;
	CParticleModifiersGridProperty* m_ModifiersProperty;
public:
	CParticleCheckGridProperty* m_ShowProperty;
public:
	void LoadFromData(sgp::ParticleGroupParam& data);
	void SaveToData(sgp::ParticleGroupParam& data);
};