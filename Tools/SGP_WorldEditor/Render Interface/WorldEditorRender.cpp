#include "..\stdafx.h"

#include "WorldEditorRender.h"
#include "..\LogViewer Pane\RichEditLogger.h"
#include "WorldEditorCamera.h"
#include "..\HelpFunction.h"
#include "..\WorldEditorConfig.h"
#include "..\MouseMsgInterface.h"
#include "..\WorldMapManager.h"

WorldEditorRender::WorldEditorRender()
{
	m_bInitilized=false;
	m_CurPosX=0;
	m_CurPosY=0;
	m_bIntersectTerrain = false;
}

WorldEditorRender::~WorldEditorRender()
{

}

bool WorldEditorRender::Init(HWND hwnd,CRichEditCtrl *pCtrl)
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
	m_pRenderDevice->setNearFarClipPlane(1.0f, 1000.0f);	
	m_pRenderDevice->setFov(45.0f);

	RECT rc;
	::GetWindowRect(hwnd,&rc);
	m_pRenderDevice->onResize(rc.right-rc.left,rc.bottom-rc.top);
	SViewPort viewarea;
	viewarea.Width = m_pRenderDevice->getScreenSize().Width;
	viewarea.Height = m_pRenderDevice->getScreenSize().Height;
	m_pRenderDevice->setViewPort(viewarea);
	m_WndWidth=rc.right-rc.left;
	m_WndHeight=rc.bottom-rc.top;

	//init camera
	m_pCamera=new WorldEditorCamera;
/*	m_pCamera->SetPos(10,256,10);
	m_pCamera->SetLookAt(10, 256, 10);
	m_pCamera->SetRotation(-float_Pi/5, 0, 0);*/
	m_pCamera->SetPos(0,20,-30);
	m_pCamera->SetLookAt(0,0,0);
	m_pCamera->SetRotation(-float_Pi/7, float_Pi/3, 0);

	// init Font// could not create selected font.
	if( !m_pRenderDevice->CreateFontInManager( "font1", String(L"Font\\YaHei_Consolas.ttf"), true, false, 16 ) )
	{
		delete m_pDevice;
		m_pDevice = NULL;
		Logger::setCurrentLogger(nullptr);
		return false;
	}

	m_pRenderDevice->GetParticleManager()->initParticleSystemEngine(m_pDevice->getTimer()->getRealTime());

	m_pRenderDevice->createRenderToFrameBuffer(rc.right-rc.left,rc.bottom-rc.top,false);

//	COpenGLConfig::getInstance()->FullScreenAntiAlias = 1;

	m_bInitilized=true;
	
	return true;
}

