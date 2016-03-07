//
//  OGLES2_iOSDemo.cpp
//  SGPEngine
//
//  Created by xyweng on 14-1-2.
//
//

#include "OGLES2_iOSDemo.h"


using namespace sgp;


#include "OGLES2_iOSCamera.h"
#include "OGLES2_iOSLogger.h"



/******************************************************************************
 Defines
 ******************************************************************************/
//SGPVertex_UPOS_VERTEXCOLOR GuidVertex[17*2*2];
//SGPVertex_UPOS_TEXTURE QuadVertex[6];

//! Interface for getting and manipulating the virtual time
class ISGPTimer
{
public:
    virtual ~ISGPTimer() {}
    
	//! Returns current real time in milliseconds of the system.
	/** This value does not start with 0 when the application starts.
     For example in one implementation the value returned could be the
     amount of milliseconds which have elapsed since the system was started.
     */
	virtual double getRealTime() const = 0;
    
	//! Returns current virtual time in milliseconds.
	/** This value could starts with 0 after you using setTime function.
     This value depends on the set speed of
     the timer if the timer is stopped, etc. If you need the system time,
     use getRealTime() */
	virtual uint32 getTime() const = 0;
    
	//! sets current virtual time
	virtual void setTime(uint32 time) = 0;
    
    
	//! Sets the speed of the timer
	/** The speed is the factor with which the time is running faster or
     slower then the real system time. */
	virtual void setSpeed(float speed = 1.0f) = 0;
    
	//! Returns current speed of the timer
	/** The speed is the factor with which the time is running faster or
     slower then the real system time. */
	virtual float getSpeed() const = 0;
    
	//! Returns virtual time elapsed since previous tick in milliseconds.
	virtual double GetElapsedTime() = 0;
    
    
	//! Advances the virtual time
	/** Makes the virtual timer update the time value based on the real
     time. This is called automatically when calling SGPDevice::run(),
     but you can call it manually if you don't use this method. */
	virtual void tick() = 0;
};

class COPENGLES2EngineTimer : public ISGPTimer
{
public:
	COPENGLES2EngineTimer() : m_Timer(Time::getCurrentTime())
	{
		VirtualTimerSpeed = 1.0f;
		VirtualTimeElapsed = 0;
		LastVirtualTime = 0;
        
		StaticTime = getRealTime();
		StartRealTime = StaticTime;
	}
    virtual ~COPENGLES2EngineTimer() {}
    
	//! Returns current real time in milliseconds of the system.
	/** Returns the number of millisecs since a fixed event (usually system startup).  */
	virtual double getRealTime() const
	{
		return m_Timer.getMillisecondCounterHiRes();
	}
    
	//! Returns current virtual time in milliseconds.
	/** This value starts with 0 and can be manipulated using setTime().
     If you need the system time, use getRealTime() */
	virtual uint32 getTime() const
	{
		return LastVirtualTime + (uint32)((StaticTime - StartRealTime) * (double)(VirtualTimerSpeed));
	}
    
	//! sets current virtual time
	virtual void setTime(uint32 _time)
	{
		StaticTime = getRealTime();
		LastVirtualTime = _time;
		StartRealTime = StaticTime;
	}
    
	//! Returns virtual time elapsed since previous tick in milliseconds.
	virtual double GetElapsedTime()
	{
		return VirtualTimeElapsed;
	}
    
	//! Sets the speed of the timer
	/** The speed is the factor with which the time is running faster or slower then the
     real system time. */
	virtual void setSpeed(float speed = 1.0f)
	{
		setTime(getTime());
        
		VirtualTimerSpeed = speed;
		if (VirtualTimerSpeed < 0.0f)
			VirtualTimerSpeed = 0.0f;
	}
    
	//! Returns current speed of the timer
	/** The speed is the factor with which the time is running faster or slower then the
     real system time. */
	virtual float getSpeed() const
	{
		return VirtualTimerSpeed;
	}
    
    
	//! Advances the virtual time
	/** Makes the virtual timer update the time value based on the real time. This is
     called automaticly when calling SGPDevice::run(), but you can call it manually
     if you don't use this method. */
	virtual void tick()
	{
		VirtualTimeElapsed = (getRealTime() - StaticTime) * (double)(VirtualTimerSpeed);
		StaticTime = getRealTime();
	}
    
private:
	Time m_Timer;				// Core Timer
    
