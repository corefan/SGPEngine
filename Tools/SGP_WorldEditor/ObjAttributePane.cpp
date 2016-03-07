// ObjAttributePane.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "ObjAttributePane.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "Render Interface\WorldEditorRenderInterface.h"
#include "SelectObjHandler.h"
#include "ObjAttributeOperation.h"
#include "OperationManager.h"
#include "SceneObjectManager.h"
#include "AttributeGridProperty.h"
#include "AttributeGroupGridProperty.h"
#include "ObjGridCheckProperty.h"

// CObjAttributePane

CObjAttributePane* CObjAttributePane::s_pAttributePane=NULL;

IMPLEMENT_DYNAMIC(CObjAttributePane, CCommonDockablePane)

CObjAttributePane::CObjAttributePane()
{
	m_ObjectType=ObjType_None;
	m_PositionProperty[0]=NULL;
	m_PositionProperty[1]=NULL;
	m_PositionProperty[2]=NULL;
	m_RotationProperty[0]=NULL;
	m_RotationProperty[1]=NULL;
	m_RotationProperty[2]=NULL;
	m_ScaleProperty=NULL;
}

CObjAttributePane::~CObjAttributePane()
{
}

BEGIN_MESSAGE_MAP(CObjAttributePane, CCommonDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED,OnAttributeChanged)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CObjAttributePane 消息处理程序

int CObjAttributePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCommonDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	m_DetailList.Create(WS_VISIBLE|WS_CHILD,rectDummy,this,(UINT)-1);
	m_DetailList.SetCustomColors(RGB(255,255,255),RGB(0,0,0),RGB(141,178,230),RGB(0,0,0),RGB(225,238,255),RGB(0,0,0),RGB(141,178,230));
	m_DetailList.SetVSDotNetLook();
	m_DetailList.MarkModifiedProperties();
	m_DetailList.EnableDescriptionArea();
	m_DetailList.SetDescriptionRows(3);
	m_DetailList.EnableHeaderCtrl(FALSE);

	m_Font.CreatePointFont(93,_T("微软雅黑"));
	m_DetailList.SetFont(&m_Font);

	s_pAttributePane=this;

	return 0;
}

void CObjAttributePane::OnSize(UINT nType, int cx, int cy)
{
	CCommonDockablePane::OnSize(nType, cx, cy);

	m_DetailList.MoveWindow(0,0,cx,cy);
}

void CObjAttributePane::LoadAttribute(const CommonObject& obj)
{
	m_PositionProperty[0]=NULL;
	m_PositionProperty[1]=NULL;
	m_PositionProperty[2]=NULL;
	m_RotationProperty[0]=NULL;
	m_RotationProperty[1]=NULL;
	m_RotationProperty[2]=NULL;
	m_ScaleProperty=NULL;
	
	m_DetailList.RemoveAll();
	if(obj.IsMF1()) LoadAttribute(obj.m_pObj);
	else if(obj.IsLight()) LoadAttribute(obj.m_pLightObj);
	else
	{	
		m_DetailList.Invalidate();
		m_ObjectType=ObjType_None;	
	}
}

