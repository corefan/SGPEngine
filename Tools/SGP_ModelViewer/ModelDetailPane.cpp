// ModelDetailPane.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ModelDetailPane.h"

#include "../SGPLibraryCode/SGPHeader.h"//engine header
using namespace sgp;
#include "ModelViewerRenderInterface.h"
#include "MainFrame.h"

#include "PropertyGridCheckProperty.h"
#include "PropertyGroupGridProperty.h"
#include "ButtonPropertyGridProperty.h"
#include "MenuPropertyGridProperty.h"
#include "EditPropertyGridProperty.h"
#include "HelpFunction.h"
// CModelDetailPane

IMPLEMENT_DYNAMIC(CModelDetailPane, /*CDockablePane*/CCommonDockPane)

CModelDetailPane::CModelDetailPane()
{
	m_pConfigSetting=NULL;
}

CModelDetailPane::~CModelDetailPane()
{
	PurgeAllConfig();
	ReleaseParticleTag();
}

BEGIN_MESSAGE_MAP(CModelDetailPane, /*CDockablePane*/CCommonDockPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED,OnDetailChanged)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

CModelDetailPane* CModelDetailPane::s_ModelDetailPane=NULL;

// CModelDetailPane 消息处理程序

int CModelDetailPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (/*CDockablePane*/CCommonDockPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	m_DetailList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, (UINT)-1);

//	m_DetailList.EnableHeaderCtrl(FALSE);
	m_DetailList.SetCustomColors(RGB(255,255,255),RGB(0,0,0),RGB(141,178,230),RGB(0,0,0),RGB(225,238,255),RGB(0,0,0),RGB(141,178,230));
	m_DetailList.SetVSDotNetLook();
	m_DetailList.MarkModifiedProperties();
	m_DetailList.EnableDescriptionArea();
	m_DetailList.SetDescriptionRows(3);
//	m_DetailList.SetShowDragContext(FALSE);
//	m_DetailList.SetAlphabeticMode(TRUE);

	// set font of m_DetailList control
	m_Font.CreatePointFont(93,_T("微软雅黑"));
	m_DetailList.SetFont(&m_Font);

	m_ConfigDlg.Create(IDD_MODELDETAIL_CONFIG,this);
	m_ConfigDlg.ShowWindow(SW_SHOW);

	s_ModelDetailPane=this;
	return 0;
}

void CModelDetailPane::OnSize(UINT nType, int cx, int cy)
{
	/*CDockablePane*/CCommonDockPane::OnSize(nType, cx, cy);

	SetCtrlPos();
}

void CModelDetailPane::ModelClosed()
{
	PurgeAllConfig();
	ReleaseParticleTag();
	m_MeshPropertyVector.clear();
	m_ParticlePropertyVector.clear();
	m_TexturePropertyVector.clear();
	m_DetailList.RemoveAll();// delete all items related with previous model
	m_DetailList.Invalidate();

	m_ConfigDlg.ModelClosed();
}

