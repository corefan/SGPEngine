#include "stdafx.h"
#include "Resource.h"
#include "ModelViewerRender.h"
#include "ModelViewerConfig.h"
#include "ModelControlDlg.h"
#include "MainFrame.h"
#include "ModelDetailPane.h"

ModelViewerRender::ModelViewerRender():\
	m_pDevice(NULL),\
	m_pRenderDevice(NULL),\
	m_pLogger(NULL),\
	m_pCamera(NULL),\
	m_pDynamicModel(NULL),\
	m_pStaticModel(NULL),\
	m_bInitilized(false),\
	m_iModelID(0xFFFFFFFF),\
	m_pMF1FileResource(NULL),\
	m_pModelMF1(NULL),\
	m_ModelPath("")
{
}

ModelViewerRender::~ModelViewerRender()
{

}

bool ModelViewerRender::Init(HWND hwnd,CRichEditCtrl *pCtrl)
{
	//init logger
	m_pLogger=new RichEditLogger(pCtrl);
	Logger::setCurrentLogger(m_pLogger);

	//create device
	SGPCreationParameters param;
	param.DriverType = SGPDT_OPENGL;
	param.Vsync = true;
	param.WindowId = reinterpret_cast<void*>(hwnd);
	param.plog = m_pLogger;
	param.MultiThreadResLoading=false;
	m_pDevice = createDeviceEx(param);
	if(m_pDevice==NULL)
	{
		Logger::setCurrentLogger(nullptr);
		return false;
	}
	m_pDevice->setResizable(true);

	//get render device
	m_pRenderDevice = m_pDevice->getRenderDevice();

	//set working directory
	TCHAR path[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),path,MAX_PATH);
	CString str=path;
	while(str.Right(4)!=_T("\\Bin")) str=str.Left(str.ReverseFind('\\'));
	str=str.Left(str.ReverseFind('\\'));
	str+="\\Data";
	WCHAR wPath[MAX_PATH];
	wmemset(wPath,0,MAX_PATH);
	MultiByteToWideChar(CP_ACP,0,(LPCTSTR)str,str.GetLength(),wPath,MAX_PATH);
	String WorkingDir(wPath);
	m_pRenderDevice->setWorkingDirection(WorkingDir);

	//init internal camera mode
	m_pRenderDevice->setCameraMode(SGPCT_PERSPECTIVE);
	m_pRenderDevice->setNearFarClipPlane(0.01f, 100.0f);	
	m_pRenderDevice->setFov(45.0f);

	RECT rc;
	::GetWindowRect(hwnd,&rc);
	m_pRenderDevice->onResize(rc.right-rc.left,rc.bottom-rc.top);
	SViewPort viewarea;
	viewarea.Width = m_pRenderDevice->getScreenSize().Width;
	viewarea.Height = m_pRenderDevice->getScreenSize().Height;
	m_pRenderDevice->setViewPort(viewarea);

	//init camera
	m_pCamera=new ModelViewerCamera;
	m_pCamera->SetPos(0,100,-100);
	m_pCamera->SetLookAt(0, 0, 0);
	m_pCamera->SetRotation(-float_Pi/7, -float_Pi/3, 0);

	// init Font// could not create selected font.
	if( !m_pRenderDevice->CreateFontInManager( "font1", String(L"Font\\YaHei_Consolas.ttf"), true, false, 16 ) )
	{
		delete m_pDevice;
		m_pDevice = NULL;
		Logger::setCurrentLogger(nullptr);
		return false;
	}

	InitGround();

	m_pRenderDevice->GetWorldSystemManager()->createWorldSun();
	m_pRenderDevice->GetWorldSystemManager()->setWorldSunPosition(120.0f);
	m_bInitilized=true;

	return true;
}

void ModelViewerRender::Release()
{
	m_pRenderDevice->GetWorldSystemManager()->shutdownWorld();
	if(m_pDynamicModel!=NULL)
	{
		m_pDynamicModel->destroyModel();
		delete m_pDynamicModel;
		m_pDynamicModel=NULL;
	}
	if(m_pStaticModel!=NULL)
	{
		m_pStaticModel->destroyModel();
		delete m_pStaticModel;
		m_pStaticModel=NULL;
	}

	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_ModelDetailPane.ReleaseAllocatedMF1();

	m_pModelMF1=NULL;
	if(m_pCamera!=NULL)
	{
		delete m_pCamera;
		m_pCamera=NULL;
	}
	Logger::setCurrentLogger(nullptr);
	if(m_pLogger!=NULL)
	{
		delete m_pLogger;
		m_pLogger=NULL;
	}
	if(m_pDevice!=NULL)
	{
		delete m_pDevice;
		m_pDevice=NULL;
	}
	m_bInitilized=false;
}

