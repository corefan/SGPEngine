#include "stdafx.h"
#include "ModelViewerRender.h"
#include "ModelViewerRenderInterface.h"

ModelViewerRenderInterface* ModelViewerRenderInterface::sInstance=NULL;

ModelViewerRenderInterface::ModelViewerRenderInterface()
{
	m_pRender=new ModelViewerRender;
}

ModelViewerRenderInterface::~ModelViewerRenderInterface()
{
	if(m_pRender!=NULL)
	{
		delete m_pRender;
		m_pRender=NULL;
	}
}

ModelViewerRenderInterface* ModelViewerRenderInterface::GetInstance()
{
	if(sInstance==NULL)
		sInstance=new ModelViewerRenderInterface;
	return sInstance;
}
void ModelViewerRenderInterface::ReleaseInstance()
{
	if(sInstance!=NULL)
	{
		delete sInstance;
		sInstance=NULL;
	}
}

bool ModelViewerRenderInterface::Init(HWND hwnd,CRichEditCtrl *pCtrl)
{
	return m_pRender->Init(hwnd,pCtrl); 
}

void ModelViewerRenderInterface::Release()
{
	m_pRender->Release();
}

void ModelViewerRenderInterface::Render()
{
	m_pRender->Render();
}

void ModelViewerRenderInterface::ResizeWindow(int cx,int cy)
{
	m_pRender->ResizeWindow(cx,cy);
}

void ModelViewerRenderInterface::ChangeModel(wchar_t* strPath,BOOL bExist)
{
	m_pRender->ChangeModel(strPath,bExist);
}

void ModelViewerRenderInterface::MoveCamera(float cx,float cy)
{
	if(m_pRender->m_pCamera!=NULL)
		m_pRender->m_pCamera->Move(cx,cy);
}

void ModelViewerRenderInterface::RotateCamera(float cx,float cy)
{
	if(m_pRender->m_pCamera!=NULL)
		m_pRender->m_pCamera->Rotate(cx,cy);
}

void ModelViewerRenderInterface::ZoomCamera(float fZoom)
{
	if(m_pRender->m_pCamera!=NULL)
		m_pRender->m_pCamera->Zoom(fZoom);
}

bool ModelViewerRenderInterface::IsInitialized()
{
	return m_pRender->m_bInitilized;
}

CSGPModelMF1* ModelViewerRenderInterface::GetModelMF1()
{
	return m_pRender->m_pModelMF1;
}

sgp::CMF1FileResource* ModelViewerRenderInterface::GetModelResource()
{
	return m_pRender->m_pMF1FileResource;
}

void ModelViewerRenderInterface::PlayAnimation(float fSpeed,int nStartFrame,int nEndFrame,bool bLoop,bool bNewAnim)
{
	if(m_pRender->m_pDynamicModel!=NULL)
	{
		m_pRender->m_pDynamicModel->setEnableUpperBodyAnim(false);
		m_pRender->m_pDynamicModel->playAnim(fSpeed,(uint32)nStartFrame,(uint32)nEndFrame,bLoop,bNewAnim);
	}
}

void ModelViewerRenderInterface::PlayMixedAnimation(unsigned int upperStart,unsigned int upperEnd,unsigned int lowerStart,unsigned int lowerEnd)
{
	if(m_pRender->m_pDynamicModel!=NULL)
	{
		m_pRender->m_pDynamicModel->setEnableUpperBodyAnim(true);
		m_pRender->m_pDynamicModel->playUpperBodyAnim(1.0f,upperStart,upperEnd,true,true);
		m_pRender->m_pDynamicModel->playAnim(1.0f,lowerStart,lowerEnd,true,true);
	}
}

void ModelViewerRenderInterface::ShowWarningLog(CString str)
{
	WCHAR wPath[MAX_PATH];
	wmemset(wPath,0,MAX_PATH);
	MultiByteToWideChar(CP_ACP,0,(LPCTSTR)str,str.GetLength(),wPath,MAX_PATH);
	Logger::getCurrentLogger()->writeToLog(String(wPath),ELL_WARNING);
}

void ModelViewerRenderInterface::ShowErrorLog(CString str)
{
	WCHAR wPath[MAX_PATH];
	wmemset(wPath,0,MAX_PATH);
	MultiByteToWideChar(CP_ACP,0,(LPCTSTR)str,str.GetLength(),wPath,MAX_PATH);
	Logger::getCurrentLogger()->writeToLog(String(wPath),ELL_ERROR);
}

void ModelViewerRenderInterface::ShowInfoLog(CString str)
{
	WCHAR wPath[MAX_PATH];
	wmemset(wPath,0,MAX_PATH);
	MultiByteToWideChar(CP_ACP,0,(LPCTSTR)str,str.GetLength(),wPath,MAX_PATH);
	Logger::getCurrentLogger()->writeToLog(String(wPath),ELL_INFORMATION);
}

sgp::ISGPRenderDevice* ModelViewerRenderInterface::GetRenderDevice()
{
	return m_pRender->m_pRenderDevice;
}

sgp::CSkeletonMeshInstance* ModelViewerRenderInterface::GetSkeletonInstance()
{
	return m_pRender->m_pDynamicModel;
}

void ModelViewerRenderInterface::SetConfigSetting(sgp::SGPMF1ConfigSetting* pConfigSetting)
{
	if(m_pRender->m_pDynamicModel!=NULL)
		m_pRender->m_pDynamicModel->setConfigSetting(pConfigSetting);
	if(m_pRender->m_pStaticModel!=NULL)
		m_pRender->m_pStaticModel->setConfigSetting(pConfigSetting);
}

BOOL ModelViewerRenderInterface::GetMeshTextureName(UINT index,char* texPath)
{
	CSGPModelMF1* pModelMF1=m_pRender->m_pModelMF1;
	SGPMF1Mesh& mf1Mesh=pModelMF1->m_pLOD0Meshes[index];
	if(mf1Mesh.m_SkinIndex==0xFFFFFFFF)
		return FALSE;
	else
	{
		SGPMF1Skin& skin=pModelMF1->m_pSkins[mf1Mesh.m_SkinIndex];
		memcpy(texPath,skin.m_cName,sizeof(skin.m_cName));

		return TRUE;
	}
}

void ModelViewerRenderInterface::RegisterTexture(char* texPath)
{
	m_pRender->m_pRenderDevice->GetTextureManager()->registerTexture(String(texPath));
}

void ModelViewerRenderInterface::UnRegisterTexture(char* texPath)
{
	m_pRender->m_pRenderDevice->GetTextureManager()->unRegisterTextureByName(String(texPath));
}

void ModelViewerRenderInterface::ResetParticleSystemByID(UINT partID,const SGPMF1ParticleTag& newSetting,bool* bShowFlag)
{
	if(m_pRender->m_pDynamicModel!=NULL)
		m_pRender->m_pDynamicModel->resetParticleSystemByID(partID,newSetting,bShowFlag);
	if(m_pRender->m_pStaticModel!=NULL)
		m_pRender->m_pStaticModel->resetParticleSystemByID(partID,newSetting,bShowFlag);
}

void ModelViewerRenderInterface::CloseModel()
{
	m_pRender->CloseModel();
}