void CObjAttributePane::LoadAttribute(const ISGPObject* pObj)
{
	m_ObjectType=ObjType_MF1;

	// Scene Name
	m_DetailList.AddProperty(new CMFCPropertyGridProperty("Object Name",COleVariant(pObj->m_SceneObjectName)));
	// Scene ID
	CMFCPropertyGridProperty* pSceneID=new CMFCPropertyGridProperty("Object ID",COleVariant((long)(pObj->m_iSceneID)));
	pSceneID->AllowEdit(FALSE);
	m_DetailList.AddProperty(pSceneID);
	// MF1 FileName
	CMFCPropertyGridProperty* pMF1Property=new CMFCPropertyGridProperty("MF1 FileName",COleVariant(pObj->m_MF1FileName));
	pMF1Property->AllowEdit(FALSE);
	m_DetailList.AddProperty(pMF1Property);
	// Config Setting
	CMFCPropertyGridProperty* pConfigSetting=new CMFCPropertyGridProperty("Config Setting","");
	pConfigSetting->AllowEdit(FALSE);
	m_DetailList.AddProperty(pConfigSetting);

	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	CStaticMeshInstance* pMeshInstance=pRenderInterface->GetWorldSystemManager()->getMeshInstanceBySceneID(pObj->getSceneObjectID());
	CMF1FileResource* pMF1Res=pRenderInterface->GetRenderDevice()->GetModelManager()->getModelByID(pMeshInstance->getMF1ModelResourceID());
	CSGPModelMF1* pMF1Model=(pMF1Res!=NULL)?pMF1Res->pModelMF1:NULL;
	ASSERT(pMF1Model!=NULL);
	pConfigSetting->AddOption("Default");
	if(pObj->m_iConfigIndex==0) pConfigSetting->SetValue("Default");
	for(uint32 i=0;i<pMF1Model->m_Header.m_iNumConfigs;++i)
	{
		CString str;
		str.Format("%u",i+1);
		pConfigSetting->AddOption(str);
		if(i+1==pObj->m_iConfigIndex) pConfigSetting->SetValue(str);
	}
	
	// Position
	m_PositionGroup=new CAttributeGroupGridProperty("Position",0,TRUE);
	m_PositionGroup->AllowEdit(FALSE);
	m_PositionProperty[0]=new CAttributeGridProperty("X",COleVariant(pObj->m_fPosition[0]),NULL,0);
	m_PositionGroup->AddSubItem(m_PositionProperty[0]);
	m_PositionProperty[1]=new CAttributeGridProperty("Y",COleVariant(pObj->m_fPosition[1]),NULL,0);
	m_PositionGroup->AddSubItem(m_PositionProperty[1]);
	m_PositionProperty[2]=new CAttributeGridProperty("Z",COleVariant(pObj->m_fPosition[2]),NULL,0);
	m_PositionGroup->AddSubItem(m_PositionProperty[2]);
	m_PositionGroup->Expand(TRUE);
	m_DetailList.AddProperty(m_PositionGroup);

	// Rotation
	m_RotationGroup=new CAttributeGroupGridProperty("Rotation",0,TRUE);
	m_RotationGroup->AllowEdit(FALSE);
	m_RotationProperty[0]=new CAttributeGridProperty("X",COleVariant((long)(pObj->m_fRotationXYZ[0]*180/float_Pi)),NULL,1);
	m_RotationProperty[0]->EnableSpinControl(TRUE,0,360);
	m_RotationGroup->AddSubItem(m_RotationProperty[0]);
	m_RotationProperty[1]=new CAttributeGridProperty("Y",COleVariant((long)(pObj->m_fRotationXYZ[1]*180/float_Pi)),NULL,1);
	m_RotationProperty[1]->EnableSpinControl(TRUE,0,360);
	m_RotationGroup->AddSubItem(m_RotationProperty[1]);
	m_RotationProperty[2]=new CAttributeGridProperty("Z",COleVariant((long)(pObj->m_fRotationXYZ[2]*180/float_Pi)),NULL,1);
	m_RotationProperty[2]->EnableSpinControl(TRUE,0,360);
	m_RotationGroup->AddSubItem(m_RotationProperty[2]);
	m_RotationGroup->Expand(TRUE);
	m_DetailList.AddProperty(m_RotationGroup);

	// Scale
	m_ScaleProperty=new CAttributeGridProperty("Scale",COleVariant(pObj->m_fScale));
	m_DetailList.AddProperty(m_ScaleProperty);

	// Alpha
	CMFCPropertyGridProperty* pAlpha=new CMFCPropertyGridProperty("Alpha",COleVariant((long)(pObj->m_fAlpha*255.0f)));
	pAlpha->EnableSpinControl(TRUE,0,255);
	m_DetailList.AddProperty(pAlpha);

	// Accept Light
	CObjGridCheckProperty* pReceiveLight=new CObjGridCheckProperty("Receive Light",pObj->m_bReceiveLight);
	m_DetailList.AddProperty(pReceiveLight);

	// Cast Shadow
	CObjGridCheckProperty* pCastShadow=new CObjGridCheckProperty("Cast Shadow",pObj->m_bCastShadow);
	m_DetailList.AddProperty(pCastShadow);
}