void ModelViewerRender::Render()
{
	if(!m_bInitilized) return ;//haven't initialized

	m_pDevice->getTimer()->tick();
	// Work out a frame delta time.
	const double frameDeltaTime = m_pDevice->getTimer()->GetElapsedTime() / 1000.0; // Time in seconds
	m_pRenderDevice->setRenderDeviceTime(m_pDevice->getTimer()->getTime(), frameDeltaTime);
	
	// notify the model current frame changed
	if(m_pMF1FileResource!=NULL&&m_pDynamicModel!=NULL)
		CModelControlDlg::NotifyPlayFrameChanged((int)(m_pDynamicModel->getAnimPlayedTime()/ISGPInstanceManager::DefaultSecondsPerFrame));
		
	m_pCamera->Update((float)frameDeltaTime);
	if(m_pDynamicModel!=NULL) m_pDynamicModel->update((float)frameDeltaTime);
	if(m_pStaticModel!=NULL) m_pStaticModel->update((float)frameDeltaTime);
	// update internal Camera position
	m_pRenderDevice->setViewMatrix3D( m_pCamera->GetRight(),m_pCamera->GetUp(),m_pCamera->GetDir(),m_pCamera->GetPos() );
	// calculate fps
	CalFPS(frameDeltaTime);
	
	ModelViewerConfig* pConfig=ModelViewerConfig::GetInstance();
	if(pConfig->m_bShowWireframe) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	m_pRenderDevice->setClearColor(pConfig->m_BkgndColor[0],pConfig->m_BkgndColor[1],\
		pConfig->m_BkgndColor[2],pConfig->m_BkgndColor[3]);

	m_pRenderDevice->beginScene(true,true,false);
	// render ground
	if(pConfig->m_bShowGround) RenderGround();
	// render model
	if(pConfig->m_bShowMesh)
	{
		if(m_pDynamicModel!=NULL) m_pDynamicModel->render();
		if(m_pStaticModel!=NULL) m_pStaticModel->render();
	}
	// render boundingbox
	if(pConfig->m_bShowBoundingBox&&m_pModelMF1!=NULL)
		RenderBoundingBox(m_pModelMF1->m_MeshAABBox);
	// render bones
	if(pConfig->m_bShowBones)
	{
		if(m_pDynamicModel!=NULL) RenderBones();
	}
	// render normals
	if(pConfig->m_bShowNormalLines)
	{
		if(m_pDynamicModel!=NULL) RenderDynamicNormals();
		if(m_pStaticModel!=NULL) RenderStaticNormals();
	}
	// render mesh box
	int meshcount=pConfig->m_bBoxShowVector.size();
	for(int i=0;i<meshcount&&m_pModelMF1!=NULL;++i)
	{
		// render mesh box
		if(pConfig->m_bBoxShowVector[i]==TRUE)
			RenderBoundingBox(m_pModelMF1->m_pLOD0Meshes[i].m_bbox);
	}
	
	// render coordinate axis
	RenderCoordinateAxis();
	
	// flush:draw objects
	m_pRenderDevice->FlushRenderBatch();

	// render attachment
	if(pConfig->m_bShowAttachment&&m_pModelMF1!=NULL) RenderAttachments();
	// render particle emitter
	if(pConfig->m_bShowParticleEmitter&&m_pModelMF1!=NULL) RenderParticleEmitter();
	m_pRenderDevice->FlushEditorLinesRenderBatch(true);

	m_pRenderDevice->BeginRenderText();
	RenderCoordinateLabel();
	RenderFPS();
	m_pRenderDevice->EndRenderText();

	m_pRenderDevice->endScene();
}