void CModelDetailPane::ModelChanged()
{
	PurgeAllConfig();
	ReleaseParticleTag();
	m_MeshPropertyVector.clear();
	m_ParticlePropertyVector.clear();
	m_TexturePropertyVector.clear();
	m_DetailList.RemoveAll();// delete all items related with previous model
	
	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	if(pModelMF1!=NULL)
	{
		if(m_ConfigDlg.m_hWnd!=NULL)
			m_ConfigDlg.LoadConfigSetting(pModelMF1->m_Header.m_iNumConfigs,pModelMF1->m_pConfigSetting);
	
		// insert mesh detail info
		int meshCount=pModelMF1->m_Header.m_iNumMeshes;
		SGPMF1Mesh* pMesh=pModelMF1->m_pLOD0Meshes;
		for(int i=0;i<meshCount;++i)
		{
			CString str;
			str.Format(_T("Mesh %d"),i+1);
			CPropertyGroupGridProperty* pMeshGroup=new CPropertyGroupGridProperty(str,COleVariant(pMesh[i].m_cName));
			pMeshGroup->SetDescription(CString("Mesh Name: ")+CString(pMesh[i].m_cName));
			m_DetailList.AddProperty(pMeshGroup);

			//whether show option
			CPropertyGridCheckProperty* pShowProperty=new CPropertyGridCheckProperty(_T("Show"),COleVariant(_T("")),Check_ShowMesh,i);
			pShowProperty->SetDescription("Click checkbox to choose whether to show mesh");
			pShowProperty->SetCheck(TRUE);
			m_MeshPropertyVector.push_back(pShowProperty);
			pMeshGroup->AddSubItem(pShowProperty);

			// texture info
			if(pMesh[i].m_SkinIndex!=0xFFFFFFFF)
			{
				SGPMF1Skin &Skin=pModelMF1->m_pSkins[pMesh[i].m_SkinIndex];
				CMenuPropertyGridProperty* pTextureProperty=new CMenuPropertyGridProperty(_T("Texture"),COleVariant(Skin.m_cName));
				pTextureProperty->SetIndex(i);
				pTextureProperty->SetDescription("Right click mouse to use texture selected from Resource Explorer");
				pTextureProperty->SetMenuType(TextureMenu);
				m_TexturePropertyVector.push_back(pTextureProperty);
				pMeshGroup->AddSubItem(pTextureProperty);
			}
			else
			{
				m_TexturePropertyVector.push_back(NULL);
			}

			// bounding box
			AABBox& aabBox=pMesh[i].m_bbox;
			CPropertyGridCheckProperty* pBoxInfo=new CPropertyGridCheckProperty(CString(_T("BoundingBox")),COleVariant(_T("")),Check_ShowBoundBox,i);
			pBoxInfo->SetDescription("Click checkbox to choose whether to show bounding box of the mesh");
			pBoxInfo->SetCheck(FALSE);
			pMeshGroup->AddSubItem(pBoxInfo);
			// min box
			CMFCPropertyGridProperty *pMinBoxInfo=new CMFCPropertyGridProperty(CString("vcMin"),0,TRUE);
			pMinBoxInfo->SetDescription("The point(which x,y,z values are minimum) within bounding box");
			pMinBoxInfo->AllowEdit(FALSE);
			pMeshGroup->AddSubItem(pMinBoxInfo);
			pMinBoxInfo->AddSubItem(new CEditPropertyGridProperty(CString("x"),COleVariant(aabBox.vcMin.x),i,Property_MinBox_X));
			pMinBoxInfo->AddSubItem(new CEditPropertyGridProperty(CString("y"),COleVariant(aabBox.vcMin.y),i,Property_MinBox_Y));
			pMinBoxInfo->AddSubItem(new CEditPropertyGridProperty(CString("z"),COleVariant(aabBox.vcMin.z),i,Property_MinBox_Z));
			// max box	
			CMFCPropertyGridProperty *pMaxBoxInfo=new CMFCPropertyGridProperty(CString("vcMax"),0,TRUE);
			pMaxBoxInfo->SetDescription("The point(which x,y,z values are maximum) within bounding box");
			pMaxBoxInfo->AllowEdit(FALSE);
			pMeshGroup->AddSubItem(pMaxBoxInfo);
			pMaxBoxInfo->AddSubItem(new CEditPropertyGridProperty(CString("x"),COleVariant(aabBox.vcMax.x),i,Property_MaxBox_X));
			pMaxBoxInfo->AddSubItem(new CEditPropertyGridProperty(CString("y"),COleVariant(aabBox.vcMax.y),i,Property_MaxBox_Y));
			pMaxBoxInfo->AddSubItem(new CEditPropertyGridProperty(CString("z"),COleVariant(aabBox.vcMax.z),i,Property_MaxBox_Z));
		}

		// insert Particle info
		int particleCount=pModelMF1->m_Header.m_iNumParticles;
		SGPMF1ParticleTag* pParticleTag=pModelMF1->m_pParticleEmitter;
		for(int i=0;i<particleCount;++i)
		{
			CString strName;
			strName.Format(_T("Particle %d"),i+1);
			CPropertyGroupGridProperty* pParticleGroup=new CPropertyGroupGridProperty(strName,COleVariant(pParticleTag[i].m_szname));
			pParticleGroup->SetDescription("Note that: particle name always begin with string \"part.\"");
			m_DetailList.AddProperty(pParticleGroup);

			CPropertyGridCheckProperty* pShowProperty=new CPropertyGridCheckProperty(_T("Show"),COleVariant(_T("")),Check_ShowParticle,i);
			pShowProperty->SetDescription("Click checkbox to choose whether to show this particle");
			m_ParticlePropertyVector.push_back(pShowProperty);
			pParticleGroup->AddSubItem(pShowProperty);

			CMenuPropertyGridProperty* pConfigProperty=new CMenuPropertyGridProperty(_T("Configure"),COleVariant(_T("")));
			pParticleGroup->AddSubItem(pConfigProperty);
			pConfigProperty->SetMenuType(ParticleMenu);
			pConfigProperty->SetRange(i,particleCount);
			ParticleSystemParam& systemParam=pParticleTag[i].m_SystemParam;
			if(systemParam.m_groupCount!=0&&systemParam.m_pGroupParam!=NULL)
			{
				CString strInfo;
				strInfo.Format("Particle %d Setting",i+1);
				pConfigProperty->SetValue(strInfo);
			}
			pConfigProperty->SetDescription("Double click or right click mouse to configure parameters of this particle");
		}

		// insert BoneFile info
		int boneFileCount=pModelMF1->m_Header.m_iNumBoneAnimFile;
		if(boneFileCount!=0)
		{
			CMFCPropertyGridProperty* pGroupBoneFile = new CMFCPropertyGridProperty(_T("BoneFiles"));
			pGroupBoneFile->SetDescription("Bone files(.bf1) related with current model");
			SGPMF1BoneFileName* pBoneName=pModelMF1->m_pBoneFileNames;
			for(int i=0;i<boneFileCount;++i)
			{
				CString str;
				str.Format(_T("BoneFile %d"),i+1);
				CMFCPropertyGridProperty* pBoneFile=new CMFCPropertyGridProperty(str,COleVariant(pBoneName[i].m_cBoneFileName));
				pBoneFile->AllowEdit(FALSE);
				pGroupBoneFile->AddSubItem(pBoneFile);
			}
			pGroupBoneFile->Expand(FALSE);
			m_DetailList.AddProperty(pGroupBoneFile);
		}

		// insert bone info
		int boneCount=pModelMF1->m_iNumBones;
		if(boneCount!=0)
		{
			CMFCPropertyGridProperty* pGroupBone = new CMFCPropertyGridProperty(_T("Bones"));
			pGroupBone->SetDescription("Bones of current model");
			SGPMF1Bone* pBones=pModelMF1->m_pBones;
			for(int i=0;i<boneCount;++i)
			{
				CString str;
				str.Format(_T("Bone %d"),i+1);
				CMFCPropertyGridProperty* pBoneInfo=new CMFCPropertyGridProperty(str,COleVariant(pBones[i].m_cName));
				pBoneInfo->AllowEdit(FALSE);
				pGroupBone->AddSubItem(pBoneInfo);
			}
			pGroupBone->Expand(FALSE);
			m_DetailList.AddProperty(pGroupBone);
		}

		// insert boundingbox info
		AABBox& aabbox=pModelMF1->m_MeshAABBox;
		CMFCPropertyGridProperty* pGroupBoundingBox = new CMFCPropertyGridProperty(_T("Bounding Box"));
		pGroupBoundingBox->SetDescription("The bounding box of current model");
		// min box
		CMFCPropertyGridProperty* pMinBox=new CMFCPropertyGridProperty(CString("vcMin"),0,TRUE);
		pMinBox->SetDescription("The point(which x,y,z values are minimum) within bounding box");
		pMinBox->AllowEdit(FALSE);
		pGroupBoundingBox->AddSubItem(pMinBox);
		pMinBox->AddSubItem(new CEditPropertyGridProperty(CString("x"),COleVariant(aabbox.vcMin.x),-1,Property_MinBox_X));
		pMinBox->AddSubItem(new CEditPropertyGridProperty(CString("y"),COleVariant(aabbox.vcMin.y),-1,Property_MinBox_Y));
		pMinBox->AddSubItem(new CEditPropertyGridProperty(CString("z"),COleVariant(aabbox.vcMin.z),-1,Property_MinBox_Z));

		// max box
		CMFCPropertyGridProperty* pMaxBox=new CMFCPropertyGridProperty(CString("vcMax"),0,TRUE);
		pMaxBox->SetDescription("The point(which x,y,z values are maximum) within bounding box");
		pMaxBox->AllowEdit(FALSE);
		pGroupBoundingBox->AddSubItem(pMaxBox);
		pMaxBox->AddSubItem(new CEditPropertyGridProperty(CString("x"),COleVariant(aabbox.vcMax.x),-1,Property_MaxBox_X));
		pMaxBox->AddSubItem(new CEditPropertyGridProperty(CString("y"),COleVariant(aabbox.vcMax.y),-1,Property_MaxBox_Y));
		pMaxBox->AddSubItem(new CEditPropertyGridProperty(CString("z"),COleVariant(aabbox.vcMax.z),-1,Property_MaxBox_Z));
		pGroupBoundingBox->Expand(FALSE);
		m_DetailList.AddProperty(pGroupBoundingBox);
	}
}