	double StartRealTime;		// Real-time computer Data Time
	double StaticTime;			// Computer Data Time when this timer created
    
	uint32 LastVirtualTime;		// User given Virtual Time (could set 0)
	float VirtualTimerSpeed;	// scaling factor for time 
    
	double VirtualTimeElapsed;	// Virtual time elapsed since previous tick (in milliseconds)
};





/*!****************************************************************************
 To use the shell, you have to inherit a class from PVRShell
 and implement the five virtual functions which describe how your application
 initializes, runs and releases the ressources.
 ******************************************************************************/
class OGLESIntroducingPVRShell : public PVRShell
{
public:
	virtual bool InitApplication();
	virtual bool InitView();
	virtual bool ReleaseView();
	virtual bool QuitApplication();
	virtual bool RenderScene();
    
public:
	ISGPRenderDevice*                           m_pRenderDevice;
	SGPOpenGLES2CreationParameters               m_CreationParameters;
    
	CIOSOpenGLES2Camera                         m_GLES2Camera;
    
	ScopedPointer<Random>                       m_pRandomizer;
	ScopedPointer<COPENGLES2EngineTimer>        m_pTimer;
	ScopedPointer<PVRShellLogger>               m_pLogger;
    
private:
	double		m_AverageMS;
	float		m_LastXPos;
	float		m_LastYPos;
	bool        m_bMousePressed;
    
    float       m_LastXPos2;
    float       m_LastYPos2;
    
	//uint32					m_testPVRID;
	CSkeletonMeshInstance *	m_PlayerRole;
	CSGPWorldMap*			m_pWorldMap;
};

/*!****************************************************************************
 @Function		InitApplication
 @Return		bool		true if no error occured
 @Description	Code in InitApplication() will be called by PVRShell once per
 run, before the rendering context is created.
 Used to initialize variables that are not dependant on it
 (e.g. external modules, loading meshes, etc.)
 If the rendering context is lost, InitApplication() will
 not be called again.
 ******************************************************************************/