void WorldEditorRender::Release()
{
	m_pRenderDevice->deleteRenderToFrameBuffer();

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

void WorldEditorRender::Render()
{
	if(!m_bInitilized) return ;//haven't initialized

	// Work out a frame delta time.
	m_pDevice->getTimer()->tick();
	const double frameDeltaTime = m_pDevice->getTimer()->GetElapsedTime() / 1000.0; // Time in seconds
	m_pRenderDevice->setRenderDeviceTime(m_pDevice->getTimer()->getTime(), frameDeltaTime);
	
	// update internal Camera position
	m_pCamera->Update((float)frameDeltaTime);
	m_pRenderDevice->setViewMatrix3D(m_pCamera->GetRight(),m_pCamera->GetUp(),m_pCamera->GetDir(),m_pCamera->GetPos());

	// Update World
	m_pRenderDevice->GetWorldSystemManager()->updateWorld((float)frameDeltaTime);

	WorldEditorConfig* pConfig=WorldEditorConfig::GetInstance();
	if(pConfig->m_bShowWireframe) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	m_pRenderDevice->setRenderToFrameBuffer();

	m_pRenderDevice->setClearColor(0.5f,0.5f,0.5f,1.0f);
	m_pRenderDevice->beginScene(true,true,false);

	/************************ 3D Render ************************/
	if(WorldMapManager::GetInstance()->m_bHaveMap)
	{
		m_pRenderDevice->GetWorldSystemManager()->renderWorld();

		if(pConfig->m_bShowLights) RenderLights();
		if(pConfig->m_bShowChunk) RenderChunkLine();
		if(pConfig->m_bShowChunkGrid) RenderChunkGridLine();
		MouseMsgInterface::GetInstance()->DrawArea();
	}
	
	RenderCoordinateAxis();
	RenderGround();

	m_pRenderDevice->FlushRenderBatch();
	
	m_pRenderDevice->renderBackToMainBuffer();

	if(WorldMapManager::GetInstance()->m_bHaveMap)
		MouseMsgInterface::GetInstance()->DrawFlag();
	m_pRenderDevice->FlushEditorLinesRenderBatch(true,4.0f);

	/********************* 2D Render:Text **********************/
	m_pRenderDevice->BeginRenderText();
	RenderFPS(frameDeltaTime);
	RenderCoordinateLabel();
	m_pRenderDevice->EndRenderText();

	m_pRenderDevice->endScene();
}

void WorldEditorRender::ResizeWindow(int cx,int cy)
{
	if(m_bInitilized)
	{
		m_pRenderDevice->onResize(cx,cy);
		m_WndWidth=cx;
		m_WndHeight=cy;
		m_pRenderDevice->deleteRenderToFrameBuffer();
		m_pRenderDevice->createRenderToFrameBuffer(cx, cy, false);
	}
}

void WorldEditorRender::SetVertexPos(SGPVertex_UPOS_VERTEXCOLOR& vertex,float x,float y,float z)
{
	vertex.x=x;
	vertex.y=y;
	vertex.z=z;
}

void WorldEditorRender::SetVertexColor(SGPVertex_UPOS_VERTEXCOLOR& vertex,float r,float g,float b,float a)
{
	vertex.VertexColor[0]=r;
	vertex.VertexColor[1]=g;
	vertex.VertexColor[2]=b;
	vertex.VertexColor[3]=a;
}

void WorldEditorRender::RenderFPS(double deltaTime)
{
	static double averageMS=0;
	static DWORD dwStart=0;
	static double fps=0;
	averageMS += (deltaTime*1000.0 - averageMS) * 0.1;
	if(GetTickCount()-dwStart>=500)
	{
		fps=1000.0/averageMS;
		dwStart=GetTickCount();
	}
	m_pRenderDevice->DrawTextInPos(10,10,0/*SGPFDL_SHADOW*/,16,200,200,200,L"FPS = %d",(int)fps);
}

void WorldEditorRender::RenderLights()
{
	std::vector<ISGPLightObject*>& lightVector=WorldMapManager::GetInstance()->m_LightObjVector;
	for(uint32 i=0;i<lightVector.size();++i)
	{
		ISGPLightObject* pObj=lightVector[i];
		Colour col((int)(pObj->m_fDiffuseColor[0]*255),(int)(pObj->m_fDiffuseColor[1]*255),(int)(pObj->m_fDiffuseColor[2]*255));
		Vector3D center(pObj->m_fPosition[0],pObj->m_fPosition[1],pObj->m_fPosition[2]);
		m_pRenderDevice->GetVertexCacheManager()->RenderDetailSphere(center,pObj->m_fLightSize,5,5,col);
		m_pRenderDevice->GetVertexCacheManager()->RenderCircle(center,pObj->m_fRange,Vector3D(0.0f,1.0f,0.0f),col);
		m_pRenderDevice->GetVertexCacheManager()->RenderCircle(center,pObj->m_fRange,Vector3D(1.0f,0.0f,0.0f),col);
		m_pRenderDevice->GetVertexCacheManager()->RenderCircle(center,pObj->m_fRange,Vector3D(0.0f,0.0f,1.0f),col);
	}
}

bool WorldEditorRender::GetWorldIntersectPoint()
{
	if(WorldMapManager::GetInstance()->m_bHaveMap)
	{
		Vector4D vcOrig,vcDir;
		m_pRenderDevice->Transform2Dto3D(SDimension2D(m_CurPosX,m_CurPosY),&vcOrig,&vcDir);

		Ray mouseRay;
		mouseRay.m_vcOrig.x=vcOrig.x;
		mouseRay.m_vcOrig.y=vcOrig.y;
		mouseRay.m_vcOrig.z=vcOrig.z;
		mouseRay.m_vcDir.x=vcDir.x;
		mouseRay.m_vcDir.y=vcDir.y;
		mouseRay.m_vcDir.z=vcDir.z;

		bool bFinded=false;
		float minDistance=100000000000.0f;
		float distance;

		const CSGPTerrain* pTerrain=m_pRenderDevice->GetWorldSystemManager()->getTerrain();
		uint32 terrainCount=((CSGPTerrain*)pTerrain)->GetTerrainChunkSize();
		terrainCount*=terrainCount;
		for(uint32 i=0;i<terrainCount;++i)
		{
			CSGPTerrainChunk* pTerrainChunk=pTerrain->m_TerrainChunks[i];
			if(mouseRay.Intersects(pTerrainChunk->m_BoundingBox,&distance))
			{
				for(uint32 j=0;j<TERRAIN_CHUNK_INDEX_LOD0_COUNT;j+=3)
				{
					Vector3D v0,v1,v2;
					SGPTerrainVertex& vertex1=pTerrainChunk->m_ChunkTerrainVertex[base_index_tile[j+1]];
					SGPTerrainVertex& vertex2=pTerrainChunk->m_ChunkTerrainVertex[base_index_tile[j]];
					SGPTerrainVertex& vertex3=pTerrainChunk->m_ChunkTerrainVertex[base_index_tile[j+2]];
					v0.Set(vertex1.x,vertex1.y,vertex1.z);
					v1.Set(vertex2.x,vertex2.y,vertex2.z);
					v2.Set(vertex3.x,vertex3.y,vertex3.z);

					if(mouseRay.Intersects(v0,v1,v2,false,&distance))
					{
						if(distance<minDistance) minDistance=distance;
						bFinded=true;
					}
				}
			}
		}

		m_bIntersectTerrain = bFinded;
		if(bFinded) m_IntersectPoint=mouseRay.m_vcOrig+mouseRay.m_vcDir*minDistance;
	//	else m_IntersectPoint.Set(0.0f,0.0f,0.0f);
		return bFinded;
	}
	else
	{
		m_bIntersectTerrain = false;
	//	m_IntersectPoint.Set(0.0f,0.0f,0.0f);
		return true;
	}
	
}

void WorldEditorRender::RenderRoundIntersectArea(float radius,float r,float g,float b)
{
	const int POINT_COUNT=60;

	SGPVertex_UPOS_VERTEXCOLOR vertex[POINT_COUNT*2];

	float thetaUnit=float_2pi/POINT_COUNT;
	for(int i=0;i<POINT_COUNT;++i)
	{
		float currTheta=thetaUnit*i;
		float nextTheta=thetaUnit*(i+1);
		float currCosTheta=cos(currTheta);
		float currSinTheta=sin(currTheta);
		float nextCosTheta=cos(nextTheta);
		float nextSinTheta=sin(nextTheta);

		float x,y,z;
		x=m_IntersectPoint.x+radius*currCosTheta;
		z=m_IntersectPoint.z+radius*currSinTheta;
		y=m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2],x,y,z);
		SetVertexColor(vertex[i*2],r,g,b,1.0f);

		x=m_IntersectPoint.x+radius*nextCosTheta;
		z=m_IntersectPoint.z+radius*nextSinTheta;
		y=m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2+1],x,y,z);
		SetVertexColor(vertex[i*2+1],r,g,b,1.0f);
	}

	m_pRenderDevice->GetVertexCacheManager()->RenderLines(POINT_COUNT*2,vertex,false);
}

