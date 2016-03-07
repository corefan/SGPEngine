#include "stdafx.h"

#include "ParticleModelParamGridProperty.h"
#include "ParticleTypesGridProperty.h"
#include "ParticleEntrysGridProperty.h"
#include "ParticleCtrlMsgDef.h"
#include "../SGPLibraryCode/SGPHeader.h"
#include "ParticleInterpolatorGraphProperty.h"

IMPLEMENT_DYNAMIC(CParticleModelParamGridProperty,CMFCPropertyGridProperty)

CParticleModelParamGridProperty::CParticleModelParamGridProperty(const CString& strName,int index,float fDefaultVal):\
	CMFCPropertyGridProperty(strName),\
	m_Index(index),\
	m_fDefaultVal(fDefaultVal)
{
	m_sInterpolatorType="";
	m_bAngle=FALSE;
	m_bRGBA=FALSE;
}

void CParticleModelParamGridProperty::SetModelParamType(CString sType,BOOL bNotify)
{
	if(m_sParamType==sType&&bNotify) return ;

	unsigned int i;
	for(i=0;i<m_PropertyVector.size();++i) RemoveSubItem(m_PropertyVector[i]);
	m_PropertyVector.clear();

	if(sType=="Default")
	{
		CMFCPropertyGridProperty *pDefaultValue;
		if(m_bRGBA) pDefaultValue=new CMFCPropertyGridProperty("Default Value",COleVariant((long)(m_fDefaultVal*255)));
		else if(m_bAngle) pDefaultValue=new CMFCPropertyGridProperty("Default Value",COleVariant((long)(m_fDefaultVal*180/float_Pi)));
		else pDefaultValue=new CMFCPropertyGridProperty("Default Value",COleVariant(m_fDefaultVal));
		pDefaultValue->AllowEdit(FALSE);
		m_PropertyVector.push_back(pDefaultValue);

		m_pInterpolatorType->Show(FALSE,FALSE);
		for(i=0;i<m_InterpolatorVector.size();++i) RemoveSubItem(m_InterpolatorVector[i]);
		m_InterpolatorVector.clear();
	}
	else if(sType=="Constant")
	{
		CMFCPropertyGridProperty* pValue;
		if(m_bRGBA) pValue=new CMFCPropertyGridProperty("Value",COleVariant((long)(m_fDefaultVal*255)),m_sDescription);
		else if(m_bAngle) pValue=new CMFCPropertyGridProperty("Value",COleVariant((long)(m_fDefaultVal*180/float_Pi)),m_sDescription);
		else pValue=new CMFCPropertyGridProperty("Value",COleVariant(m_fDefaultVal),m_sDescription);
		m_PropertyVector.push_back(pValue);
		m_pInterpolatorType->Show(FALSE,FALSE);
		for(i=0;i<m_InterpolatorVector.size();++i) RemoveSubItem(m_InterpolatorVector[i]);
		m_InterpolatorVector.clear();
	}
	else if(sType=="Random")
	{
		if(m_bRGBA)
		{
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Min Value",COleVariant((long)(m_fDefaultVal*255)),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Max Value",COleVariant((long)(m_fDefaultVal*255)),m_sDescription));
		}
		else if(m_bAngle)
		{
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Min Value",COleVariant((long)(m_fDefaultVal*180/float_Pi)),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Max Value",COleVariant((long)(m_fDefaultVal*180/float_Pi)),m_sDescription));
		}
		else
		{
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Min Value",COleVariant(m_fDefaultVal),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Max Value",COleVariant(m_fDefaultVal),m_sDescription));
		}
		m_pInterpolatorType->Show(FALSE,FALSE);
		for(i=0;i<m_InterpolatorVector.size();++i) RemoveSubItem(m_InterpolatorVector[i]);
		m_InterpolatorVector.clear();
	}
	else if(sType=="Mutable")
	{
		if(m_bRGBA)
		{
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth Value",COleVariant((long)(m_fDefaultVal*255)),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death Value",COleVariant((long)(m_fDefaultVal*255)),m_sDescription));
		}
		else if(m_bAngle)
		{
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth Value",COleVariant((long)(m_fDefaultVal*180/float_Pi)),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death Value",COleVariant((long)(m_fDefaultVal*180/float_Pi)),m_sDescription));
		}
		else
		{
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth Value",COleVariant(m_fDefaultVal),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death Value",COleVariant(m_fDefaultVal),m_sDescription));
		}
		m_pInterpolatorType->Show(FALSE,FALSE);
		for(i=0;i<m_InterpolatorVector.size();++i) RemoveSubItem(m_InterpolatorVector[i]);
		m_InterpolatorVector.clear();
	}
	else if(sType=="Mutable And Random")
	{
		if(m_bRGBA)
		{
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MinVal",COleVariant((long)(m_fDefaultVal*255)),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MaxVal",COleVariant((long)(m_fDefaultVal*255)),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MinVal",COleVariant((long)(m_fDefaultVal*255)),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MaxVal",COleVariant((long)(m_fDefaultVal*255)),m_sDescription));
		}
		else if(m_bAngle)
		{
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MinVal",COleVariant((long)(m_fDefaultVal*180/float_Pi)),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MaxVal",COleVariant((long)(m_fDefaultVal*180/float_Pi)),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MinVal",COleVariant((long)(m_fDefaultVal*180/float_Pi)),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MaxVal",COleVariant((long)(m_fDefaultVal*180/float_Pi)),m_sDescription));
		}
		else
		{
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MinVal",COleVariant(m_fDefaultVal),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MaxVal",COleVariant(m_fDefaultVal),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MinVal",COleVariant(m_fDefaultVal),m_sDescription));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MaxVal",COleVariant(m_fDefaultVal),m_sDescription));
		}
		m_pInterpolatorType->Show(FALSE,FALSE);
		for(i=0;i<m_InterpolatorVector.size();++i) RemoveSubItem(m_InterpolatorVector[i]);
		m_InterpolatorVector.clear();
	}
	else if(sType=="Interpolator")
	{
		m_pInterpolatorType->Show(TRUE,FALSE);
		SetInterpolatorType(m_pInterpolatorType->GetValue(),FALSE);
	}
	else ASSERT(FALSE);

	m_sParamType=sType;
	for(i=0;i<m_PropertyVector.size();++i) AddSubItem(m_PropertyVector[i]);
	if(bNotify) m_pWndList->GetParent()->PostMessage(UM_UPDATE_PROPERTYCTRL);
}