void ModelViewerRender::ChangeModel(wchar_t *modelPath,BOOL bDynamic)
{
	if(m_pStaticModel!=NULL)
	{
		m_pStaticModel->destroyModel();
		delete m_pStaticModel;
		m_pStaticModel=NULL;
	}
	if(m_pDynamicModel!=NULL)
	{
		m_pDynamicModel->destroyModel();
		delete m_pDynamicModel;
		m_pDynamicModel=NULL;
	}

	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
	pMainFrame->ClearModifiedFlag();
	pMainFrame->m_ModelDetailPane.ReleaseAllocatedMF1();

	if(bDynamic)
	{
		m_pDynamicModel=new CSkeletonMeshInstance(m_pRenderDevice);
		m_pDynamicModel->setVisible(true);

		String path(modelPath);
		m_ModelPath=path;
		m_pDynamicModel->changeModel(path);
		m_iModelID=m_pRenderDevice->GetModelManager()->getModelIDByName(path);
		m_pMF1FileResource = m_pRenderDevice->GetModelManager()->getModelByID(m_iModelID);
		m_pModelMF1=m_pMF1FileResource->pModelMF1;
		int num=m_pMF1FileResource->pModelMF1->m_Header.m_iNumActionList;
		SGPMF1ActionList& paction=m_pMF1FileResource->pModelMF1->m_pActionLists[0];
		m_pDynamicModel->playAnim(1.0f,paction.m_iActionStart,/*paction.m_iActionEnd*/paction.m_iActionStart,true);

		// init show flag
		int meshCount=m_pMF1FileResource->pModelMF1->m_Header.m_iNumMeshes;
		ModelViewerConfig* pConfig=ModelViewerConfig::GetInstance();
		pConfig->m_bBoxShowVector.clear();
		for(int i=0;i<meshCount;++i) pConfig->m_bBoxShowVector.push_back(FALSE);
	}
	else
	{
		m_pStaticModel=new CStaticMeshInstance(m_pRenderDevice);
		m_pStaticModel->setVisible(true);

		String path(modelPath);
		m_ModelPath=path;
		m_pStaticModel->changeModel(path);
		m_iModelID=m_pRenderDevice->GetModelManager()->getModelIDByName(path);
		m_pMF1FileResource = m_pRenderDevice->GetModelManager()->getModelByID(m_iModelID);
		m_pModelMF1=m_pMF1FileResource->pModelMF1;

		// init show flag
		int meshCount=m_pMF1FileResource->pModelMF1->m_Header.m_iNumMeshes;
		ModelViewerConfig* pConfig=ModelViewerConfig::GetInstance();
		pConfig->m_bBoxShowVector.clear();
		for(int i=0;i<meshCount;++i) pConfig->m_bBoxShowVector.push_back(FALSE);
	}
}

void ModelViewerRender::CloseModel()
{
	if(m_pDynamicModel!=NULL)
	{
		m_pDynamicModel->destroyModel();
		delete m_pDynamicModel;
		m_pDynamicModel=NULL;
	}
	if(m_pStaticModel!=NULL)
	{
		m_pStaticModel->destroyModel();
		delete m_pStaticModel;
		m_pStaticModel=NULL;
	}
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_ModelDetailPane.ReleaseAllocatedMF1();
	m_pModelMF1=NULL;
}

void ModelViewerRender::InitGround()
{
	int vertIndex=0;
	for( int Row=-13; Row<=13; Row++ )
	{
		SetVertexPos(m_SurfaceVertex[vertIndex],Row*0.5f,0,-13*0.5f);
		SetVertexColor(m_SurfaceVertex[vertIndex],((Row==0)?0:0.6f),((Row==0)?0:0.6f),((Row==0)?0:0.6f),1.0f);
		vertIndex++;
		SetVertexPos(m_SurfaceVertex[vertIndex],Row*0.5f,0,13*0.5f);
		SetVertexColor(m_SurfaceVertex[vertIndex],((Row==0)?0:0.6f),((Row==0)?0:0.6f),((Row==0)?0:0.6f),1.0f);
		vertIndex++;
	}
	for( int Col=-13; Col<=13; Col++ )
	{
		SetVertexPos(m_SurfaceVertex[vertIndex],-13*0.5f,0,Col*0.5f);
		SetVertexColor(m_SurfaceVertex[vertIndex],(Col==0)?0:0.6f,(Col==0)?0:0.6f,(Col==0)?0:0.6f,1.0f);
		vertIndex++;
		SetVertexPos(m_SurfaceVertex[vertIndex],13*0.5f,0,Col*0.5f);
		SetVertexColor(m_SurfaceVertex[vertIndex],(Col==0)?0:0.6f,(Col==0)?0:0.6f,(Col==0)?0:0.6f,1.0f);
		vertIndex++;
	}
}

void ModelViewerRender::RenderGround()
{
	m_pRenderDevice->GetVertexCacheManager()->RenderLines(sizeof(m_SurfaceVertex)/sizeof(SGPVertex_UPOS_VERTEXCOLOR),m_SurfaceVertex,false);
}