void CObjAttributePane::LoadAttribute(const ISGPLightObject* pLightObj)
{
	m_ObjectType=ObjType_Light;

	// Light Name
	CMFCPropertyGridProperty* pLightName=new CMFCPropertyGridProperty("Light Name",COleVariant(pLightObj->m_SceneObjectName));
	m_DetailList.AddProperty(pLightName);

	// Light ID
	CMFCPropertyGridProperty* pLightID=new CMFCPropertyGridProperty("Light ID",COleVariant((long)(pLightObj->m_iLightID)));
	pLightID->AllowEdit(FALSE);
	m_DetailList.AddProperty(pLightID);

	// Light Type
	CString slightType[3]={"Point Light","Spot Light","Direction Light"};
	CMFCPropertyGridProperty* pLightType=new CMFCPropertyGridProperty("Light Type",COleVariant(slightType[pLightObj->m_iLightType]));
	pLightType->AllowEdit(FALSE);
	pLightType->AddOption(slightType[0]);
	pLightType->AddOption(slightType[1]);
	pLightType->AddOption(slightType[2]);
	m_DetailList.AddProperty(pLightType);

	// Position
	m_PositionGroup=new CAttributeGroupGridProperty("Position",0,TRUE);
	m_PositionGroup->AllowEdit(FALSE);
	m_DetailList.AddProperty(m_PositionGroup);
	m_PositionProperty[0]=new CAttributeGridProperty("X",COleVariant(pLightObj->m_fPosition[0]),NULL,0);
	m_PositionGroup->AddSubItem(m_PositionProperty[0]);
	m_PositionProperty[1]=new CAttributeGridProperty("Y",COleVariant(pLightObj->m_fPosition[1]),NULL,0);
	m_PositionGroup->AddSubItem(m_PositionProperty[1]);
	m_PositionProperty[2]=new CAttributeGridProperty("Z",COleVariant(pLightObj->m_fPosition[2]),NULL,0);
	m_PositionGroup->AddSubItem(m_PositionProperty[2]);
	m_PositionGroup->Expand(TRUE);

	// Direction
	CMFCPropertyGridProperty* pDirection=new CMFCPropertyGridProperty("Direction",0,TRUE);
	pDirection->AllowEdit(FALSE);
	m_DetailList.AddProperty(pDirection);
	pDirection->AddSubItem(new CMFCPropertyGridProperty("X",COleVariant(pLightObj->m_fDirection[0]),NULL,1));
	pDirection->AddSubItem(new CMFCPropertyGridProperty("Y",COleVariant(pLightObj->m_fDirection[1]),NULL,1));
	pDirection->AddSubItem(new CMFCPropertyGridProperty("Z",COleVariant(pLightObj->m_fDirection[2]),NULL,1));
	pDirection->Expand(TRUE);

	// Size
	CMFCPropertyGridProperty* pLightSize=new CMFCPropertyGridProperty("Light Size",COleVariant(pLightObj->m_fLightSize));
	m_DetailList.AddProperty(pLightSize);

	// Diffuse Color
	COLORREF diffuseCol=RGB((int)(pLightObj->m_fDiffuseColor[0]*255),(int)(pLightObj->m_fDiffuseColor[1]*255),(int)(pLightObj->m_fDiffuseColor[2]*255));
	CMFCPropertyGridColorProperty* pDiffuseColor=new CMFCPropertyGridColorProperty("Diffuse Color",diffuseCol);
	pDiffuseColor->EnableOtherButton(_T("Other..."));
	m_DetailList.AddProperty(pDiffuseColor);

	// Specular Color
	COLORREF specularCol=RGB((int)(pLightObj->m_fSpecularColor[0]*255),(int)(pLightObj->m_fSpecularColor[1]*255),(int)(pLightObj->m_fSpecularColor[2]*255));
	CMFCPropertyGridColorProperty* pSpecularColor=new CMFCPropertyGridColorProperty("Specular Color",specularCol);
	pSpecularColor->EnableOtherButton(_T("Other..."));
	m_DetailList.AddProperty(pSpecularColor);

	// Ambient Color
	COLORREF ambientCol=RGB((int)(pLightObj->m_fAmbientColor[0]*255),(int)(pLightObj->m_fAmbientColor[1]*255),(int)(pLightObj->m_fAmbientColor[2]*255));
	CMFCPropertyGridColorProperty* pAmbientColor=new CMFCPropertyGridColorProperty("Ambient Color",ambientCol);
	pAmbientColor->EnableOtherButton(_T("Other..."));
	m_DetailList.AddProperty(pAmbientColor);

	// Light Range
	CMFCPropertyGridProperty* pLightRange=new CMFCPropertyGridProperty("Light Range",COleVariant(pLightObj->m_fRange));
	m_DetailList.AddProperty(pLightRange);

	// Fall Off
	CMFCPropertyGridProperty* pFalloff=new CMFCPropertyGridProperty("Fall off",COleVariant(pLightObj->m_fFalloff));
	m_DetailList.AddProperty(pFalloff);

	// Attenuation
	m_DetailList.AddProperty(new CMFCPropertyGridProperty("Attenuation0",COleVariant(pLightObj->m_fAttenuation0)));
	m_DetailList.AddProperty(new CMFCPropertyGridProperty("Attenuation1",COleVariant(pLightObj->m_fAttenuation1)));
	m_DetailList.AddProperty(new CMFCPropertyGridProperty("Attenuation2",COleVariant(pLightObj->m_fAttenuation2)));

	// Inner Cone Angle
	CMFCPropertyGridProperty* pInnerConeAngle=new CMFCPropertyGridProperty("Inner Cone Angle",COleVariant((long)(pLightObj->m_fTheta*180/float_Pi)));
	pInnerConeAngle->EnableSpinControl(TRUE,0,360);
	m_DetailList.AddProperty(pInnerConeAngle);
	// Outer Cone Angle
	CMFCPropertyGridProperty* pOuterConeAngle=new CMFCPropertyGridProperty("Outer Cone Angle",COleVariant((long)(pLightObj->m_fPhi*180/float_Pi)));
	pOuterConeAngle->EnableSpinControl(TRUE,0,360);
	m_DetailList.AddProperty(pOuterConeAngle);
}

