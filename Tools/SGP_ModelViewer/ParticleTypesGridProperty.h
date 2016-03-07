#pragma once

enum Property_Type
{
	Property_ZoneType,
	Property_EmitterType,
	Property_ModifierType,
	Property_RenderType,
	Property_ModelParamType,
	Property_InterpolatorType,
	Property_OrientationType
};

class CParticleTypesGridProperty:public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CParticleTypesGridProperty);
public:
	CParticleTypesGridProperty(const CString& strName, const COleVariant& varValue);
	virtual BOOL OnUpdateValue();
	virtual void OnCloseCombo();
	void SetType(Property_Type type){m_Type=type;}
private:
	Property_Type m_Type;
};