void ModelViewerRender::RenderCoordinateAxis()
{
	Matrix4x4 matView, matProj, matInvViewProj;
	Vector4D OrigTo;

	// proj position
	Vector4D vAxisOrig( -0.75f, -0.75f, 0.1f);
	m_pRenderDevice->getViewMatrix(matView);
	m_pRenderDevice->getProjMatrix(matProj);

	// Inverse view*proj to get "proj->world" transform
	matInvViewProj.InverseOf( matView*matProj );
	// move the obj to leftbottom of camera then transfer to world coordinate
	Vector4D Orig = vAxisOrig * matInvViewProj;
	Orig.x /= Orig.w;
	Orig.y /= Orig.w;
	Orig.z /= Orig.w;
	Orig.w = 1;

	// draw the lines stand for coordinate
	const float LINE_LENGTH = 0.002f;

	// RED X
	OrigTo = Orig + Vector4D(LINE_LENGTH,0,0);
	SetVertexPos(m_AxisVertex[0],Orig.x,Orig.y,Orig.z);
	SetVertexColor(m_AxisVertex[0],1.0f,0.0f,0.0f,1.0f);
	SetVertexPos(m_AxisVertex[1],OrigTo.x,OrigTo.y,OrigTo.z);
	SetVertexColor(m_AxisVertex[1],1.0f,0.0f,0.0f,1.0f);
	// GREEN Y
	OrigTo = Orig + Vector4D(0,LINE_LENGTH,0);
	SetVertexPos(m_AxisVertex[2],Orig.x,Orig.y,Orig.z);
	SetVertexColor(m_AxisVertex[2],0.0f,1.0f,0.0f,1.0f);
	SetVertexPos(m_AxisVertex[3],OrigTo.x,OrigTo.y,OrigTo.z);
	SetVertexColor(m_AxisVertex[3],0.0f,1.0f,0.0f,1.0f);
	// BLUE Z
	OrigTo = Orig + Vector4D(0,0,LINE_LENGTH);
	SetVertexPos(m_AxisVertex[4],Orig.x,Orig.y,Orig.z);
	SetVertexColor(m_AxisVertex[4],0.0f,0.0f,1.0f,1.0f);
	SetVertexPos(m_AxisVertex[5],OrigTo.x,OrigTo.y,OrigTo.z);
	SetVertexColor(m_AxisVertex[5],0.0f,0.0f,1.0f,1.0f);

	m_pRenderDevice->GetVertexCacheManager()->RenderLines(6,m_AxisVertex,false);
}

void ModelViewerRender::RenderCoordinateLabel()
{
	Matrix4x4 matView, matProj;
	m_pRenderDevice->getViewMatrix(matView);
	m_pRenderDevice->getProjMatrix(matProj);
	Matrix4x4 matWorldToScreen=matView * matProj;
	uint32 width=m_pRenderDevice->getViewPort().Width;
	uint32 height=m_pRenderDevice->getViewPort().Height;

	//draw x label
	Vector4D Lable_x(m_AxisVertex[1].x, m_AxisVertex[1].y, m_AxisVertex[1].z);
	Lable_x = Lable_x * matWorldToScreen;
	Lable_x.x /= Lable_x.w;
	Lable_x.y /= Lable_x.w;
	m_pRenderDevice->DrawTextInPos(int((Lable_x.x+1)*0.5f*width),int((1-Lable_x.y)*0.5f*height-16),0,16,255,0,0,L"x");
	//draw y label
	Vector4D Lable_y(m_AxisVertex[3].x, m_AxisVertex[3].y, m_AxisVertex[3].z);
	Lable_y = Lable_y * matWorldToScreen;
	Lable_y.x /= Lable_y.w;
	Lable_y.y /= Lable_y.w;
	m_pRenderDevice->DrawTextInPos(int((Lable_y.x+1)*0.5f*width),int((1-Lable_y.y)*0.5f*height-16),0,16,0,255,0,L"y");
	// draw z label
	Vector4D Lable_z(m_AxisVertex[5].x, m_AxisVertex[5].y, m_AxisVertex[5].z);
	Lable_z = Lable_z * matWorldToScreen;
	Lable_z.x /= Lable_z.w;
	Lable_z.y /= Lable_z.w;
	m_pRenderDevice->DrawTextInPos(int((Lable_z.x+1)*0.5f*width),int((1-Lable_z.y)*0.5f*height-16),0,16,0,0,255,L"z");
}

void ModelViewerRender::CalFPS(double deltaTime)
{
	static double averageMS=0;
	static DWORD dwStart=0;
	averageMS += (deltaTime*1000.0 - averageMS) * 0.1;
	if(GetTickCount()-dwStart>=500)
	{
		m_FPS=1000.0/averageMS;
		dwStart=GetTickCount();
	}
}

void ModelViewerRender::RenderFPS()
{
//	m_pRenderDevice->DrawTextInPos(10,10,SGPFDL_SHADOW,16,200,200,200,L"FPS = %.1f",m_FPS);
	m_pRenderDevice->DrawTextInPos(10,10,SGPFDL_SHADOW,16,200,200,200,L"FPS = %d",(int)m_FPS);
}