bool OGLESIntroducingPVRShell::InitApplication()
{
/*
	// Init Guid Vertex and Quad Vertex
	{
		int vertIndex = 0;
		for( int Row=-8; Row<=8; Row++ )
		{
			GuidVertex[vertIndex].x = Row * 0.5f;
			GuidVertex[vertIndex].y = 0;
			GuidVertex[vertIndex].z = -8 * 0.5f;
	
			GuidVertex[vertIndex].VertexColor[0] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[1] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[2] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[3] = 1.0f;
			vertIndex++;
    
			GuidVertex[vertIndex].x = Row * 0.5f;
			GuidVertex[vertIndex].y = 0;
			GuidVertex[vertIndex].z = 8 * 0.5f;
	
			GuidVertex[vertIndex].VertexColor[0] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[1] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[2] = (Row == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[3] = 1.0f;
			vertIndex++;
		}
    
		for( int Col=-8; Col<=8; Col++ )
		{
			GuidVertex[vertIndex].x = -8 * 0.5f;
			GuidVertex[vertIndex].y = 0;
			GuidVertex[vertIndex].z = Col * 0.5f;
	
			GuidVertex[vertIndex].VertexColor[0] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[1] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[2] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[3] = 1.0f;
			vertIndex++;
    
			GuidVertex[vertIndex].x = 8 * 0.5f;
			GuidVertex[vertIndex].y = 0;
			GuidVertex[vertIndex].z = Col * 0.5f;
	
			GuidVertex[vertIndex].VertexColor[0] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[1] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[2] = (Col == 0) ? 0 : 0.588f;
			GuidVertex[vertIndex].VertexColor[3] = 1.0f;
			vertIndex++;
		}
        
        	QuadVertex[0].x = -4.0f; QuadVertex[0].y = 4.0f; QuadVertex[0].z = 0.0f;
        	QuadVertex[0].tu = 0.0f; QuadVertex[0].tv = 0.0f;
        	QuadVertex[1].x = -4.0f; QuadVertex[1].y = -4.0f; QuadVertex[1].z = 0.0f;
        	QuadVertex[1].tu = 0.0f; QuadVertex[1].tv = 1.0f;
        	QuadVertex[2].x =  4.0f; QuadVertex[2].y = -4.0f; QuadVertex[1].z = 0.0f;
        	QuadVertex[2].tu = 1.0f; QuadVertex[2].tv = 1.0f;
        
        	QuadVertex[3] = QuadVertex[0];
        	QuadVertex[4] = QuadVertex[2];
        	QuadVertex[5].x =  4.0f; QuadVertex[5].y =  4.0f; QuadVertex[5].z = 0.0f;
        	QuadVertex[5].tu = 1.0f; QuadVertex[5].tv = 0.0f;
    }
*/
    
    m_CreationParameters.bRotated = true;
	m_CreationParameters.Vsync = true;
	m_CreationParameters.bDiscardColor = true;
    m_CreationParameters.bPowerSaving = true;
	m_CreationParameters.MultiThreadResLoading = true;

    
    
	m_AverageMS = 0.0;
    
	// Create Logger
	m_pLogger = new PVRShellLogger( String("OpenGL ES 2.0 Logger"), this );
	m_pLogger->setLogLevel( ELL_DEBUG );
	Logger::setCurrentLogger( m_pLogger );
    
	m_CreationParameters.plog = m_pLogger;
    
	m_pRandomizer = new Random();
	m_pTimer = new COPENGLES2EngineTimer();
	m_pTimer->setTime(0);
    
	//PVRShellSet(prefFullScreen, m_CreationParameters.Fullscreen);
	//PVRShellSet(prefShowCursor, m_CreationParameters.bShowCursor);
	PVRShellSet(prefStencilBufferContext, m_CreationParameters.Stencilbuffer);
	PVRShellSet(prefPowerSaving, m_CreationParameters.bPowerSaving);
	PVRShellSet(prefDiscardColor, m_CreationParameters.bDiscardColor);
	PVRShellSet(prefDiscardDepth, m_CreationParameters.bDiscardDepth);
	PVRShellSet(prefDiscardStencil, m_CreationParameters.bDiscardStencil);
    
	//PVRShellSet(prefWidth, (int)m_CreationParameters.WindowWidth);
	//PVRShellSet(prefHeight, (int)m_CreationParameters.WindowHeight);
	//PVRShellSet(prefPositionX, (int)m_CreationParameters.WindowPositionX);
	//PVRShellSet(prefPositionY, (int)m_CreationParameters.WindowPositionY);
	PVRShellSet(prefAASamples, (int)m_CreationParameters.AntiAlias);
	PVRShellSet(prefColorBPP, (int)m_CreationParameters.Bits);
	PVRShellSet(prefDepthBPP, (int)m_CreationParameters.ZBufferBits);
	PVRShellSet(prefSwapInterval, (int)(m_CreationParameters.Vsync ? 1 : 0) );

	return true;

}
/*!****************************************************************************
 @Function		QuitApplication
 @Return		bool		true if no error occured
 @Description	Code in QuitApplication() will be called by PVRShell once per
 run, just before exiting the program.
 If the rendering context is lost, QuitApplication() will
 not be called.
 ******************************************************************************/
bool OGLESIntroducingPVRShell::QuitApplication()
{

	m_pTimer = nullptr;
	m_pRandomizer = nullptr;
    
    
	Logger::setCurrentLogger(nullptr);
    
	m_pLogger = nullptr;

    return true;
}







/*!****************************************************************************
 @Function		InitView
 @Return		bool		true if no error occured
 @Description	Code in InitView() will be called by PVRShell upon
 initialization or after a change in the rendering context.
 Used to initialize variables that are dependant on the rendering
 context (e.g. textures, vertex buffers, etc.)
 ******************************************************************************/
