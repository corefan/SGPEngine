// DEMO_Win32Windows.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DEMO_Win32Windows.h"
#include "../../SGPLibraryCode/SGPHeader.h"
#include "DEMO_Win32_Camera.h"
#include "OpenALSound.h"
#include "SceneSound.h"
#include "SoundManager.h"
#include "SoundManagerMT.h"

#ifdef _DEBUG
#include "vld.h"
#endif

#ifndef SGP_WINDOWS
#error Windows only example
#else



using namespace sgp;
CTestCamera g_pTestCamera;
CSkeletonMeshInstance *pTestModelInstance = NULL;
SoundManagerMT *g_pSoundManager = NULL;
//Sound *g_pTestSound = NULL;
//SceneSound *g_pSceneSoundSource = NULL;
//SceneSoundListener *g_pSoundListener = NULL;



#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
SGPDevice*			device = NULL;				// Main Device
ISGPRenderDevice*	renderdevice = NULL;		// Render Interface
HWND hOKButton;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int, Logger* plog);
LRESULT CALLBACK	CustomWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DEMO_WIN32WINDOWS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// create log
	ConsoleLogger logger( String("Hello World") );
	logger.setLogLevel( ELL_DEBUG );
	Logger::setCurrentLogger( &logger );

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow, &logger))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEMO_WIN32WINDOWS));




		while(device->run() && renderdevice)
		{
			// Work out a frame delta time.
			const double frameDeltaTime = device->getTimer()->GetElapsedTime() / 1000.0; // Time in seconds
			renderdevice->setRenderDeviceTime(device->getTimer()->getTime(), frameDeltaTime);

			// Sync with other working Thread (render res loading thread / sound res loading thread)
			renderdevice->GetMTResourceLoader()->syncRenderResource();
			device->getSoundManager()->syncSoundResource();

			g_pTestCamera.Update( (float)frameDeltaTime );	

			pTestModelInstance->update( (float)frameDeltaTime );


			//g_pTestCamera.SetRotationSpeedX(-0.5f);
			g_pTestCamera.SetRotationSpeedY(0.5f);


			// update Camera position
			renderdevice->setViewMatrix3D( g_pTestCamera.GetRight(),
				g_pTestCamera.GetUp(),
				g_pTestCamera.GetDir(),
				g_pTestCamera.GetPos() );

			// Update Sound system
			if (g_pSoundManager)
			{
				OpenALSoundSource* pALSoundSource = (OpenALSoundSource*)(g_pSoundManager->getSceneSoundSourceByName(String(L"Source1")));
				if(pALSoundSource && pALSoundSource->pSoundSource)
				{
					pALSoundSource->pSoundSource->SetPos(Vector4D(pTestModelInstance->getPosition()));
					pALSoundSource->pSoundSource->SetDir(Vector4D(1,0,0,1));
				}
				SceneSoundListener* pALSoundListener = (SceneSoundListener*)(g_pSoundManager->getListener());
				if( pALSoundListener )
				{
					pALSoundListener->SetPos(g_pTestCamera.GetPos());
					pALSoundListener->SetDir(g_pTestCamera.GetDir());
					pALSoundListener->SetUp(g_pTestCamera.GetUp());
				}

				if(g_pSoundManager)
					g_pSoundManager->Update((float)frameDeltaTime);
			}

			renderdevice->setClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			renderdevice->beginScene(true, true, false);

			pTestModelInstance->render();

			renderdevice->FlushRenderBatch();

			// screen coordinate system is Top-left (0,0) Bottom-Right(width, height)
			renderdevice->BeginRenderText();
			renderdevice->DrawTextInPos( 10, 10, SGPFDL_SHADOW, 16, 200, 200, 200, L"测试字符串" );
			renderdevice->EndRenderText();

			renderdevice->endScene();
		}
		{
			pTestModelInstance->destroyModel();
			delete pTestModelInstance;
			pTestModelInstance = NULL;
		}

	//if(g_pTestSound)
	//{
	//	delete g_pTestSound;
	//	g_pTestSound = NULL;
	//}

	delete device;
	device = NULL;

	Logger::setCurrentLogger(nullptr);

	return 0/*(int) msg.wParam*/;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)CustomWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= DLGWINDOWEXTRA;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEMO_WIN32WINDOWS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DEMO_WIN32WINDOWS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int, Logger*	)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, Logger* plog)
{
	HWND hWnd;

	hInst = hInstance; // 将实例句柄存储在全局变量中

	int windowWidth = 1024;
	int windowHeight = 768;



	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	windowWidth = clientRect.right;
	windowHeight = clientRect.bottom;

	// create ok button
	hOKButton = CreateWindow("BUTTON", "OK - Close", WS_CHILD | WS_VISIBLE | BS_TEXT,
		windowWidth - 160, windowHeight - 40, 150, 30, hWnd, NULL, hInstance, NULL);

	// create some text
	CreateWindow("STATIC", "This is SGP running inside a standard Win32 window.\n"\
		"Also mixing with MFC and .NET Windows.Forms is possible.",
		WS_CHILD | WS_VISIBLE, 20, 20, 400, 40, hWnd, NULL, hInstance, NULL);

	// create window to put SGP in
	HWND hSGPWindow = CreateWindow("BUTTON", "",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			50, 80, 800, 600, hWnd, NULL, hInstance, NULL);

	/*
	So now that we have some window, we can create an SGP device
	inside of it. We use SGP Engine createEx() function for this. We only
	need the handle (HWND) to that window, set it as windowsID parameter
	and start up the engine as usual. That's it.
	*/


	SGPCreationParameters param;
	param.DriverType = SGPDT_OPENGL;
	param.WindowId = reinterpret_cast<void*>(hSGPWindow);
	param.plog = plog;
	param.MultiThreadResLoading = true;

	device = createDeviceEx(param);
	// could not create selected driver.
	if (device == NULL)
	{
		Logger::setCurrentLogger(nullptr);
		return FALSE;
	}





	// 将实例句柄存储在全局变量中
	renderdevice = device->getRenderDevice();
	

	// Setting Working Dir
	String WorkingDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getParentDirectory().getParentDirectory().getFullPathName();
	WorkingDir = WorkingDir + File::separatorString + String("Data");
	renderdevice->setWorkingDirection(WorkingDir);


	// Camera
	renderdevice->setCameraMode(SGPCT_PERSPECTIVE);
	renderdevice->setNearFarClipPlane(0.01f, 100.0f);	
	renderdevice->setFov(45.0f);

	renderdevice->onResize( 800, 600 );
	SViewPort viewarea;
	viewarea.Width = renderdevice->getScreenSize().Width;
	viewarea.Height = renderdevice->getScreenSize().Height;
	renderdevice->setViewPort(viewarea);

	g_pTestCamera.SetPos(0, 100, -100);
	g_pTestCamera.SetLookAt(0, 0, 0);
	g_pTestCamera.SetRotation(-float_Pi/7, float_Pi/7, 0);

	// init Font// could not create selected font.
	if( !renderdevice->CreateFontInManager( "font1", String(L"Font\\YaHei_Consolas.ttf"), true, false, 16 ) )
	{
		delete device;
		device = NULL;
		Logger::setCurrentLogger(nullptr);
		return 1;	
	}

	renderdevice->PreCacheChar( String("FPSxyz") );

	renderdevice->GetWorldSystemManager()->createWorldSun();

	pTestModelInstance = new CSkeletonMeshInstance(renderdevice);
	pTestModelInstance->changeModel( String(L"SkeletonMesh\\magician_male_10000.mf1") );
	pTestModelInstance->setScale(0.02f);
	pTestModelInstance->playAnim(1.0f, 75, 99, true, true);
	pTestModelInstance->setVisible(true);

	g_pSoundManager = new(std::nothrow) SoundManagerMT(device);
	device->setSoundManager(g_pSoundManager);
	if (g_pSoundManager)
	{
		g_pSoundManager->setWorkingDirection(WorkingDir);
		g_pSoundManager->addListener(String(L"MainListener"));
		g_pSoundManager->addSceneSoundSource(String(L"Source1"), String(L"\\Sound\\200042.ogg"), 5, 30, true, true, true);
	}
	//OpenALSoundSource* pALSoundSource = (OpenALSoundSource*)(g_pSoundManager->getSceneSoundSourceByName(String(L"Source1")));
	//if(pALSoundSource)
	//{
	//	pALSoundSource->pSoundSource->getSound()->setConeInnerAngle(30.0f);
	//	pALSoundSource->pSoundSource->getSound()->setConeOuterAngle(90.0f);
	//	pALSoundSource->pSoundSource->getSound()->Play(true);
	//}

	//g_pTestSound = new Sound(WorkingDir + String(L"\\Sound\\WA035.wav"));
	//g_pTestSound = new Sound(WorkingDir + String(L"\\Sound\\200042.ogg"));
	//g_pTestSound->Play(false);
	//g_pSceneSoundSource = new SceneSound(WorkingDir + String(L"\\Sound\\WA090.wav"), 5, 30, true);
	//g_pSceneSoundSource->getSound()->setConeInnerAngle(30.0f);
	//g_pSceneSoundSource->getSound()->setConeOuterAngle(90.0f);
	//g_pSceneSoundSource->getSound()->Play(true);
	//g_pSoundListener = new SceneSoundListener();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数: CustomWndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		if( hOKButton == (HWND)lParam )
		{
			DestroyWindow(hWnd);
			break;
		}
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	//case WM_PAINT:
	//	hdc = BeginPaint(hWnd, &ps);
	//	EndPaint(hWnd, &ps);
	//	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


#endif