void ModelViewerRender::ResizeWindow(int cx,int cy)
{
	if(m_bInitilized)
		m_pRenderDevice->onResize(cx,cy);
}

void ModelViewerRender::RenderBoundingBox(AABBox& box)
{
	m_pRenderDevice->GetVertexCacheManager()->RenderBox(box,Colour(0xFFFFFFFF));
}

void ModelViewerRender::RenderBones()
{
	if(m_pModelMF1==NULL) return ;

//	const float boneOffset[3]={0.05f,0.05f,0.05f};
	const float boneOffset[3]={0.0f,0.0f,0.0f};

	uint32 nBoneCount=m_pModelMF1->m_iNumBones;
	SGPMF1Bone* pBones=m_pModelMF1->m_pBones;
	float* pBoneMatrixs=m_pDynamicModel->getBonesMatrix();
	SGPVertex_UPOS_VERTEXCOLOR vertex[2];
	SetVertexColor(vertex[0],1.0f,1.0f,1.0f,1.0f);
	SetVertexColor(vertex[1],1.0f,1.0f,1.0f,1.0f);
	for(uint32 i=0;i<nBoneCount;++i)
	{
		uint32 childCount=pBones[i].m_iNumChildId;
		if(childCount!=0)
		{
			Matrix4x4 matFrame0;
			matFrame0.InverseOf(pBones[i].m_matFrame0Inv);

			Matrix4x4 matAnimation;
			matAnimation.Identity();
			matAnimation._11=pBoneMatrixs[12*i];
			matAnimation._21=pBoneMatrixs[12*i+1];
			matAnimation._31=pBoneMatrixs[12*i+2];
			matAnimation._41=pBoneMatrixs[12*i+3];
			matAnimation._12=pBoneMatrixs[12*i+4];
			matAnimation._22=pBoneMatrixs[12*i+5];
			matAnimation._32=pBoneMatrixs[12*i+6];
			matAnimation._42=pBoneMatrixs[12*i+7];
			matAnimation._13=pBoneMatrixs[12*i+8];
			matAnimation._23=pBoneMatrixs[12*i+9];
			matAnimation._33=pBoneMatrixs[12*i+10];
			matAnimation._43=pBoneMatrixs[12*i+11];

			Matrix4x4 matOrigin=matFrame0*matAnimation*m_pDynamicModel->getModelMatrix();
		//	SetVertexPos(vertex[0],matOrigin._41+0.05f,matOrigin._42+0.05f,matOrigin._43+0.05f);
			SetVertexPos(vertex[0],matOrigin._41+boneOffset[0],matOrigin._42+boneOffset[1],matOrigin._43+boneOffset[2]);
			if(pBones[i].m_sParentID==0xFFFF)
			{
				AABBox aabox=m_pModelMF1->m_MeshAABBox;
				float length=(aabox.vcMax-aabox.vcMin).GetLength()/150.0f;
				m_pRenderDevice->GetVertexCacheManager()->RenderDetailSphere(Vector3D(matOrigin._41+boneOffset[0],\
					matOrigin._42+boneOffset[1],matOrigin._43+boneOffset[2]),length,5,5,Colour(255,255,255));
			}

			for(uint32 j=0;j<childCount;++j)
			{	
				uint16 index=pBones[i].m_ChildIds[j];
				matFrame0.InverseOf(pBones[index].m_matFrame0Inv);
				matAnimation.Identity();
				matAnimation._11=pBoneMatrixs[12*index];
				matAnimation._21=pBoneMatrixs[12*index+1];
				matAnimation._31=pBoneMatrixs[12*index+2];
				matAnimation._41=pBoneMatrixs[12*index+3];
				matAnimation._12=pBoneMatrixs[12*index+4];
				matAnimation._22=pBoneMatrixs[12*index+5];
				matAnimation._32=pBoneMatrixs[12*index+6];
				matAnimation._42=pBoneMatrixs[12*index+7];
				matAnimation._13=pBoneMatrixs[12*index+8];
				matAnimation._23=pBoneMatrixs[12*index+9];
				matAnimation._33=pBoneMatrixs[12*index+10];
				matAnimation._43=pBoneMatrixs[12*index+11];

				matOrigin=matFrame0*matAnimation*m_pDynamicModel->getModelMatrix();
				SetVertexPos(vertex[1],matOrigin._41+boneOffset[0],matOrigin._42+boneOffset[1],matOrigin._43+boneOffset[2]);
				m_pRenderDevice->GetVertexCacheManager()->RenderLines(2,vertex,false);
			}
		}
		else if(pBones[i].m_sParentID==0xFFFF)//the bone don't have children
		{
			Matrix4x4 matFrame0;
			matFrame0.InverseOf(pBones[i].m_matFrame0Inv);

			Matrix4x4 matAnimation;
			matAnimation.Identity();
			matAnimation._11=pBoneMatrixs[12*i];
			matAnimation._21=pBoneMatrixs[12*i+1];
			matAnimation._31=pBoneMatrixs[12*i+2];
			matAnimation._41=pBoneMatrixs[12*i+3];
			matAnimation._12=pBoneMatrixs[12*i+4];
			matAnimation._22=pBoneMatrixs[12*i+5];
			matAnimation._32=pBoneMatrixs[12*i+6];
			matAnimation._42=pBoneMatrixs[12*i+7];
			matAnimation._13=pBoneMatrixs[12*i+8];
			matAnimation._23=pBoneMatrixs[12*i+9];
			matAnimation._33=pBoneMatrixs[12*i+10];
			matAnimation._43=pBoneMatrixs[12*i+11];

			Matrix4x4 matOrigin=matFrame0*matAnimation*m_pDynamicModel->getModelMatrix();
			AABBox aabox=m_pModelMF1->m_MeshAABBox;
			float length=(aabox.vcMax-aabox.vcMin).GetLength()/150.0f;
			m_pRenderDevice->GetVertexCacheManager()->RenderDetailSphere(Vector3D(matOrigin._41+boneOffset[0],\
				matOrigin._42+boneOffset[1],matOrigin._43+boneOffset[2]),length,5,5,Colour(255,255,255));
		}
	}
}

