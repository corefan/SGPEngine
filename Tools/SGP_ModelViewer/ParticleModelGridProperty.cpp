#include "stdafx.h"

#include "ParticleModelGridProperty.h"
#include "ParticleModelParamGridProperty.h"
#include "ParticleCheckGridProperty.h"
#include "../SGPLibraryCode/SGPHeader.h"

static CString s_ModelParam[]=
{
	"Red Channel",
	"Green Channel",
	"Blue Channel",
	"Alpha Channel",
	"Size",
	"Mass",
	"Angle",
	"Texure Index",
	"Rotation Speed"
};

static BOOL s_bRGBA[]=
{
	TRUE,
	TRUE,
	TRUE,
	TRUE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE
};

static CString s_ParamDescription[]=
{
	"Range : [0,255]",
	"Range : [0,255]",
	"Range : [0,255]",
	"Range : [0,255]",
	"",
	"",
	"Range : [0,360]",
	"",
	""
};

static float DEFAULT_VALUE[]=
{
	1.0f,	// RED
	1.0f,	// GREEN
	1.0f,	// BLUE
	1.0f,	// ALPHA
	1.0f,	// SIZE
	1.0f,	// MASS
	0.0f,	// ANGLE
	0.0f,	// TEXTURE_INDEX
	0.0f,	// ROTATION_SPEED
	0.0f,	// CUSTOM_0
	0.0f,	// CUSTOM_1
	0.0f	// CUSTOM_2
};

IMPLEMENT_DYNAMIC(CParticleModelGridProperty,CMFCPropertyGridProperty)

CParticleModelGridProperty::CParticleModelGridProperty(const CString& strName):\
	CMFCPropertyGridProperty(strName)
{
}

void CParticleModelGridProperty::Init()
{
	AddSubItem(m_MinLifeProperty=new CMFCPropertyGridProperty("Min Lifetime",COleVariant(1.0f)));
	AddSubItem(m_MaxLifeProperty=new CMFCPropertyGridProperty("Max Lifetime",COleVariant(1.0f)));
	AddSubItem(m_ImmortalProperty=new CParticleCheckGridProperty("Immortal",false));

	for(int i=0;i<sizeof(s_ModelParam)/sizeof(CString);++i)
	{
		CParticleModelParamGridProperty* pModelParam=new CParticleModelParamGridProperty(s_ModelParam[i],i,DEFAULT_VALUE[i]);
		AddSubItem(pModelParam);

		if(s_ModelParam[i]=="Angle") pModelParam->SetAngle();
		pModelParam->SetRGBA(s_bRGBA[i]);
		pModelParam->SetDescriptionStr(s_ParamDescription[i]);

		if(i<3) pModelParam->Init(TRUE);
		else pModelParam->Init(FALSE);
		m_ParamVector.push_back(pModelParam);
	}
	Expand(FALSE);
}

void CParticleModelGridProperty::LoadFromData(ParticleModelParam& data)
{
	AddSubItem(m_MinLifeProperty=new CMFCPropertyGridProperty("Min Lifetime",COleVariant(data.m_LifeTimeMin)));
	AddSubItem(m_MaxLifeProperty=new CMFCPropertyGridProperty("Max Lifetime",COleVariant(data.m_LifeTimeMax)));
	AddSubItem(m_ImmortalProperty=new CParticleCheckGridProperty("Immortal",data.m_bImmortal));

	uint32 j;
	for(int i=0;i<sizeof(s_ModelParam)/sizeof(CString);++i)
	{
		CParticleModelParamGridProperty* pParamProperty=new CParticleModelParamGridProperty(s_ModelParam[i],i,DEFAULT_VALUE[i]);
		AddSubItem(pParamProperty);

		if(s_ModelParam[i]=="Angle") pParamProperty->SetAngle();
		pParamProperty->SetRGBA(s_bRGBA[i]);
		pParamProperty->SetDescriptionStr(s_ParamDescription[i]);

		BOOL bFinded=FALSE;
		for(j=0;j<data.m_ParamCount;++j)
		{
			if(data.m_pRegularParam[j].m_ModelParam==i)
			{
				bFinded=TRUE;
				pParamProperty->LoadFromData(data.m_pRegularParam[j],data);
				break;
			}
		}
		for(j=0;j<data.m_InterpolatorCount&&(!bFinded);++j)
		{
			if(data.m_pInterpolatorParam[j].m_ModelParam==i)
			{
				bFinded=TRUE;
				pParamProperty->LoadFromData(data.m_pInterpolatorParam[j],data);
				break;
			}
		}
		if(!bFinded) pParamProperty->Init(FALSE);
		m_ParamVector.push_back(pParamProperty);
	}

	Expand(FALSE);
}

void CParticleModelGridProperty::SaveToData(ParticleModelParam& data)
{
	data.m_LifeTimeMin=m_MinLifeProperty->GetValue().fltVal;
	data.m_LifeTimeMax=m_MaxLifeProperty->GetValue().fltVal;
	data.m_bImmortal=m_ImmortalProperty->GetCheck();


	uint32 paramCount=0,interpolatorCount=0;
	uint32 i,size=m_ParamVector.size();
	for(i=0;i<size;++i)
	{
		if(m_ParamVector[i]->IsInterpolator()) ++interpolatorCount;
		else if(m_ParamVector[i]->IsNormalParam()) ++paramCount;
	}
	
	data.m_EnableFlag=0;
	data.m_MutableFlag=0;
	data.m_RandomFlag=0;
	data.m_InterpolatedFlag=0;
	data.m_ParamCount=paramCount;
	data.m_InterpolatorCount=interpolatorCount;
	
	if(paramCount!=0)
	{
		data.m_pRegularParam=new ParticleRegularParam[paramCount];
		uint32 index=0;
		for(i=0;i<size;++i)
		{
			if(m_ParamVector[i]->IsNormalParam())
			{
				ASSERT(index<paramCount);
				m_ParamVector[i]->SaveToData(data.m_pRegularParam[index++],data);
			}
		}
	}
	else data.m_pRegularParam=NULL;
	
	if(interpolatorCount!=0)
	{
		data.m_pInterpolatorParam=new ParticleInterpolatorParam[interpolatorCount];
		uint32 index=0;
		for(i=0;i<size;++i)
		{
			if(m_ParamVector[i]->IsInterpolator())
			{
				ASSERT(index<interpolatorCount);
				m_ParamVector[i]->SaveToData(data.m_pInterpolatorParam[index++],data);
			}
		}
	}
	else data.m_pInterpolatorParam=NULL;
}