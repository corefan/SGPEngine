#include "stdafx.h"

#include "ParticleRenderGridProperty.h"
#include "ParticleTypesGridProperty.h"
#include "ParticleMenuGridProperty.h"
#include "ParticleCtrlMsgDef.h"
#include "../SGPLibraryCode/SGPHeader.h"

IMPLEMENT_DYNAMIC(CParticleRenderGridProperty,CMFCPropertyGridProperty)

CParticleRenderGridProperty::CParticleRenderGridProperty(const CString& strName):\
	CMFCPropertyGridProperty(strName)
{
}

void CParticleRenderGridProperty::SetRenderType(CString sType,BOOL bNotify)
{
	if(m_sRenderType==sType&&bNotify) return ;

	unsigned int i;
	for(i=0;i<m_PropertyVector.size();++i) RemoveSubItem(m_PropertyVector[i]);
	m_PropertyVector.clear();
	for(i=0;i<m_OrientationVector.size();++i) RemoveSubItem(m_OrientationVector[i]);
	m_OrientationVector.clear();

	if(sType=="Quad")
	{
		// Texture
		CParticleMenuGridProperty* pTextureProperty=new CParticleMenuGridProperty("Texture","");
		pTextureProperty->SetDescription("Right click mouse to use texture selected from Resource Explorer.");
		m_PropertyVector.push_back(pTextureProperty);
		// Scale
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Scale X",COleVariant(1.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Scale Y",COleVariant(1.0f)));
		// Dimension
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Tile X",COleVariant((long)1)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Tile Y",COleVariant((long)1)));
		// Orientation
		m_sOrientType="Default Aligned";
		CParticleTypesGridProperty *pOrientProp=new CParticleTypesGridProperty("Orientation",m_sOrientType);
		pOrientProp->AllowEdit(FALSE);
		pOrientProp->SetType(Property_OrientationType);
		pOrientProp->AddOption("Default Aligned");
		pOrientProp->AddOption("Camera Plane Aligned");
		pOrientProp->AddOption("Camera Point Aligned");
		pOrientProp->AddOption("Direction Aligned");
		pOrientProp->AddOption("Around Axis");
		pOrientProp->AddOption("Towards Point");
		pOrientProp->AddOption("Fixed Orientation");
		m_PropertyVector.push_back(pOrientProp);
	}
	else if(sType=="Point")
	{
		// Texture
		CParticleMenuGridProperty* pTextureProperty=new CParticleMenuGridProperty("Texture","");
		pTextureProperty->SetDescription("Right click mouse to use texture selected from Resource Explorer.");
		m_PropertyVector.push_back(pTextureProperty);
		// Point Size
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Point Size",COleVariant(1.0f)));
	}
	else if(sType=="Line")
	{
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Length",COleVariant(1.0f)));
		m_PropertyVector.push_back(new CMFCPropertyGridProperty("Width",COleVariant(1.0f)));
	}
	else
	{
		ASSERT(FALSE);
	}

	m_sRenderType=sType;
	for(i=0;i<m_PropertyVector.size();++i) AddSubItem(m_PropertyVector[i]);
	if(bNotify) m_pWndList->GetParent()->PostMessage(UM_UPDATE_PROPERTYCTRL);
}

void CParticleRenderGridProperty::Init()
{
	m_sRenderType="Quad";
	// Render Type
	CParticleTypesGridProperty* pRenderType=new CParticleTypesGridProperty("Render Type",m_sRenderType);
	pRenderType->SetType(Property_RenderType);
	pRenderType->AddOption("Quad");
	pRenderType->AddOption("Point");
	pRenderType->AddOption("Line");
	pRenderType->AllowEdit(FALSE);
	pRenderType->SetDescription("Choose render type : Quad¡¢Line or Point.");
	AddSubItem(pRenderType);

	//Blending Mode
	m_BlendModeProperty=new CMFCPropertyGridProperty("Blending Mode","Additive");
	m_BlendModeProperty->AllowEdit(FALSE);
	m_BlendModeProperty->AddOption("Additive");
	m_BlendModeProperty->AddOption("Alpha");
	m_BlendModeProperty->SetDescription("Choose blending mode : Additive or Alpha.");
	AddSubItem(m_BlendModeProperty);

	SetRenderType(m_sRenderType,FALSE);

	Expand(FALSE);
}