void ModelViewerRender::RenderDynamicNormals()
{
	if(m_pMF1FileResource==NULL) return ;
	
	uint32 nBoneCount=m_pModelMF1->m_iNumBones;
	SGPMF1Bone* pBones=m_pModelMF1->m_pBones;
	float* pBuffer=m_pDynamicModel->getBonesMatrix();
	uint32 i,j;
	
	Matrix4x4* pBoneMatrix=new Matrix4x4[nBoneCount];
	for(i=0;i<nBoneCount;++i)
	{
		pBoneMatrix[i].Identity();
		pBoneMatrix[i]._11=pBuffer[12*i];
		pBoneMatrix[i]._21=pBuffer[12*i+1];
		pBoneMatrix[i]._31=pBuffer[12*i+2];
		pBoneMatrix[i]._41=pBuffer[12*i+3];
		pBoneMatrix[i]._12=pBuffer[12*i+4];
		pBoneMatrix[i]._22=pBuffer[12*i+5];
		pBoneMatrix[i]._32=pBuffer[12*i+6];
		pBoneMatrix[i]._42=pBuffer[12*i+7];
		pBoneMatrix[i]._13=pBuffer[12*i+8];
		pBoneMatrix[i]._23=pBuffer[12*i+9];
		pBoneMatrix[i]._33=pBuffer[12*i+10];
		pBoneMatrix[i]._43=pBuffer[12*i+11];
		pBoneMatrix[i]=pBoneMatrix[i]*m_pDynamicModel->getModelMatrix();
	}
	
	uint32 meshCount=m_pModelMF1->m_Header.m_iNumMeshes;
	for(i=0;i<meshCount;++i)
	{
		uint32 vertexCount=m_pModelMF1->m_pLOD0Meshes[i].m_iNumVerts;
		SGPVertex_UPOS_VERTEXCOLOR *pVert=new SGPVertex_UPOS_VERTEXCOLOR[vertexCount*2];
		for(j=0;j<vertexCount;++j)
		{
			CalTranlsatedVertex(pVert[j*2],pBoneMatrix,i,j,TRUE);
			CalTranlsatedVertex(pVert[j*2+1],pBoneMatrix,i,j,FALSE);
		}
		m_pRenderDevice->GetVertexCacheManager()->RenderLines(2*vertexCount,pVert,false);
		delete [] pVert;
	}
	delete [] pBoneMatrix;
}