void WorldEditorRender::RenderSquareIntersectArea(float radius,float r,float g,float b)
{
	const int POINT_COUNT = 30;
	float minx,maxx,minz,maxz;
	int i;
	minx = m_IntersectPoint.x - radius;
	maxx = m_IntersectPoint.x + radius;
	minz = m_IntersectPoint.z - radius;
	maxz = m_IntersectPoint.z + radius;
	SGPVertex_UPOS_VERTEXCOLOR vertex[POINT_COUNT*2];
	float x,y,z;
	float deltaDistance = radius*2/POINT_COUNT;
	for(i=0;i<POINT_COUNT;++i)
	{
		x = minx;
		z = minz + i*deltaDistance;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2],x,y,z);
		SetVertexColor(vertex[i*2],r,g,b,1.0f);

		x = minx;
		z = minz + (i+1)*deltaDistance;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2+1],x,y,z);
		SetVertexColor(vertex[i*2+1],r,g,b,1.0f);
	}
	m_pRenderDevice->GetVertexCacheManager()->RenderLines(POINT_COUNT*2,vertex,false);

	for(i=0;i<POINT_COUNT;++i)
	{
		x = maxx;
		z = minz + i*deltaDistance;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2],x,y,z);
		SetVertexColor(vertex[i*2],r,g,b,1.0f);

		x = maxx;
		z = minz + (i+1)*deltaDistance;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2+1],x,y,z);
		SetVertexColor(vertex[i*2+1],r,g,b,1.0f);
	}
	m_pRenderDevice->GetVertexCacheManager()->RenderLines(POINT_COUNT*2,vertex,false);

	for(i=0;i<POINT_COUNT;++i)
	{
		x = minx + i*deltaDistance;
		z = minz;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2],x,y,z);
		SetVertexColor(vertex[i*2],r,g,b,1.0f);

		x = minx + (i+1)*deltaDistance;
		z = minz;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2+1],x,y,z);
		SetVertexColor(vertex[i*2+1],r,g,b,1.0f);
	}
	m_pRenderDevice->GetVertexCacheManager()->RenderLines(POINT_COUNT*2,vertex,false);

	for(i=0;i<POINT_COUNT;++i)
	{
		x = minx + i*deltaDistance;
		z = maxz;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2],x,y,z);
		SetVertexColor(vertex[i*2],r,g,b,1.0f);

		x = minx + (i+1)*deltaDistance;
		z = maxz;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2+1],x,y,z);
		SetVertexColor(vertex[i*2+1],r,g,b,1.0f);
	}
	m_pRenderDevice->GetVertexCacheManager()->RenderLines(POINT_COUNT*2,vertex,false);
}