void CModelDetailPane::NotifyModelChanged()
{
	if(s_ModelDetailPane!=NULL)
		s_ModelDetailPane->ModelChanged();
}

LRESULT CModelDetailPane::OnDetailChanged(WPARAM wParam,LPARAM lParam)
{
	/***********************************************************************
	all changed details of the model should be processed by this function!!!
	***********************************************************************/
	CMFCPropertyGridProperty* pProp=(CMFCPropertyGridProperty*)lParam;
	CString strAttrName=pProp->GetName();
	
	// Bounding box changed
	if(strAttrName=="x"||strAttrName=="y"||strAttrName=="z")
	{
		ChangeBoundBox(pProp);
		return 0;
	}
	
	// Texture Changed
	if(strAttrName=="Texture")
	{
		CMenuPropertyGridProperty* pTextureProperty=(CMenuPropertyGridProperty*)lParam;
		ChangeTexture(pTextureProperty->GetCurrIndex(),pTextureProperty->GetValue());
		return 0;
	}

	return 0;
}

void CModelDetailPane::ShowMesh(int index,BOOL bShow)
{
	BOOL bFinded=FALSE;
	std::vector<void*>::iterator iter;
	for(iter=m_MeshConfigVector.begin();iter!=m_MeshConfigVector.end();++iter)
	{
		SGPMF1ConfigSetting::MeshConfig* pMeshConfig=(SGPMF1ConfigSetting::MeshConfig*)*iter;
		if(pMeshConfig->MeshID==index)
		{
			delete pMeshConfig;
			bFinded=TRUE;
			if(bShow) m_MeshConfigVector.erase(iter);
			break;
		}
	}

	if((!bFinded)&&(!bShow))
	{
		for(iter=m_MeshConfigVector.begin();iter!=m_MeshConfigVector.end();++iter)
		{
			SGPMF1ConfigSetting::MeshConfig* pMeshConfig=(SGPMF1ConfigSetting::MeshConfig*)*iter;
			if(pMeshConfig->MeshID>(uint32)index) break;
		}
		SGPMF1ConfigSetting::MeshConfig* pMeshConfig=new SGPMF1ConfigSetting::MeshConfig;
		pMeshConfig->MeshID=index;
		pMeshConfig->bVisible=false;
		m_MeshConfigVector.insert(iter,(void*)pMeshConfig);
	}

	SGPMF1ConfigSetting* pOldConfigSetting=m_pConfigSetting;
	BuildConfigSetting();
	ModelViewerRenderInterface::GetInstance()->SetConfigSetting(m_pConfigSetting);
	if(pOldConfigSetting!=NULL) delete pOldConfigSetting;
}