LRESULT CObjAttributePane::OnAttributeChanged(WPARAM wParam,LPARAM lParam)
{
	CMFCPropertyGridProperty* pProperty=(CMFCPropertyGridProperty*)lParam;

	switch(m_ObjectType)
	{
	case ObjType_MF1:
		DealObjAttrChanged(pProperty);
		break;
	case ObjType_Light:
		DealLightAttrChanged(pProperty);
		break;
	}

	return 0;
}

void CObjAttributePane::DealObjAttrChanged(CMFCPropertyGridProperty* pProperty)
{
	CString strAttrName=pProperty->GetName();
	CommonObject& obj=SelectObjHandler::GetInstance()->GetLastObj();
	std::vector<CommonObject>& objVector=SelectObjHandler::GetInstance()->GetSelectedObj();
	ASSERT(obj.IsMF1());

	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	
	if(strAttrName=="Object Name")
	{
		ObjAttributeOperation* pAttrOperation=new ObjAttributeOperation;
		pAttrOperation->AddObject(obj);
		pAttrOperation->SaveAllPrevState();
		strcpy_s(obj.m_pObj->m_SceneObjectName,sizeof(obj.m_pObj->m_SceneObjectName),(CString)pProperty->GetValue());
		SceneObjectManager::GetInstance()->SceneObjNameChanged(obj);
		pAttrOperation->SaveAllCurrState();
		OperationManager::GetInstance()->AddNewOperation(pAttrOperation);
	}
	else if(strAttrName=="Config Setting")
	{
		uint32 index;
		CString strValue=pProperty->GetValue();
		if(strValue=="Default") index=0;
		else index=atoi(strValue);

		ObjAttributeOperation* pAttrOperation=new ObjAttributeOperation;
		CString mf1FileName = obj.m_pObj->m_MF1FileName;

		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsMF1()&&mf1FileName==CString(objVector[i].m_pObj->m_MF1FileName))
			{
				if(objVector[i].m_pObj->m_iConfigIndex!=index)
				{
					uint32 pos=pAttrOperation->AddObject(objVector[i]);
					pAttrOperation->SavePrevState(pos);
					SceneObjectManager::GetInstance()->SetObjectConfigSetting(objVector[i],index);	
				}
			}
		}

		pAttrOperation->SaveAllCurrState();
		OperationManager::GetInstance()->AddNewOperation(pAttrOperation);
	}
	else if(strAttrName=="X"||strAttrName=="Y"||strAttrName=="Z")
	{
		int index;
		if(strAttrName=="X") index=0;
		else if(strAttrName=="Y") index=1;
		else index=2;

		ObjAttributeOperation* pAttrOperation =new ObjAttributeOperation;
		uint32 pos=pAttrOperation->AddObject(obj);
		pAttrOperation->SavePrevState(pos);
		
		if(pProperty->GetData()==0)// Position
		{
			obj.m_pObj->m_fPosition[index]=pProperty->GetValue().fltVal;
			SceneObjectManager::GetInstance()->MoveSceneObject(obj,obj.m_pObj->m_fPosition[0],obj.m_pObj->m_fPosition[1],obj.m_pObj->m_fPosition[2],TRUE);		
		}
		else if(pProperty->GetData()==1)// Rotation
		{
			int iValue = pProperty->GetValue().lVal;
			iValue=jlimit(0,360,iValue);
			float fValue=(float)iValue*float_Pi/180.0f;
			pProperty->SetValue(COleVariant((long)iValue));

			obj.m_pObj->m_fRotationXYZ[index]=fValue;
			SceneObjectManager::GetInstance()->RotateSceneObject(obj,obj.m_pObj->m_fRotationXYZ[0],obj.m_pObj->m_fRotationXYZ[1],obj.m_pObj->m_fRotationXYZ[2],TRUE);
		}
		else
		{
			ASSERT(FALSE);
		}
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();

		pAttrOperation->SaveCurrState(pos);
		OperationManager::GetInstance()->AddNewOperation(pAttrOperation);
	}
	else if(strAttrName=="Scale")
	{
		float fScale=pProperty->GetValue().fltVal;
		if(fScale<0.00002f)
		{
			fScale=1.0f;
			pProperty->SetValue(COleVariant(fScale));
		}

		ObjAttributeOperation* pAttrOperation =new ObjAttributeOperation;
		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsMF1())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);
				objVector[i].m_pObj->m_fScale=fScale;
				SceneObjectManager::GetInstance()->ScaleSceneObject(objVector[i],objVector[i].m_pObj->m_fScale,TRUE);
			}
		}

		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
		pAttrOperation->SaveAllCurrState();
		OperationManager::GetInstance()->AddNewOperation(pAttrOperation);
	}
	else if(strAttrName=="Alpha")
	{
		int iAlpha=pProperty->GetValue().lVal;
		iAlpha=jlimit(0,255,iAlpha);
		pProperty->SetValue(COleVariant((long)iAlpha));
		float fAlpha=iAlpha/255.0f;

		ObjAttributeOperation* pAttrOperation=new ObjAttributeOperation;
		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsMF1())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pObj->m_fAlpha=fAlpha;
				SceneObjectManager::GetInstance()->SceneObjectSetAlpha(objVector[i],fAlpha);
			}
		}
		pAttrOperation->SaveAllCurrState();
		OperationManager::GetInstance()->AddNewOperation(pAttrOperation);
	}
	else if(strAttrName=="Receive Light")
	{
		bool bReceiveLight=((CObjGridCheckProperty*)pProperty)->GetCheck();

		ObjAttributeOperation* pAttrOperation=new ObjAttributeOperation;
		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsMF1())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);
				objVector[i].m_pObj->m_bReceiveLight=bReceiveLight;
			}
		}
		pAttrOperation->SaveAllCurrState();
		OperationManager::GetInstance()->AddNewOperation(pAttrOperation);
	}
	else if(strAttrName=="Cast Shadow")
	{
		bool bCastShadow=((CObjGridCheckProperty*)pProperty)->GetCheck();

		ObjAttributeOperation* pAttrOperation=new ObjAttributeOperation;
		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsMF1())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);
				objVector[i].m_pObj->m_bCastShadow=bCastShadow;
			}
		}
		pAttrOperation->SaveAllCurrState();
		OperationManager::GetInstance()->AddNewOperation(pAttrOperation);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CObjAttributePane::DealLightAttrChanged(CMFCPropertyGridProperty* pProperty)
{
	CString strAttrName=pProperty->GetName();
	CommonObject& obj=SelectObjHandler::GetInstance()->GetLastObj();
	std::vector<CommonObject>& objVector=SelectObjHandler::GetInstance()->GetSelectedObj();
	ASSERT(obj.IsLight());

	ObjAttributeOperation* pAttrOperation=new ObjAttributeOperation;

	if(strAttrName=="Light Name")// Light Name
	{
		uint32 pos=pAttrOperation->AddObject(obj);
		pAttrOperation->SavePrevState(pos);
		strcpy_s(obj.m_pLightObj->m_SceneObjectName,sizeof(obj.m_pLightObj->m_SceneObjectName),(CString)pProperty->GetValue());
		SceneObjectManager::GetInstance()->SceneObjNameChanged(obj);
	}
	else if(strAttrName=="X"||strAttrName=="Y"||strAttrName=="Z")// Position or Direction
	{
		uint32 pos=pAttrOperation->AddObject(obj);
		pAttrOperation->SavePrevState(pos);

		int index;
		if(strAttrName=="X") index=0;
		else if(strAttrName=="Y") index=1;
		else index=2;
			
		if(pProperty->GetData()==0)// Position
			obj.m_pLightObj->m_fPosition[index]=pProperty->GetValue().fltVal;
		else if(pProperty->GetData()==1)// Direction
			obj.m_pLightObj->m_fDirection[index]=pProperty->GetValue().fltVal;
		else
			ASSERT(FALSE);
	}
	else if(strAttrName=="Light Type")// Light Type
	{
		CString strType=pProperty->GetValue();
		uint32 lightType;
		if(strType=="Point Light") lightType=SGPLT_Point;
		else if(strType=="Spot Light") lightType=SGPLT_Spot;
		else if(strType=="Direction Light") lightType=SGPLT_Direction;
		else ASSERT(FALSE);

		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight()&&objVector[i].m_pLightObj->m_iLightType!=lightType)
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pLightObj->m_iLightType=lightType;
			}
		}
	}
	else if(strAttrName=="Light Size")// Light Size
	{
		float lightSize=pProperty->GetValue().fltVal;

		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pLightObj->m_fLightSize=lightSize;
			}
		}
	}
	else if(strAttrName=="Diffuse Color")
	{
		CMFCPropertyGridColorProperty* pColorProp=(CMFCPropertyGridColorProperty*)pProperty;
		float r=(float)GetRValue(pColorProp->GetColor())/255.0f;
		float g=(float)GetGValue(pColorProp->GetColor())/255.0f;
		float b=(float)GetBValue(pColorProp->GetColor())/255.0f;

		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pLightObj->m_fDiffuseColor[0]=r;
				objVector[i].m_pLightObj->m_fDiffuseColor[1]=g;
				objVector[i].m_pLightObj->m_fDiffuseColor[2]=b;
			}
		}
	}
	else if(strAttrName=="Specular Color")
	{
		CMFCPropertyGridColorProperty* pColorProp=(CMFCPropertyGridColorProperty*)pProperty;
		float r=(float)GetRValue(pColorProp->GetColor())/255.0f;
		float g=(float)GetGValue(pColorProp->GetColor())/255.0f;
		float b=(float)GetBValue(pColorProp->GetColor())/255.0f;

		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pLightObj->m_fSpecularColor[0]=r;
				objVector[i].m_pLightObj->m_fSpecularColor[1]=g;
				objVector[i].m_pLightObj->m_fSpecularColor[2]=b;
			}
		}
	}
	else if(strAttrName=="Ambient Color")
	{
		CMFCPropertyGridColorProperty* pColorProp=(CMFCPropertyGridColorProperty*)pProperty;
		float r=(float)GetRValue(pColorProp->GetColor())/255.0f;
		float g=(float)GetGValue(pColorProp->GetColor())/255.0f;
		float b=(float)GetBValue(pColorProp->GetColor())/255.0f;

		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pLightObj->m_fAmbientColor[0]=r;
				objVector[i].m_pLightObj->m_fAmbientColor[1]=g;
				objVector[i].m_pLightObj->m_fAmbientColor[2]=b;
			}
		}
	}
	else if(strAttrName=="Light Range")
	{
		float lightRange=pProperty->GetValue().fltVal;
		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pLightObj->m_fRange=lightRange;
			}		
		}
	}
	else if(strAttrName=="Fall off")
	{
		float fFallOff=pProperty->GetValue().fltVal;
		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				obj.m_pLightObj->m_fFalloff=fFallOff;
			}
		}
	}
	else if(strAttrName=="Attenuation0")
	{
		float fAttenuation0=pProperty->GetValue().fltVal;
		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pLightObj->m_fAttenuation0=fAttenuation0;
			}
		}
	}
	else if(strAttrName=="Attenuation1")
	{
		float fAttenuation1=pProperty->GetValue().fltVal;
		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pLightObj->m_fAttenuation1=fAttenuation1;
			}
		}
	}
	else if(strAttrName=="Attenuation2")
	{
		float fAttenuation2=pProperty->GetValue().fltVal;
		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pLightObj->m_fAttenuation2=fAttenuation2;
			}
		}
	}
	else if(strAttrName=="Inner Cone Angle")
	{
		float fTheta=(float)pProperty->GetValue().lVal*float_Pi/180.0f;
		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pLightObj->m_fTheta=fTheta;
			}
		}
	}
	else if(strAttrName=="Outer Cone Angle")
	{
		float fPhi=(float)pProperty->GetValue().lVal*float_Pi/180.0f;
		for(uint32 i=0;i<objVector.size();++i)
		{
			if(objVector[i].IsLight())
			{
				uint32 pos=pAttrOperation->AddObject(objVector[i]);
				pAttrOperation->SavePrevState(pos);

				objVector[i].m_pLightObj->m_fPhi=fPhi;
			}
		}
	}
	else
	{
		ASSERT(FALSE);
	}

	pAttrOperation->SaveAllCurrState();
	OperationManager::GetInstance()->AddNewOperation(pAttrOperation);
}