void WorldEditorRender::RenderChunkGridLine()
{
	const CSGPTerrain* pTerrain=m_pRenderDevice->GetWorldSystemManager()->getTerrain();
	uint32 chunkCount = ((CSGPTerrain*)pTerrain)->GetTerrainChunkSize();
	chunkCount *= chunkCount;

	OwnedArray<COpenGLTerrainRenderer::OpenGLChunkRenderInfo>& array=((COpenGLRenderDevice*)m_pRenderDevice)->getOpenGLTerrainRenderer()->m_TerrainChunkRenderArray;
	SGPVertex_UPOS_VERTEXCOLOR vertex[TERRAIN_CHUNK_INDEX_LOD0_COUNT*2];
	uint32 i,j,k;
	static int index[6]={0,1,1,2,2,0};
	for(i=0;i<TERRAIN_CHUNK_INDEX_LOD0_COUNT*2;++i) SetVertexColor(vertex[i],1.0f,1.0f,0.0f,1.0f);

	for(i=0;i<chunkCount;++i)
	{
		CSGPTerrainChunk* pChunk=((CSGPTerrain*)pTerrain)->m_TerrainChunks[i];
		uint32 count=array[i]->nIndexCount;
		uint32 offset=array[i]->nIndexOffset;
		for(j=0;j<count;j+=3)
		{
			for(k=0;k<6;++k)
			{
				SGPTerrainVertex& terrainVertex=pChunk->m_ChunkTerrainVertex[chunk_index_tile[offset+j+index[k]]];
				SetVertexPos(vertex[2*j+k],terrainVertex.x,terrainVertex.y,terrainVertex.z);
			}
		}
		m_pRenderDevice->GetVertexCacheManager()->RenderLines(count*2,vertex,false);
	}
}

void WorldEditorRender::RenderChunkLine()
{
	const CSGPTerrain* pTerrain=m_pRenderDevice->GetWorldSystemManager()->getTerrain();
	uint32 chunkCount=((CSGPTerrain*)pTerrain)->GetTerrainChunkSize();
	chunkCount*=chunkCount;

	uint32 i,j;
	SGPVertex_UPOS_VERTEXCOLOR vertex[10];
	static int index[10]={0,8,8,80,80,72,72,0,8,72};
	for(i=0;i<10;++i) SetVertexColor(vertex[i],1.0f,1.0f,1.0f,1.0f);
	for(i=0;i<chunkCount;++i)
	{
		CSGPTerrainChunk* pChunk=((CSGPTerrain*)pTerrain)->m_TerrainChunks[i];
		for(j=0;j<10;++j)
		{
			SGPTerrainVertex& terrainVertex=pChunk->m_ChunkTerrainVertex[index[j]];
			SetVertexPos(vertex[j],terrainVertex.x,terrainVertex.y,terrainVertex.z);
		}
		m_pRenderDevice->GetVertexCacheManager()->RenderLines(10,vertex,false);
	}
}