void CModelDetailPane::ShowParticle(int index,BOOL bShow)
{
	BOOL bFinded=FALSE;
	std::vector<void*>::iterator iter;
	for(iter=m_ParticleConfigVector.begin();iter!=m_ParticleConfigVector.end();++iter)
	{
		SGPMF1ConfigSetting::ParticleConfig* pParticleConfig=(SGPMF1ConfigSetting::ParticleConfig*)*iter;
		if(pParticleConfig->ParticleID==index)
		{
			delete pParticleConfig;
			bFinded=TRUE;
			if(bShow) m_ParticleConfigVector.erase(iter);
			break;
		}
	}

	if((!bFinded)&&(!bShow))
	{
		for(iter=m_ParticleConfigVector.begin();iter!=m_ParticleConfigVector.end();++iter)
		{
			SGPMF1ConfigSetting::ParticleConfig* pParticleConfig=(SGPMF1ConfigSetting::ParticleConfig*)*iter;
			if(pParticleConfig->ParticleID>(uint32)index) break;
		}
		SGPMF1ConfigSetting::ParticleConfig* pParticleConfig=new SGPMF1ConfigSetting::ParticleConfig;
		pParticleConfig->ParticleID=index;
		pParticleConfig->bVisible=false;
		m_ParticleConfigVector.insert(iter,(void*)pParticleConfig);
	}

	SGPMF1ConfigSetting* pOldConfigSetting=m_pConfigSetting;
	BuildConfigSetting();
	ModelViewerRenderInterface::GetInstance()->SetConfigSetting(m_pConfigSetting);
	if(pOldConfigSetting!=NULL) delete pOldConfigSetting;
}