bool OGLESIntroducingPVRShell::InitView()
{
    setlocale(LC_CTYPE, "UTF-8");
    
	m_pRenderDevice = new COpenGLES2RenderDevice(m_CreationParameters);
    
	if( !m_pRenderDevice )
		return false;
    
    
	m_pRenderDevice->onResize( PVRShellGet(prefWidth), PVRShellGet(prefHeight) );
    
	// Setting Working Dir
	String WorkingDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName();
	WorkingDir = WorkingDir + File::separatorString + String("Data");
	m_pRenderDevice->setWorkingDirection(WorkingDir);
    Logger::getCurrentLogger()->writeToLog(String("Current working direction is ") + WorkingDir, ELL_INFORMATION);

    
	// Camera
	m_pRenderDevice->setCameraMode(SGPCT_PERSPECTIVE);
	m_pRenderDevice->setNearFarClipPlane(1.0f, 1000.0f);
	m_pRenderDevice->setFov(45.0f);
    
	SViewPort viewarea;
	viewarea.Width = m_pRenderDevice->getScreenSize().Width;
	viewarea.Height = m_pRenderDevice->getScreenSize().Height;
	m_pRenderDevice->setViewPort(viewarea);
    
	m_GLES2Camera.SetPos(10, 50, -30);
	m_GLES2Camera.SetLookAt(0, 0, 0);
	m_GLES2Camera.SetRotation(-float_Pi/4, float_Pi/7, 0);
    

    
 
	// init Font
	if( !m_pRenderDevice->CreateFontInManager( "font1", String(L"Font/simhei.ttf"), true, false, 32 ) )
	{
		// could not create selected font.
		delete m_pRenderDevice;
		m_pRenderDevice = NULL;
		return false;
	}
    
	m_pRenderDevice->PreCacheChar( String("FPSxyz") );
  
	m_LastXPos = 0;
	m_LastYPos = 0;
    m_LastXPos2 = 0;
    m_LastYPos2 = 0;
	m_bMousePressed = false;

  
	m_pRenderDevice->GetParticleManager()->initParticleSystemEngine(m_pTimer->getRealTime());
    
    m_pRenderDevice->GetWorldSystemManager()->loadWorldFromFile(WorkingDir, String(L"worldmap/water.map"));
    
#if 0
	m_pRenderDevice->GetWorldSystemManager()->createWorldSun();
	m_pRenderDevice->GetWorldSystemManager()->createSkydome( String(L"skydome/skydome.mf1") );
	m_pRenderDevice->GetWorldSystemManager()->setSkydomeTexture( String(L"skydome/cloud001.pvr"), String(L"skydome/perturb001.pvr") );
	m_pRenderDevice->GetWorldSystemManager()->setWorldSunPosition(38.0f);
	m_pWorldMap = new CSGPWorldMap();
	m_pRenderDevice->GetWorldSystemManager()->createNewWorld(m_pWorldMap, "newworld", SGPTS_SMALL, true, 30, String(L"Texture/terrain/dirt001.pvr") );
 
	// Create Water
	{
		// addWaterChunk must be called before creating Quadtree
		m_pRenderDevice->GetWorldSystemManager()->createWater(15.0f, String(L"Texture/water/wavesbump.pvr"));
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(192);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(193);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(194);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(195);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(208);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(209);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(210);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(211);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(224);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(225);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(226);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(227);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(240);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(241);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(242);
		m_pRenderDevice->GetWorldSystemManager()->addWaterChunk(243);
	}
  
    
	// Create Grass
	{
		m_pRenderDevice->GetWorldSystemManager()->createGrass( String(L"Texture/grasses.pvr"), 2, 2 );
		Vector3D terrainNorm;
		SGPGrassCluster tempGrassCluster;
		for( int i=0; i<10; i++ )
		{
			for( int j=0; j<10; j++ )
			{
				tempGrassCluster.fPositionX = 3.0f + m_pRandomizer->nextInt(256) / 25.0f;
				tempGrassCluster.fPositionZ = 3.0f + m_pRandomizer->nextInt(256) / 25.0f;
				tempGrassCluster.fPositionY = m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(tempGrassCluster.fPositionX, tempGrassCluster.fPositionZ);
				terrainNorm = m_pRenderDevice->GetWorldSystemManager()->getTerrainNormal(tempGrassCluster.fPositionX, tempGrassCluster.fPositionZ);
				tempGrassCluster.nPackedNormal = (uint32((terrainNorm.x * 0.5f + 0.5f) * 255) << 24) +
                (uint32((terrainNorm.y * 0.5f + 0.5f) * 255) << 16) +
                (uint32((terrainNorm.z * 0.5f + 0.5f) * 255) << 8);
				tempGrassCluster.nData = 0xFF000000 + (m_pRandomizer->nextInt(256) << 8) + m_pRandomizer->nextInt(256);
				m_pRenderDevice->GetWorldSystemManager()->setGrassCluster(tempGrassCluster.fPositionX, tempGrassCluster.fPositionZ, tempGrassCluster);
			}
		}
        
		CSGPWorldConfig::getInstance()->m_fGrassFarFadingStart = 32.0f;
		CSGPWorldConfig::getInstance()->m_fGrassFarFadingEnd = 80.0f;
	}
#endif
	m_pRenderDevice->GetWorldSystemManager()->initializeQuadTree();
 
	m_PlayerRole = new CSkeletonMeshInstance(m_pRenderDevice);
	m_PlayerRole->changeModel( String(L"SkeletonMesh/magician_male_10000.mf1") );
	m_PlayerRole->setScale(0.02f);
	m_PlayerRole->setRotationY(90.0f * pi_over_180);
	m_PlayerRole->playAnim(1.0f, 10, 58, true, true);				// idle
	m_PlayerRole->playUpperBodyAnim(1.0f, 75, 99, true, true);      // run
	m_PlayerRole->setEnableUpperBodyAnim(true);
    
	m_PlayerRole->setPosition(13.55f, m_pRenderDevice->GetWorldSystemManager()->getRealTerrainHeight(13.55f,13.55f) + 5.0f, 13.55f);
	m_PlayerRole->setVisible(true);
    

    return true;
}