void ModelViewerRender::RenderStaticNormals()
{
	if(m_pModelMF1==NULL) return ;

	const float NORMAL_LENGTH=0.02f;

	uint32 meshCount=m_pModelMF1->m_Header.m_iNumMeshes;
	for(uint32 i=0;i<meshCount;++i)
	{
		SGPMF1Mesh &mesh=m_pModelMF1->m_pLOD0Meshes[i];
		uint32 vertexCount=m_pModelMF1->m_pLOD0Meshes[i].m_iNumVerts;
		SGPVertex_UPOS_VERTEXCOLOR* pVert=new SGPVertex_UPOS_VERTEXCOLOR[vertexCount*2];
		for(uint32 j=0;j<vertexCount;++j)
		{
			SGPMF1Vertex& currVert=mesh.m_pVertex[j];
			Vector4D startPos,endPos;
			startPos.Set(currVert.vPos[0],currVert.vPos[1],currVert.vPos[2]);
			startPos=startPos*m_pStaticModel->getModelMatrix();
			endPos.Set(currVert.vPos[0]+currVert.vNormal[0]*NORMAL_LENGTH,currVert.vPos[1]+\
				currVert.vNormal[1]*NORMAL_LENGTH,currVert.vPos[2]+currVert.vNormal[2]*NORMAL_LENGTH);
			endPos=endPos*m_pStaticModel->getModelMatrix();
			SetVertexPos(pVert[2*j],startPos.x,startPos.y,startPos.z);
			SetVertexColor(pVert[2*j],1.0f,1.0f,1.0f,1.0f);
			SetVertexPos(pVert[2*j+1],endPos.x,endPos.y,endPos.z);
			SetVertexColor(pVert[2*j+1],1.0f,1.0f,1.0f,1.0f);	
		}
		m_pRenderDevice->GetVertexCacheManager()->RenderLines(2*vertexCount,pVert,false);
		delete [] pVert;
	}
}

void ModelViewerRender::SetVertexPos(SGPVertex_UPOS_VERTEXCOLOR &vertex,float x,float y,float z)
{
	vertex.x=x;
	vertex.y=y;
	vertex.z=z;
}

void ModelViewerRender::SetVertexColor(SGPVertex_UPOS_VERTEXCOLOR &vertex,float r,float g,float b,float a)
{
	vertex.VertexColor[0]=r;
	vertex.VertexColor[1]=g;
	vertex.VertexColor[2]=b;
	vertex.VertexColor[3]=a;
}

void ModelViewerRender::CalTranlsatedVertex(SGPVertex_UPOS_VERTEXCOLOR &vertex,Matrix4x4* pBoneMatrix,int meshIndex,int vertexIndex,BOOL bOrigin)
{
	const float NORMAL_LENGTH=0.02f;

	SGPMF1Mesh& mesh=m_pModelMF1->m_pLOD0Meshes[meshIndex];
	SGPMF1BoneGroup& boneGroup=m_pModelMF1->m_pBoneGroup[mesh.m_pVertexBoneGroupID[vertexIndex]];
	SGPMF1Vertex& vert=mesh.m_pVertex[vertexIndex];
	Vector4D result,origin;
	if(bOrigin)
		origin.Set(vert.vPos[0],vert.vPos[1],vert.vPos[2]);
	else
		origin.Set(vert.vPos[0]+vert.vNormal[0]*NORMAL_LENGTH,vert.vPos[1]+vert.vNormal[1]*NORMAL_LENGTH,\
			vert.vPos[2]+vert.vNormal[2]*NORMAL_LENGTH);
	uint8 totalWeight=0;
	if(boneGroup.BoneWeight.x!=0)
	{
		totalWeight+=boneGroup.BoneWeight.x;
		result=(origin*pBoneMatrix[boneGroup.BoneIndex.x])*(float)boneGroup.BoneWeight.x;
	}
	if(boneGroup.BoneWeight.y!=0)
	{
		totalWeight+=boneGroup.BoneWeight.y;
		result=result+(origin*pBoneMatrix[boneGroup.BoneIndex.y])*(float)boneGroup.BoneWeight.y;
	}
	if(boneGroup.BoneWeight.z!=0)
	{
		totalWeight+=boneGroup.BoneWeight.z;
		result=result+(origin*pBoneMatrix[boneGroup.BoneIndex.z])*(float)boneGroup.BoneWeight.z;
	}
	if(boneGroup.BoneWeight.w!=0)
	{
		totalWeight+=boneGroup.BoneWeight.w;
		result=result+(origin*pBoneMatrix[boneGroup.BoneIndex.w])*(float)boneGroup.BoneWeight.w;
	}
	result*=1/(float)totalWeight;

	SetVertexPos(vertex,result.x,result.y,result.z);
	SetVertexColor(vertex,1.0f,1.0f,1.0f,1.0f);
}