void CParticleModelParamGridProperty::SetInterpolatorType(CString sType,BOOL bNotify)
{
	if(sType==m_sInterpolatorType&&bNotify) return ;

	unsigned int i;
	for(i=0;i<m_InterpolatorVector.size();++i) RemoveSubItem(m_InterpolatorVector[i]);
	m_InterpolatorVector.clear();

	float ratio=1.0f;
	if(m_bRGBA) ratio=255.0f;
	else if(m_bAngle) ratio=180.0f;

	if(sType=="SelfDefine")
	{
		CParticleEntrysGridProperty* pEntrysProperty=new CParticleEntrysGridProperty("Entrys");
		if(m_bRGBA) pEntrysProperty->SetIsRGBA();
		else if(m_bAngle) pEntrysProperty->SetIsAngle();
		pEntrysProperty->SetDescription("Right click mouse can popup menu to add new entry.");
		m_InterpolatorVector.push_back(pEntrysProperty);
		m_InterpolatorVector.push_back(new CParticleInterpolatorGraphProperty("Show Graph",COleVariant("")));
	}
	else if(sType=="Sinusoidal")
	{
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Period",COleVariant(1.0f)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Min Amplitude",COleVariant(1.0f*ratio)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Max Amplitude",COleVariant(1.0f*ratio)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("X Offset",COleVariant(0.0f)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Y Offset",COleVariant(0.0f*ratio)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("X Start",COleVariant(0.0f)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Length",COleVariant((long)1)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Samples",COleVariant((long)2),"This value must >=2"));
		m_InterpolatorVector.push_back(new CParticleInterpolatorGraphProperty("Show Graph",COleVariant("")));
	}
	else if(sType=="Polynomial")
	{
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Constant Value",COleVariant(0.0f*ratio)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Linear Value",COleVariant(1.0f*ratio)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Quad Value",COleVariant(0.0f*ratio)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Cubic Value",COleVariant(0.0f*ratio)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("X Start",COleVariant(0.0f)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("X End",COleVariant(1.0f)));
		m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Samples",COleVariant((long)2),"This value must >=2"));
		m_InterpolatorVector.push_back(new CParticleInterpolatorGraphProperty("Show Graph",COleVariant("")));
	}

	for(i=0;i<m_InterpolatorVector.size();++i) AddSubItem(m_InterpolatorVector[i]);
	if(sType=="SelfDefine")
	{
		for(i=0;i<m_InterpolatorVector.size()-1;++i)
		{
			((CParticleEntrysGridProperty*)m_InterpolatorVector[i])->Init();
		}
	}
	m_sInterpolatorType=sType;

	if(bNotify) m_pWndList->GetParent()->PostMessage(UM_UPDATE_PROPERTYCTRL);
}