/*!****************************************************************************
 @Function		ReleaseView
 @Return		bool		true if no error occured
 @Description	Code in ReleaseView() will be called by PVRShell when the
 application quits or before a change in the rendering context.
 ******************************************************************************/
bool OGLESIntroducingPVRShell::ReleaseView()
{


	m_pRenderDevice->GetWorldSystemManager()->shutdownWorld();
    
    m_pRenderDevice->GetEffectInstanceManager()->clearAllEffectInstance();
    
#if 0
	m_pRenderDevice->GetWorldSystemManager()->getWorldMap()->Release();
    
	if( m_pWorldMap )
	{
		delete m_pWorldMap;
		m_pWorldMap = NULL;
	}
#endif
	if( m_PlayerRole )
	{
		m_PlayerRole->destroyModel();
		delete m_PlayerRole;
		m_PlayerRole = NULL;
	}
    
	if( m_pRenderDevice )
	{
		delete m_pRenderDevice;
		m_pRenderDevice = NULL;
	}

	return true;
}

/*!****************************************************************************
 @Function		RenderScene
 @Return		bool		true if no error occured
 @Description	Main rendering loop function of the program. The shell will
 call this function every frame.
 eglSwapBuffers() will be performed by PVRShell automatically.
 PVRShell will also manage important OS events.
 Will also manage relevent OS events. The user has access to
 these events through an abstraction layer provided by PVRShell.
 ******************************************************************************/