void ModelViewerRender::RenderParticleEmitter()
{
	for(uint32 i=0;i<m_pModelMF1->m_Header.m_iNumParticles;++i)
	{
		Matrix4x4 matrix;
		uint32 nBoneCount=m_pModelMF1->m_iNumBones;
		uint32 boneID=m_pModelMF1->m_pParticleEmitter[i].m_iAttachBoneID;
	//	if(boneID>=nBoneCount) continue;
		if(m_pDynamicModel!=NULL)
		{
			float* pBuffer=m_pDynamicModel->getBonesMatrix();
			Matrix4x4 boneMatrix;
			boneMatrix.Identity();
			boneMatrix._11=pBuffer[12*boneID];
			boneMatrix._21=pBuffer[12*boneID+1];
			boneMatrix._31=pBuffer[12*boneID+2];
			boneMatrix._41=pBuffer[12*boneID+3];
			boneMatrix._12=pBuffer[12*boneID+4];
			boneMatrix._22=pBuffer[12*boneID+5];
			boneMatrix._32=pBuffer[12*boneID+6];
			boneMatrix._42=pBuffer[12*boneID+7];
			boneMatrix._13=pBuffer[12*boneID+8];
			boneMatrix._23=pBuffer[12*boneID+9];
			boneMatrix._33=pBuffer[12*boneID+10];
			boneMatrix._43=pBuffer[12*boneID+11];
			matrix=m_pModelMF1->m_pParticleEmitter[i].m_AbsoluteMatrix*boneMatrix*m_pDynamicModel->getModelMatrix();
		}
		if(m_pStaticModel!=NULL)
		{
			matrix=m_pModelMF1->m_pParticleEmitter[i].m_AbsoluteMatrix*m_pStaticModel->getModelMatrix();
		}
		if(m_pStaticModel!=NULL||m_pDynamicModel!=NULL)
		{
			AABBox aabox=m_pModelMF1->m_MeshAABBox;
			float length=(aabox.vcMax-aabox.vcMin).GetLength();
			length*=0.02f;

			if((int)(i+1)==ModelViewerConfig::GetInstance()->m_nCurrSelParticle)
				m_pRenderDevice->GetVertexCacheManager()->RenderCone(matrix,length,-length,4,Colour(255,0,0));
			else
				m_pRenderDevice->GetVertexCacheManager()->RenderCone(matrix,length,-length,4,Colour(255,255,255));
		}
	}
}

void ModelViewerRender::RenderAttachments()
{
	for(uint32 i=0;i<m_pModelMF1->m_Header.m_iNumAttc;++i)
	{
		Matrix4x4 matrix;
		uint32 nBoneCount=m_pModelMF1->m_iNumBones;
		uint32 boneID=m_pModelMF1->m_pAttachTags[i].m_iAttachBoneID;
		if(m_pDynamicModel!=NULL)
		{
			if(boneID>=nBoneCount) continue;
			float* pBuffer=m_pDynamicModel->getBonesMatrix();
			Matrix4x4 boneMatrix;
			boneMatrix.Identity();
			boneMatrix._11=pBuffer[12*boneID];
			boneMatrix._21=pBuffer[12*boneID+1];
			boneMatrix._31=pBuffer[12*boneID+2];
			boneMatrix._41=pBuffer[12*boneID+3];
			boneMatrix._12=pBuffer[12*boneID+4];
			boneMatrix._22=pBuffer[12*boneID+5];
			boneMatrix._32=pBuffer[12*boneID+6];
			boneMatrix._42=pBuffer[12*boneID+7];
			boneMatrix._13=pBuffer[12*boneID+8];
			boneMatrix._23=pBuffer[12*boneID+9];
			boneMatrix._33=pBuffer[12*boneID+10];
			boneMatrix._43=pBuffer[12*boneID+11];
			matrix=m_pModelMF1->m_pAttachTags[i].m_InitMatTag*boneMatrix*m_pDynamicModel->getModelMatrix();
		}
		if(m_pStaticModel!=NULL)
		{
			matrix=m_pModelMF1->m_pAttachTags[i].m_InitMatTag*m_pStaticModel->getModelMatrix();
		}
		if(m_pStaticModel!=NULL||m_pDynamicModel!=NULL)
		{
			AABBox aabox=m_pModelMF1->m_MeshAABBox;
			float length=(aabox.vcMax-aabox.vcMin).GetLength();
			length/=100.0f;
			AABBox box;
			box.vcCenter.x=matrix._41;
			box.vcCenter.y=matrix._42;
			box.vcCenter.z=matrix._43;
			box.vcMin.x=box.vcCenter.x-length;
			box.vcMin.y=box.vcCenter.y-length;
			box.vcMin.z=box.vcCenter.z-length;
			box.vcMax.x=box.vcCenter.x+length;
			box.vcMax.y=box.vcCenter.y+length;
			box.vcMax.z=box.vcCenter.z+length;
			m_pRenderDevice->GetVertexCacheManager()->RenderBox(box,Colour(0,255,0));
		//	m_pRenderDevice->GetVertexCacheManager()->RenderDetailSphere(Vector3D(matrix._41,matrix._42,matrix._43),length/100.0f,5,5,Colour(255,255,0));
		}
	}
}