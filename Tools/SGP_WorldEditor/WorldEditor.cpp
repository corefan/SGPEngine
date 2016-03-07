
// SGP_WorldEditor.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "WorldEditor.h"
#include "MainFrame.h"

#include "WorldEditorDoc.h"
#include "WorldEditorView.h"

#include ".\Render Interface\WorldEditorRenderInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSGP_WorldEditorApp

BEGIN_MESSAGE_MAP(CSGP_WorldEditorApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSGP_WorldEditorApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSGP_WorldEditorApp 构造

CSGP_WorldEditorApp::CSGP_WorldEditorApp()
{
	m_hDllModelViewer=NULL;
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("SGP_WorldEditor.AppID.1.0.0.1"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CSGP_WorldEditorApp 对象

CSGP_WorldEditorApp theApp;


// CSGP_WorldEditorApp 初始化

BOOL CSGP_WorldEditorApp::InitInstance()
{
	// check whether application can startup
	if(!CheckStartup()) return FALSE;

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
/*	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}*/

	AfxEnableControlContainer();

	AfxInitRichEdit2();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("SGPEngine_WorldEditor 1.0.1.1"));
	LoadStdProfileSettings(5);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Gdiplus Init
	GdiplusStartupInput input;
	GdiplusStartup(&m_Token,&input,NULL);

//	CDocManager::bStaticInit=FALSE;
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSGP_WorldEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CSGP_WorldEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生

	return TRUE;
}

int CSGP_WorldEditorApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
//	AfxOleTerm(FALSE);

	if(m_hDllModelViewer!=NULL)
	{
		typedef void (*Show)(void);
		Show pAddr=(Show)GetProcAddress(m_hDllModelViewer,"DestroyAllModelViewer");
		if(pAddr!=NULL) pAddr();
		FreeLibrary(m_hDllModelViewer);
	}

	GdiplusShutdown(m_Token);

	return CWinAppEx::ExitInstance();
}

// CSGP_WorldEditorApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CSGP_WorldEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSGP_WorldEditorApp 自定义加载/保存方法

void CSGP_WorldEditorApp::PreLoadState()
{
/*	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);*/
}

void CSGP_WorldEditorApp::LoadCustomState()
{
}

void CSGP_WorldEditorApp::SaveCustomState()
{
}

// CSGP_WorldEditorApp 消息处理程序

BOOL CSGP_WorldEditorApp::CheckStartup()
{
	/*******************************************************
		check work directory
	*******************************************************/
	TCHAR path[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),path,MAX_PATH);
	CString strPath=path;
	while(strPath.Right(4)!=_T("\\Bin"))
	{
		int pos=strPath.ReverseFind('\\');
		if(pos==-1)
		{
			AfxMessageBox("Can't find Bin directory!\n\nThe executable file should be put in Bin directory!",MB_OK|MB_ICONERROR);
			return FALSE;
		}
		strPath = strPath.Left(pos);
	}
	int pos=strPath.ReverseFind('\\');
	if(pos==-1)
	{
		AfxMessageBox("Can't find work directory!\n\nPlease set work directory correctly and startup again!",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	strPath = strPath.Left(pos);
	strPath+=_T("\\Data");

	if(!PathIsDirectory(strPath))
	{
	//	MessageBox(NULL,"Can't find work directory!\nPlease correctly set work directory!","Startup Error!",MB_OK);
		AfxMessageBox("Can't find work directory!\n\nPlease set work directory correctly and startup again!",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	return TRUE;
}

BOOL CSGP_WorldEditorApp::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		MouseMsgInterface::GetInstance()->StopInsertObjects();

	return CWinAppEx::PreTranslateMessage(pMsg);
}


BOOL CSGP_WorldEditorApp::OnIdle(LONG lCount)
{
	if(IsRenderable())
	{
		WorldEditorRenderInterface::GetInstance()->Render();

		//update UI
	#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
		// check MFC's allocator (before idle)
		if (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & _CRTDBG_CHECK_ALWAYS_DF)
			ASSERT(AfxCheckMemory());
	#endif

		if (lCount <= 0)
		{
			// send WM_IDLEUPDATECMDUI to the main window
			CWnd* pMainWnd = m_pMainWnd;
			if (pMainWnd != NULL && pMainWnd->m_hWnd != NULL &&
				pMainWnd->IsWindowVisible())
			{
				AfxCallWndProc(pMainWnd, pMainWnd->m_hWnd,
					WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
				pMainWnd->SendMessageToDescendants(WM_IDLEUPDATECMDUI,
					(WPARAM)TRUE, 0, TRUE, TRUE);
			}
			// send WM_IDLEUPDATECMDUI to all frame windows
			AFX_MODULE_THREAD_STATE* pState = _AFX_CMDTARGET_GETSTATE()->m_thread;
			CFrameWnd* pFrameWnd = pState->m_frameList;
			while (pFrameWnd != NULL)
			{
				if (pFrameWnd->m_hWnd != NULL && pFrameWnd != pMainWnd)
				{
					if (pFrameWnd->m_nShowDelay == SW_HIDE)
						pFrameWnd->ShowWindow(pFrameWnd->m_nShowDelay);
					if (pFrameWnd->IsWindowVisible() ||
						pFrameWnd->m_nShowDelay >= 0)
					{
						AfxCallWndProc(pFrameWnd, pFrameWnd->m_hWnd,
							WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
						pFrameWnd->SendMessageToDescendants(WM_IDLEUPDATECMDUI,
							(WPARAM)TRUE, 0, TRUE, TRUE);
					}
					if (pFrameWnd->m_nShowDelay > SW_HIDE)
						pFrameWnd->ShowWindow(pFrameWnd->m_nShowDelay);
					pFrameWnd->m_nShowDelay = -1;
				}
				pFrameWnd = pFrameWnd->m_pNextFrameWnd;
			}
		}
		else if (lCount >= 0)
		{
			AFX_MODULE_THREAD_STATE* pState = _AFX_CMDTARGET_GETSTATE()->m_thread;
			if (pState->m_nTempMapLock == 0)
			{
				// free temp maps, OLE DLLs, etc.
				AfxLockTempMaps();
				AfxUnlockTempMaps();
			}
		}

	#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
		// check MFC's allocator (after idle)
		if (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & _CRTDBG_CHECK_ALWAYS_DF)
			ASSERT(AfxCheckMemory());
	#endif
		return TRUE;
	}
	else
	{
		return CWinAppEx::OnIdle(lCount);
	}
	
}

BOOL CSGP_WorldEditorApp::IsRenderable()
{
	if(m_pMainWnd==NULL) return FALSE;
	// whether minimize
	if(::IsIconic(m_pMainWnd->m_hWnd)) return FALSE;
	
	return TRUE;
}