bool OGLESIntroducingPVRShell::RenderScene()
{

	m_pTimer->tick();
    
	// Work out a frame delta time.
	const double frameDeltaTime = m_pTimer->GetElapsedTime() / 1000.0; // Time in seconds
	m_pRenderDevice->setRenderDeviceTime(m_pTimer->getTime(), frameDeltaTime);
    
    
	// Sync with other working Thread
	m_pRenderDevice->GetMTResourceLoader()->syncRenderResource();
    
 
	float *pMousePtr = (float *)PVRShellGet(prefPointerLocation);
	if( pMousePtr != NULL )
	{
        if (PVRShellIsKeyPressed(PVRShellKeyNameTWOTOUCH))
        {
            if( !m_bMousePressed )
            {
                m_LastXPos = pMousePtr[0];
                m_LastYPos = pMousePtr[1];
                m_LastXPos2 = pMousePtr[2];
                m_LastYPos2 = pMousePtr[3];
                m_bMousePressed = true;
            }
            
            float f = std::sqrt( (pMousePtr[0] - pMousePtr[2]) * (pMousePtr[0] - pMousePtr[2]) + (pMousePtr[1] - pMousePtr[3]) * (pMousePtr[1] - pMousePtr[3]) ) * 40.0f;
            
            float f0 = std::sqrt( (m_LastXPos - m_LastXPos2) * (m_LastXPos - m_LastXPos2) + (m_LastYPos - m_LastYPos2) * (m_LastYPos - m_LastYPos2) ) * 40.0f;
            
            m_GLES2Camera.SetZoom(f-f0);
            
            m_LastXPos = pMousePtr[0];
            m_LastYPos = pMousePtr[1];
            m_LastXPos2 = pMousePtr[2];
            m_LastYPos2 = pMousePtr[3];
        }
        else
        {
            if( !m_bMousePressed )
            {
                m_LastXPos = pMousePtr[0];
                m_LastYPos = pMousePtr[1];
                m_bMousePressed = true;
            }

		
            m_GLES2Camera.SetRotationSpeedX((pMousePtr[0] - m_LastXPos) * -10.0f);
            m_GLES2Camera.SetRotationSpeedY((pMousePtr[1] - m_LastYPos) * -10.0f);
        
            m_LastXPos = pMousePtr[0];
            m_LastYPos = pMousePtr[1];
        }
	}
	else
	{
		m_bMousePressed = false;
        
		m_GLES2Camera.SetPanX( 0 );
		m_GLES2Camera.SetPanY( 0 );
		m_GLES2Camera.SetRotationSpeedX(0);
		m_GLES2Camera.SetRotationSpeedY(0);
	}
    

    

 
    
	m_GLES2Camera.Update( (float)frameDeltaTime );	// DeltaTime in seconds
	m_GLES2Camera.SetZoom(0);
    
	// update Camera position
	m_pRenderDevice->setViewMatrix3D(
                                     m_GLES2Camera.GetRight(),
                                     m_GLES2Camera.GetUp(),
                                     m_GLES2Camera.GetDir(),
                                     m_GLES2Camera.GetPos() );
    
	m_pRenderDevice->GetWorldSystemManager()->updateWorld( (float)frameDeltaTime );
    
	m_PlayerRole->update( (float)frameDeltaTime );
    
    m_pRenderDevice->GetEffectInstanceManager()->updateAllEffectInstance( (float)frameDeltaTime );
    
	m_pRenderDevice->setClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
	m_pRenderDevice->beginScene(true, true, false);
 
	m_pRenderDevice->GetWorldSystemManager()->renderWorld();
	m_PlayerRole->render();
    m_pRenderDevice->GetEffectInstanceManager()->renderAllEffectInstance();
 
	m_pRenderDevice->FlushRenderBatch();


    
     m_AverageMS += (frameDeltaTime*1000.0 - m_AverageMS) * 0.1;
 
 
    // screen coordinate system is Top-left (0,0) Bottom-Right(width, height)
    m_pRenderDevice->BeginRenderText();


    m_pRenderDevice->DrawTextInPos( 10, 10, SGPFDL_DEFAULT, 32, 200, 200, 200, L"测试字符串FPS = %f", float(1000.0 / m_AverageMS) );
    
    //m_pRenderDevice->DrawTextInPos( 10, 30, SGPFDL_DEFAULT, 16, 200, 200, 200, L"Very detailed Chunk Num = %d ", static_cast<COpenGLES2RenderDevice*>(m_pRenderDevice)->getOpenGLTerrainRenderer()->getVeryDetailedChunkNumber() );
    //m_pRenderDevice->DrawTextInPos( 10, 50, SGPFDL_DEFAULT, 16, 200, 200, 200, L"LOD0 Chunk Num = %d ", static_cast<COpenGLES2RenderDevice*>(m_pRenderDevice)->getOpenGLTerrainRenderer()->getLOD0ChunkNumber() );
    //m_pRenderDevice->DrawTextInPos( 10, 70, SGPFDL_DEFAULT, 16, 200, 200, 200, L"LOD1 Chunk Num = %d ", static_cast<COpenGLES2RenderDevice*>(m_pRenderDevice)->getOpenGLTerrainRenderer()->getLOD1ChunkNumber() );
    //m_pRenderDevice->DrawTextInPos( 10, 90, SGPFDL_DEFAULT, 16, 200, 200, 200, L"Blend LOD Chunk Num = %d ", static_cast<COpenGLES2RenderDevice*>(m_pRenderDevice)->getOpenGLTerrainRenderer()->getLODBlendChunkNumber() );
 
    m_pRenderDevice->EndRenderText();

    
	m_pRenderDevice->endScene();
    


	return true;
}

/*!****************************************************************************
 @Function		NewDemo
 @Return		PVRShell*		The demo supplied by the user
 @Description	This function must be implemented by the user of the shell.
 The user should return its PVRShell object defining the
 behaviour of the application.
 ******************************************************************************/
PVRShell* NewDemo()
{
	return new OGLESIntroducingPVRShell();
}








