#pragma once

class ModelViewerRender;

/***********************************************************************
this class is used for render
for convenience,it is a singleton
***********************************************************************/
namespace sgp
{
	class CSGPModelMF1;
	class ISGPRenderDevice;
	class CMF1FileResource;
	class CSkeletonMeshInstance;
	struct SGPMF1ConfigSetting;
	struct SGPMF1ParticleTag;
}

class ModelViewerRenderInterface
{
	ModelViewerRenderInterface();
	~ModelViewerRenderInterface();
public:
	//Init:this function should before rendering
	bool Init(HWND hwnd,CRichEditCtrl *pCtrl);
	//release resources
	void Release();
	//rendering
	void Render();

	void ChangeModel(wchar_t* strPath,BOOL bExist=TRUE);
	void CloseModel();
	void ResizeWindow(int cx,int cy);
	//camera control
	void MoveCamera(float cx,float cy);
	void RotateCamera(float cx,float cy);
	void ZoomCamera(float fZoom);

	//whether initialized
	bool IsInitialized();
	//play animation
	void PlayAnimation(float fSpeed,int nStartFrame,int nEndFrame,bool bLoop=true,bool bNewAnim=true);
	void PlayMixedAnimation(unsigned int upperStart,unsigned int upperEnd,unsigned int lowerStart,unsigned int lowerEnd);

	// show log
	void ShowWarningLog(CString str);
	void ShowErrorLog(CString str);
	void ShowInfoLog(CString str);

	sgp::CSGPModelMF1* GetModelMF1();
	sgp::CMF1FileResource* GetModelResource();
	sgp::ISGPRenderDevice* GetRenderDevice();
	sgp::CSkeletonMeshInstance* GetSkeletonInstance();
	void SetConfigSetting(sgp::SGPMF1ConfigSetting* pConfigSetting);
	BOOL GetMeshTextureName(UINT index,char* texPath);
	void ResetParticleSystemByID(UINT partID,const sgp::SGPMF1ParticleTag& newSetting,bool* bShowFlag=NULL);
	void RegisterTexture(char* texPath);
	void UnRegisterTexture(char* texPath);
public:
	static ModelViewerRenderInterface* GetInstance();
	static void ReleaseInstance();
private:
	static ModelViewerRenderInterface* sInstance;
private:
	ModelViewerRender *m_pRender;
};