void CParticleRenderGridProperty::LoadFromData(ParticleRenderParam& data)
{
	m_PropertyVector.clear();
	switch(data.m_type)
	{
	case Render_Quad:
		{
			m_sRenderType="Quad";
			ParticleQuadRenderData& quadData=data.m_quadData;
			// Texture
			CParticleMenuGridProperty* pTextureProperty=new CParticleMenuGridProperty("Texture",quadData.m_texPath);
			pTextureProperty->SetDescription("Right click mouse to use texture selected from Resource Explorer.");
			m_PropertyVector.push_back(pTextureProperty);
			// Scale
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Scale X",COleVariant(quadData.m_xScale)));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Scale Y",COleVariant(quadData.m_yScale)));
			// Dimension
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Tile X",COleVariant((long)quadData.m_xDimension)));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Tile Y",COleVariant((long)quadData.m_yDimension)));
		}
		break;
	case Render_Line:
		{
			m_sRenderType="Line";
			ParticleLineRenderData& lineData=data.m_lineData;
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Length",COleVariant(lineData.m_length)));
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Width",COleVariant(lineData.m_width)));
		}
		break;
	case Render_Point:
		{
			m_sRenderType="Point";
			ParticlePointRenderData& pointData=data.m_pointData;
			// Texture
			CParticleMenuGridProperty* pTextureProperty=new CParticleMenuGridProperty("Texture",pointData.m_texPath);
			pTextureProperty->SetDescription("Right click mouse to use texture selected from Resource Explorer.");
			m_PropertyVector.push_back(pTextureProperty);
			// Point Size
			m_PropertyVector.push_back(new CMFCPropertyGridProperty("Point Size",COleVariant(pointData.m_size)));
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	// Render Type
	CParticleTypesGridProperty* pRenderType=new CParticleTypesGridProperty("Render Type",m_sRenderType);
	pRenderType->SetType(Property_RenderType);
	pRenderType->AddOption("Quad");
	pRenderType->AddOption("Point");
	pRenderType->AddOption("Line");
	pRenderType->AllowEdit(FALSE);
	pRenderType->SetDescription("Choose render type : Quad¡¢Line or Point.");
	AddSubItem(pRenderType);

	//Blending Mode
	CString sBlendMode;
	if(data.m_blendMode==SPARK::BLENDING_ADD) sBlendMode="Additive";
	else if(data.m_blendMode==SPARK::BLENDING_ALPHA) sBlendMode="Alpha";
	m_BlendModeProperty=new CMFCPropertyGridProperty("Blending Mode",sBlendMode);
	m_BlendModeProperty->AllowEdit(FALSE);
	m_BlendModeProperty->AddOption("Additive");
	m_BlendModeProperty->AddOption("Alpha");
	m_BlendModeProperty->SetDescription("Choose blending mode : Additive or Alpha.");
	AddSubItem(m_BlendModeProperty);

	for(uint32 i=0;i<m_PropertyVector.size();++i) AddSubItem(m_PropertyVector[i]);

	if(data.m_type==Render_Quad)
	{
		m_sOrientType.Empty();
		switch(data.m_quadData.m_nOrientation)
		{
		case SPARK::DEFAULT_ALIGNED:
			m_sOrientType="Default Aligned";
			break;
		case SPARK::CAMERA_PLANE_ALIGNED:
			m_sOrientType="Camera Plane Aligned";
			break;
		case SPARK::CAMERA_POINT_ALIGNED:
			m_sOrientType="Camera Point Aligned";
			break;
		case SPARK::DIRECTION_ALIGNED:
			m_sOrientType="Direction Aligned";
			break;
		case SPARK::AROUND_AXIS:
			m_sOrientType="Around Axis";
			break;
		case SPARK::TOWARDS_POINT:
			m_sOrientType="Towards Point";
			break;
		case SPARK::FIXED_ORIENTATION:
			m_sOrientType="Fixed Orientation";
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		
		CParticleTypesGridProperty *pOrientProp=new CParticleTypesGridProperty("Orientation",m_sOrientType);
		pOrientProp->AllowEdit(FALSE);
		pOrientProp->SetType(Property_OrientationType);
		pOrientProp->AddOption("Default Aligned");
		pOrientProp->AddOption("Camera Plane Aligned");
		pOrientProp->AddOption("Camera Point Aligned");
		pOrientProp->AddOption("Direction Aligned");
		pOrientProp->AddOption("Around Axis");
		pOrientProp->AddOption("Towards Point");
		pOrientProp->AddOption("Fixed Orientation");
		m_PropertyVector.push_back(pOrientProp);
		AddSubItem(pOrientProp);

		ParticleQuadRenderData& quadData=data.m_quadData;
		m_OrientationVector.clear();
		if(m_sOrientType=="Around Axis")
		{
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector X",COleVariant(quadData.m_lookVector[0])));
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Y",COleVariant(quadData.m_lookVector[1])));
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Z",COleVariant(quadData.m_lookVector[2])));
		}
		else if(m_sOrientType=="Towards Point")
		{
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector X",COleVariant(quadData.m_lookVector[0])));
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Y",COleVariant(quadData.m_lookVector[1])));
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Z",COleVariant(quadData.m_lookVector[2])));
		}
		else if(m_sOrientType=="Fixed Orientation")
		{
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector X",COleVariant(quadData.m_lookVector[0])));
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Y",COleVariant(quadData.m_lookVector[1])));
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Z",COleVariant(quadData.m_lookVector[2])));
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("up vector X",COleVariant(quadData.m_upVector[0])));
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("up vector Y",COleVariant(quadData.m_upVector[1])));
			m_OrientationVector.push_back(new CMFCPropertyGridProperty("up vector Z",COleVariant(quadData.m_upVector[2])));
		}
		for(unsigned int i=0;i<m_OrientationVector.size();++i) AddSubItem(m_OrientationVector[i]);
	}

	Expand(FALSE);
}