void CParticleModelParamGridProperty::Init(BOOL bEnable)
{
	if(bEnable) m_sParamType="Constant";
	else m_sParamType="Default";

	// Param Type
	CParticleTypesGridProperty* pType=new CParticleTypesGridProperty("Type",m_sParamType);
	pType->AddOption("Default");
	pType->AddOption("Constant");
	pType->AddOption("Mutable");
	pType->AddOption("Random");
	pType->AddOption("Mutable And Random");
	pType->AddOption("Interpolator");
	pType->AllowEdit(FALSE);
	pType->SetDescription("Choose param type: Default、Constant、Mutable、Random、Mutable And Random、Interpolator.");
	pType->SetType(Property_ModelParamType);
	AddSubItem(pType);

	// Interpolator Type
	m_pInterpolatorType=new CParticleTypesGridProperty("Interpolator Type","SelfDefine");
	m_pInterpolatorType->AddOption("SelfDefine");
	m_pInterpolatorType->AddOption("Sinusoidal");
	m_pInterpolatorType->AddOption("Polynomial");
	m_pInterpolatorType->AllowEdit(FALSE);
	m_pInterpolatorType->SetDescription("Choose interpolator type: Self Define、Sinusoidal、Polynomial.");
	m_pInterpolatorType->SetType(Property_InterpolatorType);
	AddSubItem(m_pInterpolatorType);
//	m_InterpolatorVector.push_back(pInterpolatorType);

	SetModelParamType(m_sParamType,FALSE);
	Expand(FALSE);
}