void CModelDetailPane::ChangeTexture(int index,CString texturePath)
{
	char skinPath[64];
	ModelViewerRenderInterface::GetInstance()->GetMeshTextureName(index,skinPath);

	BOOL bFinded=FALSE;
	std::vector<void*>::iterator iter;
	for(iter=m_TextureConfigVector.begin();iter!=m_TextureConfigVector.end();++iter)
	{
		SGPMF1ConfigSetting::ReplaceTextureConfig* pTextureConfig=(SGPMF1ConfigSetting::ReplaceTextureConfig*)*iter;
		if(pTextureConfig->MeshID==index)
		{
			bFinded=TRUE;
			if(strcmp(skinPath,(LPCTSTR)texturePath)==0)
			{
				delete pTextureConfig;
				m_TextureConfigVector.erase(iter);
			}
			else
				strcpy_s(pTextureConfig->TexFilename,(LPCTSTR)texturePath);
			break;
		}
	}

	if((!bFinded)&&strcmp(skinPath,(LPCTSTR)texturePath)!=0)
	{
		for(iter=m_TextureConfigVector.begin();iter!=m_TextureConfigVector.end();++iter)
		{
			SGPMF1ConfigSetting::ReplaceTextureConfig* pTextureConfig=(SGPMF1ConfigSetting::ReplaceTextureConfig*)*iter;
			if(pTextureConfig->MeshID>(uint32)index) break;
		}
		SGPMF1ConfigSetting::ReplaceTextureConfig* pTextureConfig=new SGPMF1ConfigSetting::ReplaceTextureConfig;
		pTextureConfig->MeshID=index;
		strcpy_s(pTextureConfig->TexFilename,(LPCTSTR)texturePath);
		m_TextureConfigVector.insert(iter,(void*)pTextureConfig);
	}

	SGPMF1ConfigSetting* pOldConfigSetting=m_pConfigSetting;
	BuildConfigSetting();
	ModelViewerRenderInterface::GetInstance()->SetConfigSetting(m_pConfigSetting);
	if(pOldConfigSetting!=NULL) delete pOldConfigSetting;
}

void CModelDetailPane::ChangeBoundBox(CMFCPropertyGridProperty* pProp)
{
	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	CEditPropertyGridProperty* pBoxProperty=(CEditPropertyGridProperty*)pProp;
	AABBox* pBox=NULL;
	if(pBoxProperty->GetIndex()==-1)
		pBox=&(pModelMF1->m_MeshAABBox);
	else if((uint32)(pBoxProperty->GetIndex())<pModelMF1->m_Header.m_iNumMeshes)
		pBox=&(pModelMF1->m_pLOD0Meshes[pBoxProperty->GetIndex()].m_bbox);
	else return ;

	CMainFrame *pMainFrame=(CMainFrame*)AfxGetMainWnd();
	const float PRECISION=0.0001f;
	if(pBoxProperty->GetValue().fltVal-pBoxProperty->GetOriginalValue().fltVal>=PRECISION)
		pMainFrame->SetModifiedFlag();
	if(pBoxProperty->GetOriginalValue().fltVal-pBoxProperty->GetValue().fltVal>=PRECISION)
		pMainFrame->SetModifiedFlag();

	switch(pBoxProperty->GetType())
	{
	case Property_MinBox_X:
		pBox->vcMin.x=pBoxProperty->GetValue().fltVal;
		break;
	case Property_MinBox_Y:
		pBox->vcMin.y=pBoxProperty->GetValue().fltVal;
		break;
	case Property_MinBox_Z:
		pBox->vcMin.z=pBoxProperty->GetValue().fltVal;
		break;
	case Property_MaxBox_X:
		pBox->vcMax.x=pBoxProperty->GetValue().fltVal;
		break;
	case Property_MaxBox_Y:
		pBox->vcMax.y=pBoxProperty->GetValue().fltVal;
		break;
	case Property_MaxBox_Z:
		pBox->vcMax.z=pBoxProperty->GetValue().fltVal;
		break;
	}
}

void CModelDetailPane::ParticleConfigure(int index)
{
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	if(pModelMF1!=NULL)
	{
		SGPMF1ParticleTag& particleTag=pModelMF1->m_pParticleEmitter[index];
		pMainFrame->m_ParticlePane.SetParticleIndex(index);
		pMainFrame->m_ParticlePane.LoadFromData(particleTag.m_SystemParam);
		pMainFrame->m_ParticlePane.ShowPane(TRUE,FALSE,TRUE);
	}
}

