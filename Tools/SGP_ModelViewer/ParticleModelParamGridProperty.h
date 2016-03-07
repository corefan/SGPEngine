#pragma once

#include <vector>

namespace sgp
{
	struct ParticleModelParam;
	struct ParticleRegularParam;
	struct ParticleInterpolatorParam;
}

class CParticleTypesGridProperty;

class CParticleModelParamGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleModelParamGridProperty);
public:
	CParticleModelParamGridProperty(const CString& strName,int index,float fDefaultVal);
	void SetModelParamType(CString sType,BOOL bNotify=TRUE);
	void SetInterpolatorType(CString sType,BOOL bNotify=TRUE);
	void Init(BOOL bEnable);
private:
	std::vector<CMFCPropertyGridProperty*> m_PropertyVector;
	std::vector<CMFCPropertyGridProperty*> m_InterpolatorVector;
	CParticleTypesGridProperty* m_pInterpolatorType;
	CString m_sParamType;
	CString m_sInterpolatorType;
	int m_Index;
	float m_fDefaultVal;
	BOOL m_bAngle;//whether is angle param
	BOOL m_bRGBA;//whether is RGBA
	CString m_sDescription;
public:
	// Data Transmission
	void LoadFromData(sgp::ParticleRegularParam& data,sgp::ParticleModelParam& model);
	void LoadFromData(sgp::ParticleInterpolatorParam& data,sgp::ParticleModelParam& model);
	void SaveToData(sgp::ParticleRegularParam& data,sgp::ParticleModelParam& model);
	void SaveToData(sgp::ParticleInterpolatorParam& data,sgp::ParticleModelParam& model);
	BOOL IsInterpolator(){return m_sParamType=="Interpolator";}
	BOOL IsNormalParam(){return (m_sParamType!="Default"&&m_sParamType!="Interpolator");}
	int GetIndex(){return m_Index;}
	void SetAngle(){m_bAngle=TRUE;}
	void SetRGBA(BOOL bRGBA){m_bRGBA=bRGBA;}
	void SetDescriptionStr(CString str){m_sDescription=str;}
	void SaveToData(sgp::ParticleInterpolatorParam& data);
};