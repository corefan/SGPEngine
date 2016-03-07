#include "stdafx.h"

#include "ParticleTypesGridProperty.h"
#include "ParticleZoneGridProperty.h"
#include "ParticleEmitterGridProperty.h"
#include "ParticleModifierGridProperty.h"
#include "ParticleRenderGridProperty.h"
#include "ParticleModelParamGridProperty.h"

IMPLEMENT_DYNAMIC(CParticleTypesGridProperty,CMFCPropertyGridProperty)

CParticleTypesGridProperty::CParticleTypesGridProperty(const CString& strName, const COleVariant& varValue):\
	CMFCPropertyGridProperty(strName,varValue)
{

}

BOOL CParticleTypesGridProperty::OnUpdateValue()
{
	BOOL bResult=CMFCPropertyGridProperty::OnUpdateValue();
/*	CString sNowText;
	m_pWndInPlace->GetWindowText(sNowText);
	switch(m_Type)
	{
	case Property_ZoneType:
		((CParticleZoneGridProperty*)GetParent())->SetZoneType(sNowText);
		break;
	case Property_EmitterType:
		((CParticleEmitterGridProperty*)GetParent())->SetEmitterType(sNowText);
		break;
	case Property_ModifierType:
		((CParticleModifierGridProperty*)GetParent())->SetModifierType(sNowText);
		break;
	case Property_RenderType:
		((CParticleRenderGridProperty*)GetParent())->SetRenderType(sNowText);
		break;
	case Property_ModelParamType:
		((CParticleModelParamGridProperty*)GetParent())->SetModelParamType(sNowText);
		break;
	case Property_InterpolatorType:
		((CParticleModelParamGridProperty*)GetParent())->SetInterpolatorType(sNowText);
		break;
	case Property_OrientationType:
		((CParticleRenderGridProperty*)GetParent())->SetOrientationType(sNowText);
		break;
	}*/

	return bResult;
}

void CParticleTypesGridProperty::OnCloseCombo()
{
	CString sNowText;
	m_pWndInPlace->GetWindowText(sNowText);
	switch(m_Type)
	{
	case Property_ZoneType:
		((CParticleZoneGridProperty*)GetParent())->SetZoneType(sNowText);
		break;
	case Property_EmitterType:
		((CParticleEmitterGridProperty*)GetParent())->SetEmitterType(sNowText);
		break;
	case Property_ModifierType:
		((CParticleModifierGridProperty*)GetParent())->SetModifierType(sNowText);
		break;
	case Property_RenderType:
		((CParticleRenderGridProperty*)GetParent())->SetRenderType(sNowText);
		break;
	case Property_ModelParamType:
		((CParticleModelParamGridProperty*)GetParent())->SetModelParamType(sNowText);
		break;
	case Property_InterpolatorType:
		((CParticleModelParamGridProperty*)GetParent())->SetInterpolatorType(sNowText);
		break;
	case Property_OrientationType:
		((CParticleRenderGridProperty*)GetParent())->SetOrientationType(sNowText);
		break;
	}
}