void CParticleRenderGridProperty::SaveToData(ParticleRenderParam& data)
{
	memset(&data,0,sizeof(ParticleRenderParam));

	CString sBlendMode;
	sBlendMode=m_BlendModeProperty->GetValue();
	if(sBlendMode=="Additive") data.m_blendMode=SPARK::BLENDING_ADD;
	else if(sBlendMode=="Alpha") data.m_blendMode=SPARK::BLENDING_ALPHA;
	else ASSERT(FALSE);

	if(m_sRenderType=="Quad")
	{
		data.m_type=Render_Quad;
		ASSERT(m_PropertyVector.size()==6);
		ParticleQuadRenderData& quadData=data.m_quadData;
		CString sTexturePath=m_PropertyVector[0]->GetValue();
		strcpy_s(quadData.m_texPath,LPCTSTR(sTexturePath));
		quadData.m_xScale=m_PropertyVector[1]->GetValue().fltVal;
		quadData.m_yScale=m_PropertyVector[2]->GetValue().fltVal;
		quadData.m_xDimension=m_PropertyVector[3]->GetValue().lVal;
		quadData.m_yDimension=m_PropertyVector[4]->GetValue().lVal;
		quadData.m_texMode=SPARK::TEXTURE_2D;

		// Orientation
		if(m_sOrientType=="Default Aligned") quadData.m_nOrientation=SPARK::DEFAULT_ALIGNED;
		else if(m_sOrientType=="Camera Plane Aligned") quadData.m_nOrientation=SPARK::CAMERA_PLANE_ALIGNED;
		else if(m_sOrientType=="Camera Point Aligned") quadData.m_nOrientation=SPARK::CAMERA_POINT_ALIGNED;
		else if(m_sOrientType=="Direction Aligned") quadData.m_nOrientation=SPARK::DIRECTION_ALIGNED;
		else if(m_sOrientType=="Around Axis")
		{
			quadData.m_nOrientation=SPARK::AROUND_AXIS;
			ASSERT(m_OrientationVector.size()==3);
			quadData.m_lookVector[0]=m_OrientationVector[0]->GetValue().fltVal;
			quadData.m_lookVector[1]=m_OrientationVector[1]->GetValue().fltVal;
			quadData.m_lookVector[2]=m_OrientationVector[2]->GetValue().fltVal;
		}
		else if(m_sOrientType=="Towards Point")
		{
			quadData.m_nOrientation=SPARK::TOWARDS_POINT;
			ASSERT(m_OrientationVector.size()==3);
			quadData.m_lookVector[0]=m_OrientationVector[0]->GetValue().fltVal;
			quadData.m_lookVector[1]=m_OrientationVector[1]->GetValue().fltVal;
			quadData.m_lookVector[2]=m_OrientationVector[2]->GetValue().fltVal;
		}
		else if(m_sOrientType=="Fixed Orientation")
		{
			quadData.m_nOrientation=SPARK::FIXED_ORIENTATION;
			ASSERT(m_OrientationVector.size()==6);
			quadData.m_lookVector[0]=m_OrientationVector[0]->GetValue().fltVal;
			quadData.m_lookVector[1]=m_OrientationVector[1]->GetValue().fltVal;
			quadData.m_lookVector[2]=m_OrientationVector[2]->GetValue().fltVal;
			quadData.m_upVector[0]=m_OrientationVector[3]->GetValue().fltVal;
			quadData.m_upVector[1]=m_OrientationVector[4]->GetValue().fltVal;
			quadData.m_upVector[2]=m_OrientationVector[5]->GetValue().fltVal;
		}
		else ASSERT(FALSE);
	}
	else if(m_sRenderType=="Line")
	{
		data.m_type=Render_Line;
		ASSERT(m_PropertyVector.size()==2);
		ParticleLineRenderData& lineData=data.m_lineData;
		lineData.m_length=m_PropertyVector[0]->GetValue().fltVal;
		lineData.m_width=m_PropertyVector[1]->GetValue().fltVal;
	}
	else if(m_sRenderType=="Point")
	{
		data.m_type=Render_Point;
		ASSERT(m_PropertyVector.size()==2);
		ParticlePointRenderData& pointData=data.m_pointData;
		CString sTexturePath=m_PropertyVector[0]->GetValue();
		strcpy_s(pointData.m_texPath,LPCTSTR(sTexturePath));
		pointData.m_size=m_PropertyVector[1]->GetValue().fltVal;
		pointData.m_type=SPARK::POINT_SPRITE;
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CParticleRenderGridProperty::SetOrientationType(CString sType,BOOL bNotify)
{
	if(m_sOrientType==sType&&bNotify) return ;

	unsigned int i;
	for(i=0;i<m_OrientationVector.size();++i) RemoveSubItem(m_OrientationVector[i]);
	m_OrientationVector.clear();

	if(sType=="Around Axis")
	{
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector X",COleVariant(0.0f)));
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Y",COleVariant(0.0f)));
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Z",COleVariant(1.0f)));
	}
	else if(sType=="Towards Point")
	{
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector X",COleVariant(0.0f)));
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Y",COleVariant(0.0f)));
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Z",COleVariant(1.0f)));
	}
	else if(sType=="Fixed Orientation")
	{
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector X",COleVariant(0.0f)));
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Y",COleVariant(0.0f)));
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("look vector Z",COleVariant(1.0f)));
		
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("up vector X",COleVariant(0.0f)));
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("up vector Y",COleVariant(1.0f)));
		m_OrientationVector.push_back(new CMFCPropertyGridProperty("up vector Z",COleVariant(0.0f)));
	}

	m_sOrientType=sType;
	for(i=0;i<m_OrientationVector.size();++i) AddSubItem(m_OrientationVector[i]);
	if(bNotify) m_pWndList->GetParent()->PostMessage(UM_UPDATE_PROPERTYCTRL);
}