uint16 WorldEditorRender::GetTerrainHeight(uint32 index_x,uint32 index_z)
{
	const CSGPTerrain* pTerrain=m_pRenderDevice->GetWorldSystemManager()->getTerrain();
	uint32 heightmap_index = index_z * (((CSGPTerrain*)pTerrain)->GetTerrainChunkSize()*SGPTT_TILENUM+1) + index_x;
	return ((CSGPTerrain*)pTerrain)->GetHeightMap()[heightmap_index];
}

void WorldEditorRender::RenderSelectedChunk(int chunkId,float r,float g,float b)
{
/*	const CSGPTerrain* pTerrain=m_pRenderDevice->GetWorldSystemManager()->getTerrain();

	CSGPTerrainChunk* pChunk=((CSGPTerrain*)pTerrain)->m_TerrainChunks[chunkId];
	m_pRenderDevice->GetVertexCacheManager()->RenderBox(pChunk->m_BoundingBox,Colour(r,g,b,1.0f));*/

	const CSGPTerrain* pTerrain=m_pRenderDevice->GetWorldSystemManager()->getTerrain();
	int chunkCount=((CSGPTerrain*)pTerrain)->GetTerrainChunkSize();
	int index_x = chunkId % chunkCount;
	int index_z = chunkId / chunkCount;

	const int POINT_COUNT = 8;
	float minx,maxx,minz,maxz;
	int i;
	minx = (float)index_x*SGPTT_TILENUM*SGPTT_TILE_METER;
	maxx = (float)(index_x+1)*SGPTT_TILENUM*SGPTT_TILE_METER;
	minz = (float)(chunkCount-1-index_z)*SGPTT_TILENUM*SGPTT_TILE_METER;
	maxz = (float)(chunkCount-index_z)*SGPTT_TILENUM*SGPTT_TILE_METER;
	SGPVertex_UPOS_VERTEXCOLOR vertex[POINT_COUNT*2];
	float x,y,z;
	float deltaDistance = (float)SGPTT_TILENUM*SGPTT_TILE_METER/POINT_COUNT;
	for(i=0;i<POINT_COUNT;++i)
	{
		x = minx;
		z = minz + i*deltaDistance;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2],x,y,z);
		SetVertexColor(vertex[i*2],r,g,b,1.0f);

		x = minx;
		z = minz + (i+1)*deltaDistance;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2+1],x,y,z);
		SetVertexColor(vertex[i*2+1],r,g,b,1.0f);
	}
	m_pRenderDevice->GetVertexCacheManager()->RenderLines(POINT_COUNT*2,vertex,false);

	for(i=0;i<POINT_COUNT;++i)
	{
		x = maxx;
		z = minz + i*deltaDistance;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2],x,y,z);
		SetVertexColor(vertex[i*2],r,g,b,1.0f);

		x = maxx;
		z = minz + (i+1)*deltaDistance;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2+1],x,y,z);
		SetVertexColor(vertex[i*2+1],r,g,b,1.0f);
	}
	m_pRenderDevice->GetVertexCacheManager()->RenderLines(POINT_COUNT*2,vertex,false);

	for(i=0;i<POINT_COUNT;++i)
	{
		x = minx + i*deltaDistance;
		z = minz;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2],x,y,z);
		SetVertexColor(vertex[i*2],r,g,b,1.0f);

		x = minx + (i+1)*deltaDistance;
		z = minz;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2+1],x,y,z);
		SetVertexColor(vertex[i*2+1],r,g,b,1.0f);
	}
	m_pRenderDevice->GetVertexCacheManager()->RenderLines(POINT_COUNT*2,vertex,false);

	for(i=0;i<POINT_COUNT;++i)
	{
		x = minx + i*deltaDistance;
		z = maxz;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2],x,y,z);
		SetVertexColor(vertex[i*2],r,g,b,1.0f);

		x = minx + (i+1)*deltaDistance;
		z = maxz;
		y = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(x,z);
		SetVertexPos(vertex[i*2+1],x,y,z);
		SetVertexColor(vertex[i*2+1],r,g,b,1.0f);
	}
	m_pRenderDevice->GetVertexCacheManager()->RenderLines(POINT_COUNT*2,vertex,false);
}