void CObjAttributePane::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	
}

void CObjAttributePane::NotifyAttributeChanged(const std::vector<CommonObject>& objVector)
{
	if(!IsVisible()) return ;
	CommonObject lastObj=SelectObjHandler::GetInstance()->GetLastObj();
	if(lastObj.IsNULL()) return ;
	BOOL bFinded=FALSE;
	for(uint32 i=0;i<objVector.size();++i)
	{
		if(lastObj==objVector[i])
		{
			bFinded=TRUE;
			break;
		}
	}
	if(bFinded) LoadAttribute(lastObj);
}

void CObjAttributePane::NotifyAttributeChanged(const CommonObject& obj)
{
	if(!IsVisible()) return ;
	CommonObject lastObj=SelectObjHandler::GetInstance()->GetLastObj();
	if(obj==lastObj) LoadAttribute(lastObj);
}

void CObjAttributePane::NotifyPositionChanged(const std::vector<CommonObject>& objVector)
{
	if(!IsVisible()) return ;
	CommonObject lastObj=SelectObjHandler::GetInstance()->GetLastObj();
	if(lastObj.IsNULL()) return ;
	BOOL bFinded=FALSE;
	for(uint32 i=0;i<objVector.size();++i)
	{
		if(lastObj==objVector[i])
		{
			bFinded=TRUE;
			break;
		}
	}
	if(bFinded)
	{
		if(m_PositionProperty[0]!=NULL&&m_PositionProperty[1]!=NULL&&m_PositionProperty[2]!=NULL)
		{
			float x,y,z;
			if(lastObj.IsMF1())
			{
				x = lastObj.m_pObj->m_fPosition[0];
				y = lastObj.m_pObj->m_fPosition[1];
				z = lastObj.m_pObj->m_fPosition[2];
			}
			else if(lastObj.IsLight())
			{
				x = lastObj.m_pLightObj->m_fPosition[0];
				y = lastObj.m_pLightObj->m_fPosition[1];
				z = lastObj.m_pLightObj->m_fPosition[2];
			}
			else ASSERT(FALSE);
			m_PositionProperty[0]->SetGridValue(COleVariant(x));
			m_PositionProperty[1]->SetGridValue(COleVariant(y));
			m_PositionProperty[2]->SetGridValue(COleVariant(z));
			m_PositionGroup->RedrawProperty();
		}
	}
}