void CModelDetailPane::ParticleConfigureSameTo(int index,int goal)
{
	CString strInfo;
	strInfo.Format("Are you sure to use Particle %d Setting ?",goal+1);
	if(MessageBox(strInfo,"Particle Config",MB_YESNO|MB_ICONQUESTION)==IDYES)
	{
		CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
		if(pModelMF1!=NULL)
		{
			SGPMF1ParticleTag& sourceTag=pModelMF1->m_pParticleEmitter[goal];
			SGPMF1ParticleTag& destTag=pModelMF1->m_pParticleEmitter[index];
			UnRegisterTexture(destTag);
			int pos=RetrieveParticlePos(index);
			if(pos!=-1)
			{
				ParticleSystemParam& systemParam=destTag.m_SystemParam;
				if(systemParam.m_groupCount!=0&&systemParam.m_pGroupParam!=NULL)
				{
					delete [] systemParam.m_pGroupParam;
					systemParam.m_pGroupParam=NULL;
				}
			}
			if(!(sourceTag.m_SystemParam==destTag.m_SystemParam))
				((CMainFrame*)AfxGetMainWnd())->SetModifiedFlag();
			ParticleSystemCopy(sourceTag.m_SystemParam,destTag.m_SystemParam);
			RegisterTexture(destTag);
			ModelViewerRenderInterface::GetInstance()->ResetParticleSystemByID(index,destTag);
			AddParticleIndex(index);
			pos=RetrieveParticlePos(index);
			ASSERT(pos!=-1);
			m_ParticleGroupVector[pos]=destTag.m_SystemParam.m_pGroupParam;
		}
	}
}

void CModelDetailPane::PurgeAllConfig()
{
	int i;
	int size=m_MeshConfigVector.size();
	for(i=0;i<size;++i)
	{
		SGPMF1ConfigSetting::MeshConfig* pMeshConfig=(SGPMF1ConfigSetting::MeshConfig*)m_MeshConfigVector[i];
		delete pMeshConfig;
	}
	m_MeshConfigVector.clear();
	
	size=m_TextureConfigVector.size();
	for(i=0;i<size;++i)
	{
		SGPMF1ConfigSetting::ReplaceTextureConfig* pTextureConfig=(SGPMF1ConfigSetting::ReplaceTextureConfig*)m_TextureConfigVector[i];
		delete pTextureConfig;
	}
	m_TextureConfigVector.clear();

	size=m_ParticleConfigVector.size();
	for(i=0;i<size;++i)
	{
		SGPMF1ConfigSetting::ParticleConfig* pParticleConfig=(SGPMF1ConfigSetting::ParticleConfig*)m_ParticleConfigVector[i];
		delete pParticleConfig;
	}
	m_ParticleConfigVector.clear();

	size=m_RibbonConfigVector.size();
	for(i=0;i<size;++i)
	{
		SGPMF1ConfigSetting::RibbonConfig* pRibbonConfig=(SGPMF1ConfigSetting::RibbonConfig*)m_RibbonConfigVector[i];
		delete pRibbonConfig;
	}
	m_RibbonConfigVector.clear();
	
	if(m_pConfigSetting!=NULL)
	{
		delete m_pConfigSetting;
		m_pConfigSetting=NULL;
	}
}

void CModelDetailPane::BuildConfigSetting()
{
	m_pConfigSetting=new SGPMF1ConfigSetting;
	
	m_pConfigSetting->MeshConfigNum=(uint32)m_MeshConfigVector.size();	
	if(m_pConfigSetting->MeshConfigNum!=0)
	{
		m_pConfigSetting->pMeshConfigList=new SGPMF1ConfigSetting::MeshConfig[m_pConfigSetting->MeshConfigNum];
		for(uint32 i=0;i<m_pConfigSetting->MeshConfigNum;++i)
		{
			SGPMF1ConfigSetting::MeshConfig* pMeshConfig=(SGPMF1ConfigSetting::MeshConfig*)m_MeshConfigVector[i];
			m_pConfigSetting->pMeshConfigList[i].MeshID=pMeshConfig->MeshID;
			m_pConfigSetting->pMeshConfigList[i].bVisible=pMeshConfig->bVisible;
		}
	}

	m_pConfigSetting->ReplaceTextureConfigNum=(uint32)m_TextureConfigVector.size();
	if(m_pConfigSetting->ReplaceTextureConfigNum!=NULL)
	{
		m_pConfigSetting->pReplaceTextureConfigList=new SGPMF1ConfigSetting::ReplaceTextureConfig[m_pConfigSetting->ReplaceTextureConfigNum];
		for(uint32 i=0;i<m_pConfigSetting->ReplaceTextureConfigNum;++i)
		{
			SGPMF1ConfigSetting::ReplaceTextureConfig* pTextureConfig=(SGPMF1ConfigSetting::ReplaceTextureConfig*)m_TextureConfigVector[i];
			m_pConfigSetting->pReplaceTextureConfigList[i].MeshID=pTextureConfig->MeshID;
			strcpy_s(m_pConfigSetting->pReplaceTextureConfigList[i].TexFilename,pTextureConfig->TexFilename);
		}
	}

	m_pConfigSetting->ParticleConfigNum=m_ParticleConfigVector.size();
	if(m_pConfigSetting->ParticleConfigNum!=0)
	{
		m_pConfigSetting->pParticleConfigList=new SGPMF1ConfigSetting::ParticleConfig[m_pConfigSetting->ParticleConfigNum];
		for(uint32 i=0;i<m_pConfigSetting->ParticleConfigNum;++i)
		{
			SGPMF1ConfigSetting::ParticleConfig* pParticleConfig=(SGPMF1ConfigSetting::ParticleConfig*)m_ParticleConfigVector[i];
			m_pConfigSetting->pParticleConfigList[i].ParticleID=pParticleConfig->ParticleID;
			m_pConfigSetting->pParticleConfigList[i].bVisible=pParticleConfig->bVisible;
		}
	}

	m_pConfigSetting->RibbonConfigNum=m_RibbonConfigVector.size();
	if(m_pConfigSetting->RibbonConfigNum!=0)
	{
		m_pConfigSetting->pRibbonConfigList=new SGPMF1ConfigSetting::RibbonConfig[m_pConfigSetting->RibbonConfigNum];
		for(uint32 i=0;i<m_pConfigSetting->RibbonConfigNum;++i)
		{
			SGPMF1ConfigSetting::RibbonConfig* pRibbonConfig=(SGPMF1ConfigSetting::RibbonConfig*)m_RibbonConfigVector[i];
			m_pConfigSetting->pRibbonConfigList[i].RibbonID=pRibbonConfig->RibbonID;
			m_pConfigSetting->pRibbonConfigList[i].bVisible=pRibbonConfig->bVisible;
		}
	}
}