void WorldEditorRender::RenderSelectedChunk(int chunkId,float height,float r,float g,float b)
{
	SGPSkin skin;
	skin.bAlpha=false;
	skin.nShaderType=SGPST_VERTEXCOLOR;
	skin.nPrimitiveType=SGPPT_TRIANGLES;

	const CSGPTerrain* pTerrain=m_pRenderDevice->GetWorldSystemManager()->getTerrain();
	int chunkCount=((CSGPTerrain*)pTerrain)->GetTerrainChunkSize();
	SGPVertex_UPOS_VERTEXCOLOR vertex[4];
	int x = chunkId % chunkCount;
	int z = chunkId / chunkCount;
	vertex[0].x = (float)(x * SGPTT_TILENUM * SGPTT_TILE_METER);
	vertex[0].y = height;
	vertex[0].z = (float)(chunkCount - 1 - z) * SGPTT_TILENUM * SGPTT_TILE_METER;
	SetVertexColor(vertex[0],r,g,b,1.0f);

	vertex[1].x = (float)((x+1) * SGPTT_TILENUM * SGPTT_TILE_METER);
	vertex[1].y = height;
	vertex[1].z = (float)((chunkCount -1-z)*SGPTT_TILENUM * SGPTT_TILE_METER);
	SetVertexColor(vertex[1],r,g,b,1.0f);

	vertex[2].x = (float)((x+1) * SGPTT_TILENUM * SGPTT_TILE_METER);
	vertex[2].y = height;
	vertex[2].z = (float)((chunkCount - z)*SGPTT_TILENUM * SGPTT_TILE_METER);
	SetVertexColor(vertex[2],r,g,b,1.0f);

	vertex[3].x = (float)(x * SGPTT_TILENUM * SGPTT_TILE_METER);
	vertex[3].y = height;
	vertex[3].z = (float)((chunkCount - z)*SGPTT_TILENUM * SGPTT_TILE_METER);
	SetVertexColor(vertex[3],r,g,b,1.0f);

	uint16 index[] = {0,1,2,0,2,3};

	m_pRenderDevice->GetVertexCacheManager()->RenderDynamicBuffer(SGPVT_UPOS_VERTEXCOLOR,skin,4,6,vertex,index);
}

void WorldEditorRender::RenderSelectedCell(int cellId,int r,int g,int b)
{
	const CSGPTerrain* pTerrain=m_pRenderDevice->GetWorldSystemManager()->getTerrain();
	int chunkCount=((CSGPTerrain*)pTerrain)->GetTerrainChunkSize();
	int x=cellId%(chunkCount*SGPTT_TILENUM+1);
	int z=cellId/(chunkCount*SGPTT_TILENUM+1);

	Vector3D vertex[4];
	vertex[0].x=(float)x*SGPTT_TILE_METER;
	vertex[0].z=(float)(chunkCount*SGPTT_TILENUM-1-z)*SGPTT_TILE_METER;
	vertex[0].y=m_pRenderDevice->GetWorldSystemManager()->getTerrainHeight(vertex[0].x,vertex[0].z);

	vertex[1].x=(float)(x+1)*SGPTT_TILE_METER;
	vertex[1].z=(float)(chunkCount*SGPTT_TILENUM-1-z)*SGPTT_TILE_METER;
	vertex[1].y=m_pRenderDevice->GetWorldSystemManager()->getTerrainHeight(vertex[1].x,vertex[1].z);

	vertex[2].x=(float)(x+1)*SGPTT_TILE_METER;
	vertex[2].z=(float)(chunkCount*SGPTT_TILENUM-z)*SGPTT_TILE_METER;
	vertex[2].y=m_pRenderDevice->GetWorldSystemManager()->getTerrainHeight(vertex[2].x,vertex[2].z);

	vertex[3].x=(float)x*SGPTT_TILE_METER;
	vertex[3].z=(float)(chunkCount*SGPTT_TILENUM-z)*SGPTT_TILE_METER;
	vertex[3].y=m_pRenderDevice->GetWorldSystemManager()->getTerrainHeight(vertex[3].x,vertex[3].z);

	AABBox aabBox;
	aabBox+=vertex[0];
	aabBox+=vertex[1];
	aabBox+=vertex[2];
	aabBox+=vertex[3];
	m_pRenderDevice->GetVertexCacheManager()->RenderBox(aabBox,Colour(r,g,b,1.0f));
}