void CObjAttributePane::NotifyRotationChanged(const std::vector<CommonObject>& objVector)
{
	if(!IsVisible()) return ;
	CommonObject lastObj=SelectObjHandler::GetInstance()->GetLastObj();
	if(lastObj.IsNULL()) return ;
	BOOL bFinded=FALSE;
	for(uint32 i=0;i<objVector.size();++i)
	{
		if(lastObj==objVector[i])
		{
			bFinded=TRUE;
			break;
		}
	}
	if(bFinded)
	{
		if(m_RotationProperty[0]!=NULL&&m_RotationProperty[1]!=NULL&&m_RotationProperty[2]!=NULL)
		{
			long x = 0, y = 0, z = 0;
			if(lastObj.IsMF1())
			{
				x = (long)(lastObj.m_pObj->m_fRotationXYZ[0]*180/float_Pi);
				y = (long)(lastObj.m_pObj->m_fRotationXYZ[1]*180/float_Pi);
				z = (long)(lastObj.m_pObj->m_fRotationXYZ[2]*180/float_Pi);
			}
			else ASSERT(FALSE);

			m_RotationProperty[0]->SetGridValue(COleVariant(x));
			m_RotationProperty[1]->SetGridValue(COleVariant(y));
			m_RotationProperty[2]->SetGridValue(COleVariant(z));
			m_RotationGroup->RedrawProperty();
		}
	}
}

void CObjAttributePane::NotifyScaleChanged(const std::vector<CommonObject>& objVector)
{
	if(!IsVisible()) return ;
	CommonObject lastObj=SelectObjHandler::GetInstance()->GetLastObj();
	if(lastObj.IsNULL()) return ;
	BOOL bFinded=FALSE;
	for(uint32 i=0;i<objVector.size();++i)
	{
		if(lastObj==objVector[i])
		{
			bFinded=TRUE;
			break;
		}
	}
	if(bFinded)
	{
		if(m_ScaleProperty!=NULL)
		{
			if(lastObj.IsMF1())
			{
				m_ScaleProperty->SetGridValue(lastObj.m_pObj->m_fScale);
				m_DetailList.UpdateWindow();
			}
			else ASSERT(FALSE);
		}
	}
}
