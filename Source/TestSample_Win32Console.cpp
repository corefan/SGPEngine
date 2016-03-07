#include <windows.h>

#include "../SGPLibraryCode/SGPHeader.h"

#include "TestSample_Camera.h"

#ifdef _DEBUG
//#include "vld.h"
#endif

#ifndef SGP_WINDOWS
#error Windows only example
#else

// Hide Console windows
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"") 

using namespace sgp;


CTestCamera g_pTestCamera;

uint32 g_iEffectInstanceID = 0xFFFFFFFF;
//SGPVertex_UPOS_VERTEXCOLOR GuidVertex[(16*8+1)*2*2];

class MyEventReceiver : public ISGPEventReceiver
{
public:
 
	// This is the one method that we have to implement
	virtual bool OnEvent(const SSGPEvent& event)
	{
		// Remember whether each key is down
		if (event.EventType == SGPET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		// Key Up
		if (event.EventType == SGPET_KEY_INPUT_EVENT &&
			event.KeyInput.PressedDown == false)
		{
			
		}

		// Mouse Event
		if (event.EventType == SGPET_MOUSE_INPUT_EVENT)
		{
			// ZOOM
			if( event.MouseInput.Event == SGPMIE_MOUSE_WHEEL )
				g_pTestCamera.SetZoom(event.MouseInput.Wheel * 1.0f);

			if( event.MouseInput.Event == SGPMIE_RMOUSE_PRESSED_DOWN )
				bRButtonDown = true;
			if( event.MouseInput.Event == SGPMIE_RMOUSE_LEFT_UP )
				bRButtonDown = false;

			if( event.MouseInput.Event == SGPMIE_MMOUSE_PRESSED_DOWN )
				bMButtonDown = true;
			if( event.MouseInput.Event == SGPMIE_MMOUSE_LEFT_UP )
				bMButtonDown = false;

			LastXPos = event.MouseInput.X;
			LastYPos = event.MouseInput.Y;
		}

		return false;
	}


	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(SGP_KEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	
	MyEventReceiver() : LastXPos(0), LastYPos(0), bMButtonDown(false), bRButtonDown(false)
	{
		for (uint32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

	uint32 getXPosition() { return LastXPos; }
	uint32 getYPosition() { return LastYPos; }

	bool bMButtonDown;
	bool bRButtonDown;

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];

	uint32 LastXPos;
	uint32 LastYPos;
};



void RenderCoordinateAxis(ISGPRenderDevice* renderdevice,
	SGPVertex_UPOS_VERTEXCOLOR (&AxisVertex) [6])
{
	Matrix4x4 matView, matProj, matInvViewProj;
	Vector4D OrigTo;

	// proj position
	Vector4D vAxisOrig( -0.85f, -0.85f, 0.1f);
	renderdevice->getViewMatrix(matView);
	renderdevice->getProjMatrix(matProj);

	// Inverse view*proj to get "proj->world" transform
	matInvViewProj.InverseOf( matView*matProj );


	// move the obj to leftbottom of camera then transfer to world coordinate
	Vector4D Orig = vAxisOrig * matInvViewProj;
	Orig.x /= Orig.w;
	Orig.y /= Orig.w;
	Orig.z /= Orig.w;
	Orig.w = 1;

	// draw the lines stand for coordinate
	const float LINE_LENGTH = 0.2f;

	// RED X
	OrigTo = Orig + Vector4D(LINE_LENGTH,0,0);
	AxisVertex[0].x = Orig.x;
	AxisVertex[0].y = Orig.y;
	AxisVertex[0].z = Orig.z;
	AxisVertex[0].VertexColor[0] = 1.0f;
	AxisVertex[0].VertexColor[1] = 0.0f;
	AxisVertex[0].VertexColor[2] = 0.0f;
	AxisVertex[0].VertexColor[3] = 1.0f;

	AxisVertex[1].x = OrigTo.x;
	AxisVertex[1].y = OrigTo.y;
	AxisVertex[1].z = OrigTo.z;
	AxisVertex[1].VertexColor[0] = 1.0f;
	AxisVertex[1].VertexColor[1] = 0.0f;
	AxisVertex[1].VertexColor[2] = 0.0f;
	AxisVertex[1].VertexColor[3] = 1.0f;


	// GREEN Y
	OrigTo = Orig + Vector4D(0,LINE_LENGTH,0);
	AxisVertex[2].x = Orig.x;
	AxisVertex[2].y = Orig.y;
	AxisVertex[2].z = Orig.z;
	AxisVertex[2].VertexColor[0] = 0.0f;
	AxisVertex[2].VertexColor[1] = 1.0f;
	AxisVertex[2].VertexColor[2] = 0.0f;
	AxisVertex[2].VertexColor[3] = 1.0f;

	AxisVertex[3].x = OrigTo.x;
	AxisVertex[3].y = OrigTo.y;
	AxisVertex[3].z = OrigTo.z;
	AxisVertex[3].VertexColor[0] = 0.0f;
	AxisVertex[3].VertexColor[1] = 1.0f;
	AxisVertex[3].VertexColor[2] = 0.0f;
	AxisVertex[3].VertexColor[3] = 1.0f;


	// BLUE Z
	OrigTo = Orig + Vector4D(0,0,LINE_LENGTH);
	AxisVertex[4].x = Orig.x;
	AxisVertex[4].y = Orig.y;
	AxisVertex[4].z = Orig.z;
	AxisVertex[4].VertexColor[0] = 0.0f;
	AxisVertex[4].VertexColor[1] = 0.0f;
	AxisVertex[4].VertexColor[2] = 1.0f;
	AxisVertex[4].VertexColor[3] = 1.0f;

	AxisVertex[5].x = OrigTo.x;
	AxisVertex[5].y = OrigTo.y;
	AxisVertex[5].z = OrigTo.z;
	AxisVertex[5].VertexColor[0] = 0.0f;
	AxisVertex[5].VertexColor[1] = 0.0f;
	AxisVertex[5].VertexColor[2] = 1.0f;
	AxisVertex[5].VertexColor[3] = 1.0f;


	renderdevice->GetVertexCacheManager()->RenderLines(	6, AxisVertex, false );
}


void RenderLableXYZ(ISGPRenderDevice* renderdevice,
	float LableX_x, float LableX_y, float LableX_z,
	float LableY_x, float LableY_y, float LableY_z,
	float LableZ_x, float LableZ_y, float LableZ_z)
{
	Matrix4x4 matView, matProj;

	renderdevice->getViewMatrix(matView);
	renderdevice->getProjMatrix(matProj);

	//draw x, y, z label
	Matrix4x4 matWorldToScreen;
	matWorldToScreen =  matView * matProj;
	Vector4D Lable_x(LableX_x, LableX_y, LableX_z);
	Lable_x = Lable_x * matWorldToScreen;
	Lable_x.x /= Lable_x.w;
	Lable_x.y /= Lable_x.w;
	renderdevice->DrawTextInPos( int((Lable_x.x+1)*0.5f*renderdevice->getViewPort().Width), int(renderdevice->getViewPort().Height-(Lable_x.y+1)*0.5f*renderdevice->getViewPort().Height-16), 0, 16, 255, 0, 0, L"x" );
	Vector4D Lable_y(LableY_x, LableY_y, LableY_z);
	Lable_y = Lable_y * matWorldToScreen;
	Lable_y.x /= Lable_y.w;
	Lable_y.y /= Lable_y.w;
	renderdevice->DrawTextInPos( int((Lable_y.x+1)*0.5f*renderdevice->getViewPort().Width), int(renderdevice->getViewPort().Height-(Lable_y.y+1)*0.5f*renderdevice->getViewPort().Height-16), 0, 16, 0, 255, 0, L"y" );
	Vector4D Lable_z(LableZ_x, LableZ_y, LableZ_z);
	Lable_z = Lable_z * matWorldToScreen;
	Lable_z.x /= Lable_z.w;
	Lable_z.y /= Lable_z.w;
	renderdevice->DrawTextInPos( int((Lable_z.x+1)*0.5f*renderdevice->getViewPort().Width), int(renderdevice->getViewPort().Height-(Lable_z.y+1)*0.5f*renderdevice->getViewPort().Height-16), 0, 16, 0, 0, 255, L"z" );

}






/*
   Now ask for the driver and create the Windows specific window.
*/
int main()
{
/*
	{
		int vertIndex = 0;
		for( int Row=0; Row<=16*8; Row++ )
		{
			GuidVertex[vertIndex].x = 0;
			GuidVertex[vertIndex].y = 0;
			GuidVertex[vertIndex].z = Row * 2.0f;
		
			GuidVertex[vertIndex].VertexColor[0] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[1] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[2] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[3] = 1.0f;
			vertIndex++;

			GuidVertex[vertIndex].x = 2*8*16;
			GuidVertex[vertIndex].y = 0;
			GuidVertex[vertIndex].z = Row * 2.0f;
		
			GuidVertex[vertIndex].VertexColor[0] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[1] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[2] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[3] = 1.0f;
			vertIndex++;
		}

		for( int Col=0; Col<=16*8; Col++ )
		{
			GuidVertex[vertIndex].x = Col * 2.0f;
			GuidVertex[vertIndex].y = 0;
			GuidVertex[vertIndex].z = 0;
		
			GuidVertex[vertIndex].VertexColor[0] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[1] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[2] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[3] = 1.0f;
			vertIndex++;

			GuidVertex[vertIndex].x = Col * 2.0f;
			GuidVertex[vertIndex].y = 0;
			GuidVertex[vertIndex].z = 2*8*16;
		
			GuidVertex[vertIndex].VertexColor[0] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[1] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[2] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[3] = 1.0f;
			vertIndex++;
		}
	}
*/



	// create log
	ConsoleLogger logger( String("Hello World") );
	logger.setLogLevel( ELL_DEBUG );
	Logger::setCurrentLogger( &logger );

	// create device
	MyEventReceiver receiver;

	SGPDevice* device = createDevice(
		SGPDT_OPENGL,
		1024, 768, 32, 
		false, false, false,
		true,
		&logger,
		&receiver);

	// could not create selected driver.
	if (device == NULL)
	{
		Logger::setCurrentLogger(nullptr);
		return 1;
	}

	device->setResizable(true);

	ISGPRenderDevice* renderdevice = device->getRenderDevice();
	Random* globalrandom = device->getRandomizer();

	// could not get render driver or globalrandom.
	if (!renderdevice || !globalrandom)
	{
		Logger::setCurrentLogger(nullptr);
		return 1;
	}

	// Setting Working Dir
	String WorkingDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getParentDirectory().getParentDirectory().getFullPathName();
	WorkingDir = WorkingDir + File::separatorString + String("Data");
	renderdevice->setWorkingDirection(WorkingDir);


	// Camera
	renderdevice->setCameraMode(SGPCT_PERSPECTIVE);
	renderdevice->setNearFarClipPlane(1.0f, 1000.0f);	
	renderdevice->setFov(45.0f);

	SViewPort viewarea;
	viewarea.Width = renderdevice->getScreenSize().Width;
	viewarea.Height = renderdevice->getScreenSize().Height;
	renderdevice->setViewPort(viewarea);

	g_pTestCamera.SetPos(10, 256, 10);
	g_pTestCamera.SetRotation(-float_Pi/5, 0, 0);


	// init Font// could not create selected font.
	if( !renderdevice->CreateFontInManager( "font1", String(L"Font\\YaHei_Consolas.ttf"), true, false, 16 ) )
	{
		delete device;
		device = NULL;
		Logger::setCurrentLogger(nullptr);
		return 1;
	}

	renderdevice->PreCacheChar( String("FPSxyz") );

	uint32 testDDSID7 = renderdevice->GetTextureManager()->registerTexture(String(L"Texture\\moss.tga"), true);

	// Init Guid
	SGPVertex_UPOS_VERTEXCOLOR AxisVertex[6];



	renderdevice->GetParticleManager()->initParticleSystemEngine(device->getTimer()->getRealTime());

	CSkeletonMeshInstance *ptestModel = new CSkeletonMeshInstance(renderdevice);
	ptestModel->changeModel( String(L"SkeletonMesh\\magician_male_10000.mf1") );
	ptestModel->setScale(0.02f);
	ptestModel->setRotationY(90.0f * pi_over_180);
	ptestModel->playAnim(1.0f, 10, 58, true, true);				// idle
	ptestModel->playUpperBodyAnim(1.0f, 75, 99, true, true);	// run
	ptestModel->setEnableUpperBodyAnim(true);	
	//ptestModel->addAttachment(SGPATTDEF_LEFTHAND, String(L"Avatar\\hammer.mf1"));

#if 1
	renderdevice->GetWorldSystemManager()->createWorldSun();	
	renderdevice->GetWorldSystemManager()->createSkydome( String(L"skydome\\skydome.mf1") );
	renderdevice->GetWorldSystemManager()->setSkydomeTexture( String(L"skydome\\cloud001.dds"), String(L"skydome\\perturb001.dds") );
	renderdevice->GetWorldSystemManager()->setWorldSunPosition(38.0f);

	CSGPWorldMap* pWorldMap = new CSGPWorldMap();
	renderdevice->GetWorldSystemManager()->createNewWorld(pWorldMap, "newworld", SGPTS_SMALL, false, 70, String(L"Texture\\terrain\\dirt001.dds") );
#endif

	//renderdevice->GetWorldSystemManager()->loadWorldFromFile(WorkingDir, String(L"worldmap\\newworld.map"));

#if 1
	// addWaterChunk must be called before creating Quadtree
	renderdevice->GetWorldSystemManager()->createWater(40.0f, String(L"Texture\\water\\wavesbump.dds"));
	renderdevice->GetWorldSystemManager()->addWaterChunk(192);
	renderdevice->GetWorldSystemManager()->addWaterChunk(193);
	renderdevice->GetWorldSystemManager()->addWaterChunk(194);
	renderdevice->GetWorldSystemManager()->addWaterChunk(195);
	renderdevice->GetWorldSystemManager()->addWaterChunk(208);
	renderdevice->GetWorldSystemManager()->addWaterChunk(209);
	renderdevice->GetWorldSystemManager()->addWaterChunk(210);
	renderdevice->GetWorldSystemManager()->addWaterChunk(211);
	renderdevice->GetWorldSystemManager()->addWaterChunk(224);
	renderdevice->GetWorldSystemManager()->addWaterChunk(225);
	renderdevice->GetWorldSystemManager()->addWaterChunk(226);
	renderdevice->GetWorldSystemManager()->addWaterChunk(227);
	renderdevice->GetWorldSystemManager()->addWaterChunk(240);
	renderdevice->GetWorldSystemManager()->addWaterChunk(241);
	renderdevice->GetWorldSystemManager()->addWaterChunk(242);
	renderdevice->GetWorldSystemManager()->addWaterChunk(243);

	renderdevice->GetWorldSystemManager()->createGrass( String(L"Texture\\grasses.dds"), 2, 2 );
	Vector3D terrainNorm;
	SGPGrassCluster tempGrassCluster;
	for( int i=0; i<10; i++ )
	{
		for( int j=0; j<10; j++ )
		{
			tempGrassCluster.fPositionX = 10.0f + globalrandom->nextInt(256) / 25.0f;
			tempGrassCluster.fPositionZ = 10.0f + globalrandom->nextInt(256) / 25.0f;
			tempGrassCluster.fPositionY = renderdevice->GetWorldSystemManager()->getRealTerrainHeight(tempGrassCluster.fPositionX, tempGrassCluster.fPositionZ);
			terrainNorm = renderdevice->GetWorldSystemManager()->getTerrainNormal(tempGrassCluster.fPositionX, tempGrassCluster.fPositionZ);
			tempGrassCluster.nPackedNormal = (uint32((terrainNorm.x * 0.5f + 0.5f) * 255) << 24) +
											 (uint32((terrainNorm.y * 0.5f + 0.5f) * 255) << 16) +
											 (uint32((terrainNorm.z * 0.5f + 0.5f) * 255) << 8);			
			tempGrassCluster.nData = 0xFF000000 + (globalrandom->nextInt(256) << 8) + globalrandom->nextInt(256);
			renderdevice->GetWorldSystemManager()->setGrassCluster(tempGrassCluster.fPositionX, tempGrassCluster.fPositionZ, tempGrassCluster);
		}
	}			
	
	CSGPWorldConfig::getInstance()->m_fGrassFarFadingStart = 32.0f;
	CSGPWorldConfig::getInstance()->m_fGrassFarFadingEnd = 80.0f;
#endif

	renderdevice->GetWorldSystemManager()->initializeQuadTree();

#if 1
	ISGPObject* pObj0 = renderdevice->GetWorldSystemManager()->createObject(
		String(L"StaticMesh\\jianzhu.mf1"),
		String(L"SceneObject01"),
		Vector3D(45.0f, 30.0f, 28.0f) );
	pObj0->m_fScale = 1.5f;
	pObj0->m_fRotationXYZ[1] = 45.0f * pi_over_180;
	
	renderdevice->GetWorldSystemManager()->addSceneObject(pObj0);

#endif

	ptestModel->setPosition(13.55f, renderdevice->GetWorldSystemManager()->getRealTerrainHeight(13.55f,3.55f), 3.55f);
	//ptestModel->setScale(5.0f);
	ptestModel->setVisible(true);

	// Create other render to frame buffer	
	CSGPWorldConfig::getInstance()->m_bPostFog = true;

	renderdevice->createRenderToFrameBuffer( viewarea.Width, viewarea.Height, false );


	// In order to do framerate independent movement, we have to know
	// how long it was since the last frame
	device->setWindowCaption(L"SGP Engine - Test Sample Demo");

	uint32 LastXPos = receiver.getXPosition();
	uint32 LastYPos = receiver.getYPosition();
	double averageMS = 0;
	double averageFPS = 0;
	while(device->run() && renderdevice)
	{
		if(receiver.IsKeyDown(KEY_F1))
		{
			static int mmmm = 0;
			if( mmmm == 0)
			{
				String WorldMapName = String(L"worldmap\\") + renderdevice->GetWorldSystemManager()->getWorldName() + String(L".map");
				renderdevice->GetWorldSystemManager()->saveWorldToFile(WorkingDir, WorldMapName);
				mmmm++;
			}
		}
		if(receiver.IsKeyDown(KEY_F3))
		{
			static int gg = 0;

			if( gg == 0 )
			{
				renderdevice->GetWorldSystemManager()->setHeightMap(8, 119, 80);
				renderdevice->GetWorldSystemManager()->setHeightMap(8, 120, 80);
				uint32 chunks[4] = {224, 225, 240, 241};
				renderdevice->GetWorldSystemManager()->flushTerrainHeight(chunks, 4);


				renderdevice->GetWorldSystemManager()->setTerrainChunkLayerTexture(240, eChunk_Diffuse1Texture, String(L"Texture\\terrain\\dirt004.dds"));
				renderdevice->GetWorldSystemManager()->setTerrainChunkLayerTexture(240, eChunk_Diffuse2Texture, String(L"Texture\\terrain\\dirt002.dds"));
				renderdevice->GetWorldSystemManager()->setTerrainChunkLayerTexture(240, eChunk_Diffuse3Texture, String(L"Texture\\terrain\\stone001.dds"));

				renderdevice->GetWorldSystemManager()->setTerrainChunkLayerTexture(240, eChunk_NormalMapTexture, String(L"Texture\\terrain\\normal001.dds"));
				renderdevice->GetWorldSystemManager()->setTerrainChunkLayerTexture(240, eChunk_DetailMapTexture, String(L"Texture\\terrain\\detailmap001.dds"));
				renderdevice->GetWorldSystemManager()->setTerrainChunkLayerTexture(240, eChunk_SlopeMapTexture, String(L"Texture\\terrain\\slope.dds"));
				gg++;
			}

			uint32* pRawAlphaData = renderdevice->GetWorldSystemManager()->getWorldMap()->m_WorldChunkAlphaTextureData;
			for( int i=0; i<SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION; i++ )
				for( int j=15*SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION; j<16*SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION; j++ )
					pRawAlphaData[j*16*SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION+i] = 0xFF800080;
			renderdevice->GetWorldSystemManager()->editTerrainChunkBlendTexture(0, 15*SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION, SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION, SGPTT_TILENUM * SGPTBD_BLENDTEXTURE_DIMISION, pRawAlphaData);
			
		}
		if(receiver.IsKeyDown(KEY_F4))
		{
			static int hhhh = 0;
			if( hhhh == 0)
			{
				uint32 chunks[4] = {224, 225, 240, 241};
				renderdevice->GetWorldSystemManager()->flushTerrainNormal(chunks, 4);
				renderdevice->GetWorldSystemManager()->flushTerrainChunkColorMinimapTexture(chunks, 1);
				renderdevice->GetWorldSystemManager()->initializeQuadTree();
				hhhh++;
			}
		}
		if(receiver.IsKeyDown(KEY_F5))
		{
			static int aaaa = 0;
			if( aaaa == 0)
			{

				ISGPLightObject *pLight02 = 
					renderdevice->GetWorldSystemManager()->createLightObject(
						String(L"LightGreen"),
						SGPLT_Point,
						Vector3D(45.0f, 38.0f, 21.0f), Vector3D(0.0f, -1.0f, 0.0f),
						0.3f,
						30.0f,
						Colour(0, 255, 0) );


				renderdevice->GetWorldSystemManager()->addLightObject(pLight02);

				renderdevice->GetWorldSystemManager()->initializeCollisionSet();

				float fProgress = 0;
				renderdevice->GetWorldSystemManager()->updateSceneObjectLightmapTexture(fProgress, pObj0, 512, 512, globalrandom);
				//renderdevice->GetWorldSystemManager()->updateTerrainLightmapTexture(fProgress, globalrandom);
				

				delete pLight02;
				pLight02 = NULL;

				aaaa++;
			}
		}
		if(receiver.IsKeyDown(KEY_F6))
		{
			pObj0->m_fPosition[0] += 0.001f;
			pObj0->m_fPosition[1] += 0.001f;
			CStaticMeshInstance* pInst = renderdevice->GetWorldSystemManager()->getMeshInstanceBySceneID(pObj0->getSceneObjectID());
			pInst->setPosition(pObj0->m_fPosition[0], pObj0->m_fPosition[1], pObj0->m_fPosition[2]);
			renderdevice->GetWorldSystemManager()->flushSceneObject(pObj0, 1, false);
			renderdevice->GetWorldSystemManager()->initializeQuadTree();
		}
		if(receiver.IsKeyDown(KEY_F7))
		{
			static int llll = 0;
			if( llll == 0)
			{
				ptestModel->setEnableUpperBodyAnim(false);
				ptestModel->playAnimCrossFade(0.1f, 105, 131, true, 3.0f);	// attack
				
				llll++;
			}
		}
		if(receiver.IsKeyDown(KEY_KEY_1))
		{
			static float pp = 0;
			if( g_iEffectInstanceID == 0xFFFFFFFF )
			{
				g_iEffectInstanceID = renderdevice->GetEffectInstanceManager()->createEffectInstance(String(L"effect\\hammer.mf1"));
				CEffectInstance* pEffectInst = renderdevice->GetEffectInstanceManager()->getEffectInstanceByID(g_iEffectInstanceID);
				int32 BoneID = ptestModel->getBoneIDByName("Bip01 Spine1");
				Vector3D BonePos = ptestModel->getBoneWorldPositionByID(BoneID);
				pEffectInst->setPosition( BonePos.x, BonePos.y, BonePos.z );
				pEffectInst->playAnim(1.0f, true);
				pEffectInst->setScale(0.2f);
			}
			else
			{
				CEffectInstance* pEffectInst = renderdevice->GetEffectInstanceManager()->getEffectInstanceByID(g_iEffectInstanceID);
				int32 BoneID = ptestModel->getBoneIDByName("Bip01 Spine1");
				Vector3D BonePos = ptestModel->getBoneWorldPositionByID(BoneID);
				pp += 0.005f;
				pEffectInst->setPosition( BonePos.x + pp, BonePos.y, BonePos.z );
			}
		}


		// Work out a frame delta time.
		const double frameDeltaTime = device->getTimer()->GetElapsedTime() / 1000.0; // Time in seconds
		renderdevice->setRenderDeviceTime(device->getTimer()->getTime(), frameDeltaTime);

		// Sync with other working Thread
		renderdevice->GetMTResourceLoader()->syncRenderResource();

		if( receiver.bMButtonDown )
		{
			g_pTestCamera.SetPanX( (float(receiver.getXPosition()) - float(LastXPos)) * -0.01f );
			g_pTestCamera.SetPanY( (float(receiver.getYPosition()) - float(LastYPos)) * 0.01f );
		}
		else
		{
			g_pTestCamera.SetPanX( 0 );
			g_pTestCamera.SetPanY( 0 );
		}
		if( receiver.bRButtonDown )
		{
			g_pTestCamera.SetRotationSpeedX((float(receiver.getYPosition()) - float(LastYPos)) * -0.5f);
			g_pTestCamera.SetRotationSpeedY((float(receiver.getXPosition()) - float(LastXPos)) * 0.5f);
		}
		else
		{
			g_pTestCamera.SetRotationSpeedX(0);
			g_pTestCamera.SetRotationSpeedY(0);
		}

		LastXPos = receiver.getXPosition();
		LastYPos = receiver.getYPosition();

		if( receiver.IsKeyDown(KEY_KEY_W) )
		{
			g_pTestCamera.SetZoom(0.2f);
		}
		else if( receiver.IsKeyDown(KEY_KEY_S) )
		{
			g_pTestCamera.SetZoom(-0.2f);
		}
		else if( receiver.IsKeyDown(KEY_KEY_A) )
		{
			g_pTestCamera.SetPanX(-0.2f);
		}
		else if( receiver.IsKeyDown(KEY_KEY_D) )
		{
			g_pTestCamera.SetPanX(0.2f);
		}
		else if( receiver.IsKeyDown(KEY_KEY_Z) )
		{
			Vector4D CameraPos = g_pTestCamera.GetPos();
			g_pTestCamera.SetPos(CameraPos.x, 3.0f+renderdevice->GetWorldSystemManager()->getTerrainHeight(CameraPos.x,CameraPos.z), CameraPos.z);
		}



		g_pTestCamera.Update( (float)frameDeltaTime );	// DeltaTime in seconds

		g_pTestCamera.SetZoom(0);
		
		// update Camera position
		renderdevice->setViewMatrix3D( g_pTestCamera.GetRight(),
			g_pTestCamera.GetUp(),
            g_pTestCamera.GetDir(),
            g_pTestCamera.GetPos() );



		if( receiver.IsKeyDown(KEY_LEFT) )
		{
			CSGPSkyDome* pSkyDome = const_cast<CSGPSkyDome*>(renderdevice->GetWorldSystemManager()->getSkydome());
			pSkyDome->GetScatter().m_fInscatteringMultiplier -= 0.01f;
		}
		if( receiver.IsKeyDown(KEY_RIGHT) )
		{
			CSGPSkyDome* pSkyDome = const_cast<CSGPSkyDome*>(renderdevice->GetWorldSystemManager()->getSkydome());
			pSkyDome->GetScatter().m_fInscatteringMultiplier += 0.01f;
		}
		if( receiver.IsKeyDown(KEY_PRIOR) )
		{
			CSGPSkyDome* pSkyDome = const_cast<CSGPSkyDome*>(renderdevice->GetWorldSystemManager()->getSkydome());
			pSkyDome->GetScatter().m_fBetaMieMultiplier -= 0.00001f;
		}
		if( receiver.IsKeyDown(KEY_NEXT) )
		{
			CSGPSkyDome* pSkyDome = const_cast<CSGPSkyDome*>(renderdevice->GetWorldSystemManager()->getSkydome());
			pSkyDome->GetScatter().m_fBetaMieMultiplier += 0.00001f;
		}
		if( receiver.IsKeyDown(KEY_INSERT) )
		{
			CSGPSkyDome* pSkyDome = const_cast<CSGPSkyDome*>(renderdevice->GetWorldSystemManager()->getSkydome());
			pSkyDome->GetScatter().m_fBetaRayMultiplier -= 0.2f;
		}
		if( receiver.IsKeyDown(KEY_DELETE) )
		{
			CSGPSkyDome* pSkyDome = const_cast<CSGPSkyDome*>(renderdevice->GetWorldSystemManager()->getSkydome());
			pSkyDome->GetScatter().m_fBetaRayMultiplier += 0.2f;
		}


		renderdevice->GetWorldSystemManager()->updateWorld( (float)frameDeltaTime );

		ptestModel->update( (float)frameDeltaTime );

		renderdevice->GetEffectInstanceManager()->updateAllEffectInstance( (float)frameDeltaTime );


		averageMS += (frameDeltaTime*1000.0 - averageMS) * 0.1;
		averageFPS = 1000.0 / averageMS;
		
	
		if(receiver.IsKeyDown(KEY_ESCAPE))
		{
			device->closeDevice();
		}

		// Set render to other frame buffer
		renderdevice->setRenderToFrameBuffer();



		renderdevice->setClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		renderdevice->beginScene(true, true, false);


		
		renderdevice->GetWorldSystemManager()->renderWorld();

		ptestModel->render();

		renderdevice->GetEffectInstanceManager()->renderAllEffectInstance();


		renderdevice->GetVertexCacheManager()->RenderSphere( Vector3D(45.0f, 38.0f, 24.0f), 30.0f, Colour(0,255,0) );
		renderdevice->GetVertexCacheManager()->RenderDetailSphere( Vector3D(45.0f, 38.0f, 24.0f), 0.3f, 5, 5, Colour(0,255,0) );


		renderdevice->FlushRenderBatch();

		//renderdevice->GetVertexCacheManager()->RenderLines( (16*8+1)*2*2, GuidVertex, false );
		//renderdevice->FlushEditorLinesRenderBatch(false);


		// reset back to default back buffer
		// NOTE: After this function, Scene Depth will not be valid!!!
		renderdevice->renderBackToMainBuffer();

		// Below render context will not use depth, For example, no-depth lines, post process and 2D text etc.
		// Render Coordinate Axis
		RenderCoordinateAxis(renderdevice, AxisVertex);	

		renderdevice->FlushEditorLinesRenderBatch(true);

		{
			
			// screen coordinate system is Top-left (0,0) Bottom-Right(width, height)
			renderdevice->BeginRenderText();

			// Render Coordinate Lables
			RenderLableXYZ( renderdevice,
				AxisVertex[1].x, AxisVertex[1].y, AxisVertex[1].z,
				AxisVertex[3].x, AxisVertex[3].y, AxisVertex[3].z,
				AxisVertex[5].x, AxisVertex[5].y, AxisVertex[5].z );

			renderdevice->DrawTextInPos( 10, 10, SGPFDL_SHADOW, 16, 200, 200, 200, L"äÖÈ¾Ö¡ÊýFPS = %f ", averageFPS );
			renderdevice->DrawTextInPos( 10, 30, SGPFDL_DEFAULT, 16, 200, 200, 200, L"Very detailed Chunk Num = %d ",
				static_cast<COpenGLRenderDevice*>(renderdevice)->getOpenGLTerrainRenderer()->getVeryDetailedChunkNumber() );
			renderdevice->DrawTextInPos( 10, 50, SGPFDL_DEFAULT, 16, 200, 200, 200, L"LOD0 Chunk Num = %d ",
				static_cast<COpenGLRenderDevice*>(renderdevice)->getOpenGLTerrainRenderer()->getLOD0ChunkNumber() );
			renderdevice->DrawTextInPos( 10, 70, SGPFDL_DEFAULT, 16, 200, 200, 200, L"LOD1 Chunk Num = %d ",
				static_cast<COpenGLRenderDevice*>(renderdevice)->getOpenGLTerrainRenderer()->getLOD1ChunkNumber() );
			renderdevice->DrawTextInPos( 10, 90, SGPFDL_DEFAULT, 16, 200, 200, 200, L"Blend LOD Chunk Num = %d ",
				static_cast<COpenGLRenderDevice*>(renderdevice)->getOpenGLTerrainRenderer()->getLODBlendChunkNumber() );

			renderdevice->EndRenderText();

		}

		renderdevice->endScene();
	}

	
	renderdevice->deleteRenderToFrameBuffer();


	ptestModel->destroyModel();
	delete ptestModel;
	ptestModel = NULL;
#if 1
	delete pObj0;
	pObj0 = NULL;
#endif



	renderdevice->GetWorldSystemManager()->shutdownWorld();

	renderdevice->GetEffectInstanceManager()->clearAllEffectInstance();

#if 1
	renderdevice->GetWorldSystemManager()->getWorldMap()->Release();
	delete pWorldMap;
	pWorldMap = NULL;
#endif


	if( testDDSID7 != 0 )
		renderdevice->GetTextureManager()->unRegisterTextureByID(testDDSID7);


	delete device;
	device = NULL;

	Logger::setCurrentLogger(nullptr);

	return 0;
}

#endif