void WorldEditorRender::RenderCoordinateAxis()
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
	const float LINE_LENGTH = 0.18f;

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

void WorldEditorRender::RenderCoordinateLabel()
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

void WorldEditorRender::RenderGround()
{
	SGPVertex_UPOS_VERTEXCOLOR vertex[6];
	// x axis
	SetVertexPos(vertex[0],0.0f,0.0f,0.0f);
	SetVertexColor(vertex[0],1.0f,0.0f,0.0f,1.0f);
	SetVertexPos(vertex[1],7.0f,0.0f,0.0f);
	SetVertexColor(vertex[1],1.0f,0.0f,0.0f,1.0f);
	// y axis
	SetVertexPos(vertex[2],0.0f,0.0f,0.0f);
	SetVertexColor(vertex[2],0.0f,1.0f,0.0f,1.0f);
	SetVertexPos(vertex[3],0.0f,7.0f,0.0f);
	SetVertexColor(vertex[3],0.0f,1.0f,0.0f,1.0f);
	// z axis
	SetVertexPos(vertex[4],0.0f,0.0f,0.0f);
	SetVertexColor(vertex[4],0.0f,0.0f,1.0f,1.0f);
	SetVertexPos(vertex[5],0.0f,0.0f,7.0f);
	SetVertexColor(vertex[5],0.0f,0.0f,1.0f,1.0f);
	m_pRenderDevice->GetVertexCacheManager()->RenderLines(6,vertex,false);
}

void WorldEditorRender::RenderBoxSelectArea(int left,int top,int right,int bottom,float r,float g,float b)
{
	Matrix4x4 matView, matProj, matInvViewProj;
	m_pRenderDevice->getViewMatrix(matView);
	m_pRenderDevice->getProjMatrix(matProj);
	matInvViewProj.InverseOf( matView*matProj );

	Vector4D pos[4];
	pos[0].Set(2.0f*left/m_WndWidth-1,1-2.0f*top/m_WndHeight,0.9f);
	pos[1].Set(2.0f*right/m_WndWidth-1,1-2.0f*top/m_WndHeight,0.9f);
	pos[2].Set(2.0f*right/m_WndWidth-1,1-2.0f*bottom/m_WndHeight,0.9f);
	pos[3].Set(2.0f*left/m_WndWidth-1,1-2.0f*bottom/m_WndHeight,0.9f);

	for(int i=0;i<4;++i)
	{
		pos[i]=pos[i]*matInvViewProj;
		pos[i].x=pos[i].x/pos[i].w;
		pos[i].y=pos[i].y/pos[i].w;
		pos[i].z=pos[i].z/pos[i].w;
	}

	SGPVertex_UPOS_VERTEXCOLOR vertex[8];
	SetVertexPos(vertex[0],pos[0].x,pos[0].y,pos[0].z);
	SetVertexColor(vertex[0],r,g,b,1.0f);
	SetVertexPos(vertex[1],pos[1].x,pos[1].y,pos[1].z);
	SetVertexColor(vertex[1],r,g,b,1.0f);
	SetVertexPos(vertex[2],pos[1].x,pos[1].y,pos[1].z);
	SetVertexColor(vertex[2],r,g,b,1.0f);
	SetVertexPos(vertex[3],pos[2].x,pos[2].y,pos[2].z);
	SetVertexColor(vertex[3],r,g,b,1.0f);
	SetVertexPos(vertex[4],pos[2].x,pos[2].y,pos[2].z);
	SetVertexColor(vertex[4],r,g,b,1.0f);
	SetVertexPos(vertex[5],pos[3].x,pos[3].y,pos[3].z);
	SetVertexColor(vertex[5],r,g,b,1.0f);
	SetVertexPos(vertex[6],pos[3].x,pos[3].y,pos[3].z);
	SetVertexColor(vertex[6],r,g,b,1.0f);
	SetVertexPos(vertex[7],pos[0].x,pos[0].y,pos[0].z);
	SetVertexColor(vertex[7],r,g,b,1.0f);

	m_pRenderDevice->GetVertexCacheManager()->RenderLines(8,vertex,false);
}