void CModelDetailPane::OnContextMenu(CWnd* pWnd,CPoint point)
{
	
}

void CModelDetailPane::SetCtrlPos()
{
	CRect rect;
	GetClientRect(rect);

	int nTop=rect.top;
	if(m_ConfigDlg.m_hWnd!=NULL)
	{
		CRect rcConfig;
		m_ConfigDlg.GetClientRect(rcConfig);
		m_ConfigDlg.SetWindowPos(NULL,rect.left,nTop,rect.Width(),rcConfig.Height(),SWP_NOACTIVATE | SWP_NOZORDER);
		nTop+=rcConfig.Height();
	}

	if(m_DetailList.m_hWnd!=NULL)
	{
		m_DetailList.SetWindowPos(NULL, rect.left, nTop, rect.Width(), rect.bottom-nTop, SWP_NOACTIVATE | SWP_NOZORDER);
	}

}

void CModelDetailPane::LoadConfigSetting(sgp::SGPMF1ConfigSetting* configSetting)
{
	if(m_pConfigSetting!=NULL)
		delete m_pConfigSetting;
	m_pConfigSetting=new SGPMF1ConfigSetting;

	if(configSetting!=NULL)
		ConfigSettingCopy(*configSetting,*m_pConfigSetting);
	
	uint32 i,j;
	//clear
	uint32 size=m_MeshConfigVector.size();
	for(i=0;i<size;++i)
	{
		SGPMF1ConfigSetting::MeshConfig* pMeshConfig=(SGPMF1ConfigSetting::MeshConfig*)m_MeshConfigVector[i];
		delete pMeshConfig;
	}
	m_MeshConfigVector.clear();

	size=m_TextureConfigVector.size();
	for(i=0;i<size;++i)
	{
		SGPMF1ConfigSetting::ReplaceTextureConfig* pTextureConfig=(SGPMF1ConfigSetting::ReplaceTextureConfig*)m_TextureConfigVector[i];
		delete pTextureConfig;
	}
	m_TextureConfigVector.clear();

	size=m_ParticleConfigVector.size();
	for(i=0;i<size;++i)
	{
		SGPMF1ConfigSetting::ParticleConfig* pParticleConfig=(SGPMF1ConfigSetting::ParticleConfig*)m_ParticleConfigVector[i];
		delete pParticleConfig;
	}
	m_ParticleConfigVector.clear();

	size=m_RibbonConfigVector.size();
	for(i=0;i<size;++i)
	{
		SGPMF1ConfigSetting::RibbonConfig* pRibbonConfig=(SGPMF1ConfigSetting::RibbonConfig*)m_RibbonConfigVector[i];
		delete pRibbonConfig;
	}
	m_RibbonConfigVector.clear();

	// build
	for(i=0;i<m_pConfigSetting->MeshConfigNum;++i)
	{
		SGPMF1ConfigSetting::MeshConfig* pTextureConfig=new SGPMF1ConfigSetting::MeshConfig;
		*pTextureConfig=m_pConfigSetting->pMeshConfigList[i];
		m_MeshConfigVector.push_back((void*)pTextureConfig);
	}
	for(i=0;i<m_pConfigSetting->ParticleConfigNum;++i)
	{
		SGPMF1ConfigSetting::ParticleConfig* pParticleConfig=new SGPMF1ConfigSetting::ParticleConfig;
		*pParticleConfig=m_pConfigSetting->pParticleConfigList[i];
		m_ParticleConfigVector.push_back((void*)pParticleConfig);
	}
	for(i=0;i<m_pConfigSetting->ReplaceTextureConfigNum;++i)
	{
		SGPMF1ConfigSetting::ReplaceTextureConfig* pTextureConfig=new SGPMF1ConfigSetting::ReplaceTextureConfig;
		pTextureConfig->MeshID=m_pConfigSetting->pReplaceTextureConfigList[i].MeshID;
		strcpy_s(pTextureConfig->TexFilename,m_pConfigSetting->pReplaceTextureConfigList[i].TexFilename);
		m_TextureConfigVector.push_back((void*)pTextureConfig);
	}
	for(i=0;i<m_pConfigSetting->RibbonConfigNum;++i)
	{
		SGPMF1ConfigSetting::RibbonConfig* pRibbonConfig=new SGPMF1ConfigSetting::RibbonConfig;
		*pRibbonConfig=m_pConfigSetting->pRibbonConfigList[i];
		m_RibbonConfigVector.push_back((void*)pRibbonConfig);
	}

	// control status update
	// Mesh
	for(i=0;i<m_MeshPropertyVector.size();++i)
	{
		BOOL bCheck=TRUE;
		for(j=0;j<m_pConfigSetting->MeshConfigNum;++j)
		{
			if(m_pConfigSetting->pMeshConfigList[j].MeshID==i)
			{
				if(m_pConfigSetting->pMeshConfigList[j].bVisible)
					bCheck=TRUE;
				else
					bCheck=FALSE;
			}
		}
		m_MeshPropertyVector[i]->SetCheck(bCheck);
	}
	// Particle
	for(i=0;i<m_ParticlePropertyVector.size();++i)
	{
		BOOL bCheck=TRUE;
		for(j=0;j<m_pConfigSetting->ParticleConfigNum;++j)
		{
			if(m_pConfigSetting->pParticleConfigList[j].ParticleID==i)
			{
				if(m_pConfigSetting->pParticleConfigList[j].bVisible) bCheck=TRUE;
				else bCheck=FALSE;
			}
		}
		m_ParticlePropertyVector[i]->SetCheck(bCheck);
	}
	// Texture
	for(i=0;i<m_TexturePropertyVector.size();++i)
	{
		BOOL bFinded=FALSE;
		for(j=0;j<m_pConfigSetting->ReplaceTextureConfigNum;++j)
		{
			if(m_pConfigSetting->pReplaceTextureConfigList[j].MeshID==i)
			{
				m_TexturePropertyVector[i]->SetValue(COleVariant(m_pConfigSetting->pReplaceTextureConfigList[j].TexFilename));
				bFinded=TRUE;
				break;
			}
		}
		if(!bFinded)
		{
			if(m_TexturePropertyVector[i]!=NULL)
			{
				char texPath[64];
				ModelViewerRenderInterface::GetInstance()->GetMeshTextureName(i,texPath);
				m_TexturePropertyVector[i]->SetValue(texPath);
			}
		}
	}
}

void CModelDetailPane::ReleaseParticleTag()
{
	m_ParticleIndexVector.clear();
}

int CModelDetailPane::RetrieveParticlePos(int index)
{
	int size=m_ParticleIndexVector.size();
	for(int i=0;i<size;++i)
	{
		if(m_ParticleIndexVector[i]==index) return i;
	}
	return -1;
}

void CModelDetailPane::AddParticleIndex(int index)
{
	if(RetrieveParticlePos(index)==-1)
	{
		m_ParticleIndexVector.push_back(index);
		m_ParticleGroupVector.push_back(NULL);
	}
}

void CModelDetailPane::ReleaseAllocatedMF1()
{
	for(uint32 i=0;i<m_ParticleGroupVector.size();++i)
	{
		if(m_ParticleGroupVector[i]!=NULL)
		{
			delete [] m_ParticleGroupVector[i];
			m_ParticleGroupVector[i]=NULL;
		}
	}
	m_ParticleGroupVector.clear();
}