void CParticleModelParamGridProperty::LoadFromData(ParticleRegularParam& data,ParticleModelParam& model)
{
	m_PropertyVector.clear();
	m_InterpolatorVector.clear();
	uint32 flag=(1<<m_Index);
	switch(data.m_count)
	{
	case 1:
		{
			ASSERT(flag&model.m_EnableFlag);
			ASSERT((flag&model.m_MutableFlag)==0);
			ASSERT((flag&model.m_RandomFlag)==0);
			ASSERT((flag&model.m_InterpolatedFlag)==0);
			m_sParamType="Constant";
			if(m_bRGBA) m_PropertyVector.push_back(new CMFCPropertyGridProperty("Value",COleVariant((long)(data.m_data[0]*255)),m_sDescription));
			else if(m_bAngle) m_PropertyVector.push_back(new CMFCPropertyGridProperty("Value",COleVariant((long)(data.m_data[0]*180/float_Pi)),m_sDescription));
			else m_PropertyVector.push_back(new CMFCPropertyGridProperty("Value",COleVariant(data.m_data[0]),m_sDescription));
		}
		break;
	case 2:
		{
			ASSERT(flag&model.m_EnableFlag);
			ASSERT((flag&model.m_InterpolatedFlag)==0);
			if((flag&model.m_MutableFlag)!=0)// Mutable
			{
				ASSERT((flag&model.m_RandomFlag)==0);
				m_sParamType="Mutable";
				if(m_bRGBA)
				{
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth Value",COleVariant((long)(data.m_data[0]*255)),m_sDescription));
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death Value",COleVariant((long)(data.m_data[1]*255)),m_sDescription));
				}
				else if(m_bAngle)
				{
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth Value",COleVariant((long)(data.m_data[0]*180/float_Pi)),m_sDescription));
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death Value",COleVariant((long)(data.m_data[1]*180/float_Pi)),m_sDescription));
				}
				else
				{
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth Value",COleVariant(data.m_data[0]),m_sDescription));
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death Value",COleVariant(data.m_data[1]),m_sDescription));
				}
			}
			else// Random
			{
				ASSERT((flag&model.m_MutableFlag)==0);
				ASSERT(flag&model.m_RandomFlag);
				m_sParamType="Random";
				if(m_bRGBA)
				{
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Min Value",COleVariant((long)(data.m_data[0]*255)),m_sDescription));
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Max Value",COleVariant((long)(data.m_data[1]*255)),m_sDescription));
				}
				else if(m_bAngle)
				{
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Min Value",COleVariant((long)(data.m_data[0]*180/float_Pi)),m_sDescription));
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Max Value",COleVariant((long)(data.m_data[1]*180/float_Pi)),m_sDescription));
				}
				else
				{
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Min Value",COleVariant(data.m_data[0]),m_sDescription));
					m_PropertyVector.push_back(new CMFCPropertyGridProperty("Max Value",COleVariant(data.m_data[1]),m_sDescription));
				}
			}
		}
		break;
	case 4:
		{
			ASSERT(flag&model.m_EnableFlag);
			ASSERT(flag&model.m_MutableFlag);
			ASSERT(flag&model.m_RandomFlag);
			ASSERT((flag&model.m_InterpolatedFlag)==0);
			m_sParamType="Mutable And Random";
			if(m_bRGBA)
			{
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MinVal",COleVariant((long)(data.m_data[0]*255)),m_sDescription));
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MaxVal",COleVariant((long)(data.m_data[1]*255)),m_sDescription));
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MinVal",COleVariant((long)(data.m_data[2]*255)),m_sDescription));
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MaxVal",COleVariant((long)(data.m_data[3]*255)),m_sDescription));
			}
			else if(m_bAngle)
			{
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MinVal",COleVariant((long)(data.m_data[0]*180/float_Pi)),m_sDescription));
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MaxVal",COleVariant((long)(data.m_data[1]*180/float_Pi)),m_sDescription));
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MinVal",COleVariant((long)(data.m_data[2]*180/float_Pi)),m_sDescription));
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MaxVal",COleVariant((long)(data.m_data[3]*180/float_Pi)),m_sDescription));
			}
			else
			{
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MinVal",COleVariant(data.m_data[0]),m_sDescription));
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Birth MaxVal",COleVariant(data.m_data[1]),m_sDescription));
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MinVal",COleVariant(data.m_data[2]),m_sDescription));
				m_PropertyVector.push_back(new CMFCPropertyGridProperty("Death MaxVal",COleVariant(data.m_data[3]),m_sDescription));
			}
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	
	// Param Type
	CParticleTypesGridProperty* pType=new CParticleTypesGridProperty("Type",m_sParamType);
	pType->AddOption("Default");
	pType->AddOption("Constant");
	pType->AddOption("Mutable");
	pType->AddOption("Random");
	pType->AddOption("Mutable And Random");
	pType->AddOption("Interpolator");
	pType->AllowEdit(FALSE);
	pType->SetDescription("Choose param type: Default、Constant、Mutable、Random、Mutable And Random、Interpolator.");
	pType->SetType(Property_ModelParamType);
	AddSubItem(pType);

	// Interpolator Type
	m_pInterpolatorType=new CParticleTypesGridProperty("Interpolator Type","SelfDefine");
	m_pInterpolatorType->AddOption("SelfDefine");
	m_pInterpolatorType->AddOption("Sinusoidal");
	m_pInterpolatorType->AddOption("Polynomial");
	m_pInterpolatorType->AllowEdit(FALSE);
	m_pInterpolatorType->SetDescription("Choose interpolator type: Self Define、Sinusoidal、Polynomial.");
	m_pInterpolatorType->SetType(Property_InterpolatorType);
	AddSubItem(m_pInterpolatorType);
	m_pInterpolatorType->Show(FALSE,FALSE);

	for(uint32 i=0;i<m_PropertyVector.size();++i) AddSubItem(m_PropertyVector[i]);

	Expand(FALSE);
}

void CParticleModelParamGridProperty::LoadFromData(ParticleInterpolatorParam& data,ParticleModelParam& model)
{
	uint32 flag=(1<<m_Index);
	ASSERT(flag&model.m_EnableFlag);
	ASSERT(flag&model.m_InterpolatedFlag);
	ASSERT((flag&model.m_RandomFlag)==0);
	ASSERT((flag&model.m_MutableFlag)==0);

	m_PropertyVector.clear();
	m_InterpolatorVector.clear();

	float ratio=1.0f;
	if(m_bRGBA) ratio=255.0f;
	else if(m_bAngle) ratio=180.0f/float_Pi;

	switch(data.m_InterpolatorType)
	{
	case Interpolator_SelfDefine:
		{
			m_sInterpolatorType="SelfDefine";
			ParticleSelfDefInterpolatorData& selfData=data.m_SelfDefData;
			CParticleEntrysGridProperty* pEntrysProperty=new CParticleEntrysGridProperty("Entrys");
			if(m_bRGBA) pEntrysProperty->SetIsRGBA();
			else if(m_bAngle) pEntrysProperty->SetIsAngle();
			pEntrysProperty->SetDescription("Right click mouse can popup menu to add new entry.");
			m_InterpolatorVector.push_back(pEntrysProperty);
			m_InterpolatorVector.push_back(new CParticleInterpolatorGraphProperty("Show Graph",COleVariant("")));
		}
		break;
	case Interpolator_Sinusoidal:
		{
			m_sInterpolatorType="Sinusoidal";
			ParticleSinInterpolatorData& sinData=data.m_SinData;
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Period",COleVariant(sinData.m_period)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Min Amplitude",COleVariant(sinData.m_amplitudeMin*ratio)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Max Amplitude",COleVariant(sinData.m_amplitudeMax*ratio)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("X Offset",COleVariant(sinData.m_offsetX)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Y Offset",COleVariant(sinData.m_offsetY*ratio)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("X Start",COleVariant(sinData.m_startX)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Length",COleVariant((long)sinData.m_length)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Samples",COleVariant((long)sinData.m_nbSamples)));
			m_InterpolatorVector.push_back(new CParticleInterpolatorGraphProperty("Show Graph",COleVariant("")));
		}
		break;
	case Interpolator_Polynomial:
		{
			m_sInterpolatorType="Polynomial";
			ParticlePolyInterpolatorData& polyData=data.m_PolyData;
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Constant Value",COleVariant(polyData.m_constant*ratio)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Linear Value",COleVariant(polyData.m_linear*ratio)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Quad Value",COleVariant(polyData.m_quadratic*ratio)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Cubic Value",COleVariant(polyData.m_cubic*ratio)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("X Start",COleVariant(polyData.m_startX)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("X End",COleVariant(polyData.m_endX)));
			m_InterpolatorVector.push_back(new CMFCPropertyGridProperty("Samples",COleVariant((long)polyData.m_nbSamples)));
			m_InterpolatorVector.push_back(new CParticleInterpolatorGraphProperty("Show Graph",COleVariant("")));
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	m_sParamType="Interpolator";
	// Param Type
	CParticleTypesGridProperty* pType=new CParticleTypesGridProperty("Type",m_sParamType);
	pType->AddOption("Default");
	pType->AddOption("Constant");
	pType->AddOption("Mutable");
	pType->AddOption("Random");
	pType->AddOption("Mutable And Random");
	pType->AddOption("Interpolator");
	pType->AllowEdit(FALSE);
	pType->SetDescription("Choose param type: Default、Constant、Mutable、Random、Mutable And Random、Interpolator.");
	pType->SetType(Property_ModelParamType);
	AddSubItem(pType);

	// Interpolator Type
	m_pInterpolatorType=new CParticleTypesGridProperty("Interpolator Type",m_sInterpolatorType);
	m_pInterpolatorType->AddOption("SelfDefine");
	m_pInterpolatorType->AddOption("Sinusoidal");
	m_pInterpolatorType->AddOption("Polynomial");
	m_pInterpolatorType->AllowEdit(FALSE);
	m_pInterpolatorType->SetDescription("Choose interpolator type: Self Define、Sinusoidal、Polynomial.");
	m_pInterpolatorType->SetType(Property_InterpolatorType);
	AddSubItem(m_pInterpolatorType);

	for(uint32 i=0;i<m_InterpolatorVector.size();++i) AddSubItem(m_InterpolatorVector[i]);

	if(m_sInterpolatorType=="SelfDefine")
		((CParticleEntrysGridProperty*)m_InterpolatorVector[0])->LoadFromData(data.m_SelfDefData);

	Expand(FALSE);
}

void CParticleModelParamGridProperty::SaveToData(ParticleRegularParam& data,ParticleModelParam& model)
{
	ASSERT(IsNormalParam());

	uint32 flag=(1<<m_Index);
	memset(data.m_data,0,sizeof(float)*4);
	if(m_sParamType=="Constant")
	{
		model.m_EnableFlag|=flag;
		ASSERT(m_PropertyVector.size()==1);
		data.m_ModelParam=(uint8)m_Index;
		data.m_count=1;
		if(m_bRGBA) data.m_data[0]=(float)m_PropertyVector[0]->GetValue().lVal/255.0f;
		else if(m_bAngle) data.m_data[0]=(float)m_PropertyVector[0]->GetValue().lVal*float_Pi/180.0f;
		else data.m_data[0]=m_PropertyVector[0]->GetValue().fltVal;
	}
	else if(m_sParamType=="Mutable")
	{
		model.m_EnableFlag|=flag;
		model.m_MutableFlag|=flag;
		ASSERT(m_PropertyVector.size()==2);
		data.m_ModelParam=(uint8)m_Index;
		data.m_count=2;
		if(m_bRGBA)
		{
			data.m_data[0]=(float)m_PropertyVector[0]->GetValue().lVal/255.0f;
			data.m_data[1]=(float)m_PropertyVector[1]->GetValue().lVal/255.0f;
		}
		else if(m_bAngle)
		{
			data.m_data[0]=(float)m_PropertyVector[0]->GetValue().lVal*float_Pi/180.0f;
			data.m_data[1]=(float)m_PropertyVector[1]->GetValue().lVal*float_Pi/180.0f;
		}
		else
		{
			data.m_data[0]=m_PropertyVector[0]->GetValue().fltVal;
			data.m_data[1]=m_PropertyVector[1]->GetValue().fltVal;
		}
	}
	else if(m_sParamType=="Random")
	{
		model.m_EnableFlag|=flag;
		model.m_RandomFlag|=flag;
		ASSERT(m_PropertyVector.size()==2);
		data.m_ModelParam=(uint8)m_Index;
		data.m_count=2;
		if(m_bRGBA)
		{
			data.m_data[0]=(float)m_PropertyVector[0]->GetValue().lVal/255.0f;
			data.m_data[1]=(float)m_PropertyVector[1]->GetValue().lVal/255.0f;
		}
		else if(m_bAngle)
		{
			data.m_data[0]=(float)m_PropertyVector[0]->GetValue().lVal*float_Pi/180.0f;
			data.m_data[1]=(float)m_PropertyVector[1]->GetValue().lVal*float_Pi/180.0f;
		}
		else
		{
			data.m_data[0]=m_PropertyVector[0]->GetValue().fltVal;
			data.m_data[1]=m_PropertyVector[1]->GetValue().fltVal;
		}
	}
	else if(m_sParamType=="Mutable And Random")
	{
		model.m_EnableFlag|=flag;
		model.m_RandomFlag|=flag;
		model.m_MutableFlag|=flag;
		ASSERT(m_PropertyVector.size()==4);
		data.m_ModelParam=(uint8)m_Index;
		data.m_count=4;
		if(m_bRGBA)
		{
			data.m_data[0]=(float)m_PropertyVector[0]->GetValue().lVal/255.0f;
			data.m_data[1]=(float)m_PropertyVector[1]->GetValue().lVal/255.0f;
			data.m_data[2]=(float)m_PropertyVector[2]->GetValue().lVal/255.0f;
			data.m_data[3]=(float)m_PropertyVector[3]->GetValue().lVal/255.0f;
		}
		else if(m_bAngle)
		{
			data.m_data[0]=(float)m_PropertyVector[0]->GetValue().lVal*float_Pi/180.0f;
			data.m_data[1]=(float)m_PropertyVector[1]->GetValue().lVal*float_Pi/180.0f;
			data.m_data[2]=(float)m_PropertyVector[2]->GetValue().lVal*float_Pi/180.0f;
			data.m_data[3]=(float)m_PropertyVector[3]->GetValue().lVal*float_Pi/180.0f;
		}
		else
		{
			data.m_data[0]=m_PropertyVector[0]->GetValue().fltVal;
			data.m_data[1]=m_PropertyVector[1]->GetValue().fltVal;
			data.m_data[2]=m_PropertyVector[2]->GetValue().fltVal;
			data.m_data[3]=m_PropertyVector[3]->GetValue().fltVal;
		}
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CParticleModelParamGridProperty::SaveToData(ParticleInterpolatorParam& data,ParticleModelParam& model)
{
	ASSERT(IsInterpolator());

	uint32 flag=(1<<m_Index);
	model.m_EnableFlag|=flag;
	model.m_InterpolatedFlag|=flag;
	data.m_ModelParam=(uint8)m_Index;

	float ratio=1.0f;
	if(m_bRGBA) ratio=1.0f/255.0f;
	else if(m_bAngle) ratio=float_Pi/180.0f;

	if(m_sInterpolatorType=="SelfDefine")
	{
		data.m_InterpolatorType=Interpolator_SelfDefine;
		ParticleSelfDefInterpolatorData& selfData=data.m_SelfDefData;
		ASSERT(m_InterpolatorVector.size()==2);
		((CParticleEntrysGridProperty*)m_InterpolatorVector[0])->SaveToData(selfData);
	}
	else if(m_sInterpolatorType=="Sinusoidal")
	{
		data.m_InterpolatorType=Interpolator_Sinusoidal;
		ASSERT(m_InterpolatorVector.size()==9);
		ParticleSinInterpolatorData& sinData=data.m_SinData;
		sinData.m_period=m_InterpolatorVector[0]->GetValue().fltVal;
		sinData.m_amplitudeMin=m_InterpolatorVector[1]->GetValue().fltVal*ratio;
		sinData.m_amplitudeMax=m_InterpolatorVector[2]->GetValue().fltVal*ratio;
		sinData.m_offsetX=m_InterpolatorVector[3]->GetValue().fltVal;
		sinData.m_offsetY=m_InterpolatorVector[4]->GetValue().fltVal*ratio;
		sinData.m_startX=m_InterpolatorVector[5]->GetValue().fltVal;
		sinData.m_length=m_InterpolatorVector[6]->GetValue().lVal;
		sinData.m_nbSamples=m_InterpolatorVector[7]->GetValue().lVal;
	}
	else if(m_sInterpolatorType=="Polynomial")
	{
		data.m_InterpolatorType=Interpolator_Polynomial;
		ASSERT(m_InterpolatorVector.size()==8);
		ParticlePolyInterpolatorData& polyData=data.m_PolyData;
		polyData.m_constant=m_InterpolatorVector[0]->GetValue().fltVal*ratio;
		polyData.m_linear=m_InterpolatorVector[1]->GetValue().fltVal*ratio;
		polyData.m_quadratic=m_InterpolatorVector[2]->GetValue().fltVal*ratio;
		polyData.m_cubic=m_InterpolatorVector[3]->GetValue().fltVal*ratio;
		polyData.m_startX=m_InterpolatorVector[4]->GetValue().fltVal;
		polyData.m_endX=m_InterpolatorVector[5]->GetValue().fltVal;
		polyData.m_nbSamples=m_InterpolatorVector[6]->GetValue().lVal;
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CParticleModelParamGridProperty::SaveToData(ParticleInterpolatorParam& data)
{
	if(m_sInterpolatorType=="SelfDefine")
	{
		data.m_InterpolatorType=Interpolator_SelfDefine;
		ParticleSelfDefInterpolatorData& selfData=data.m_SelfDefData;
		ASSERT(m_InterpolatorVector.size()==2);
		((CParticleEntrysGridProperty*)m_InterpolatorVector[0])->SaveToData(selfData);
		if(m_bRGBA)
		{
			for(uint32 i=0;i<selfData.m_count;++i)
			{
				selfData.m_pEntry[i].m_y0*=255.0f;
				selfData.m_pEntry[i].m_y1*=255.0f;
			}
		}
		else if(m_bAngle)
		{
			for(uint32 i=0;i<selfData.m_count;++i)
			{
				selfData.m_pEntry[i].m_y0*=180.0f/float_Pi;
				selfData.m_pEntry[i].m_y1*=180.0f/float_Pi;
			}
		}
	}
	else if(m_sInterpolatorType=="Sinusoidal")
	{
		data.m_InterpolatorType=Interpolator_Sinusoidal;
		ASSERT(m_InterpolatorVector.size()==9);
		ParticleSinInterpolatorData& sinData=data.m_SinData;
		sinData.m_period=m_InterpolatorVector[0]->GetValue().fltVal;
		sinData.m_amplitudeMin=m_InterpolatorVector[1]->GetValue().fltVal;
		sinData.m_amplitudeMax=m_InterpolatorVector[2]->GetValue().fltVal;
		sinData.m_offsetX=m_InterpolatorVector[3]->GetValue().fltVal;
		sinData.m_offsetY=m_InterpolatorVector[4]->GetValue().fltVal;
		sinData.m_startX=m_InterpolatorVector[5]->GetValue().fltVal;
		sinData.m_length=m_InterpolatorVector[6]->GetValue().lVal;
		sinData.m_nbSamples=m_InterpolatorVector[7]->GetValue().lVal;
	}
	else if(m_sInterpolatorType=="Polynomial")
	{
		data.m_InterpolatorType=Interpolator_Polynomial;
		ASSERT(m_InterpolatorVector.size()==8);
		ParticlePolyInterpolatorData& polyData=data.m_PolyData;
		polyData.m_constant=m_InterpolatorVector[0]->GetValue().fltVal;
		polyData.m_linear=m_InterpolatorVector[1]->GetValue().fltVal;
		polyData.m_quadratic=m_InterpolatorVector[2]->GetValue().fltVal;
		polyData.m_cubic=m_InterpolatorVector[3]->GetValue().fltVal;
		polyData.m_startX=m_InterpolatorVector[4]->GetValue().fltVal;
		polyData.m_endX=m_InterpolatorVector[5]->GetValue().fltVal;
		polyData.m_nbSamples=m_InterpolatorVector[6]->GetValue().lVal;
	}
	else
	{
		ASSERT(FALSE);
	}
}