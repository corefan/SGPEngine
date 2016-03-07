// SGP_ModelViewer.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "MainFrame.h"
#include "ModelViewerView.h"
#include "ModelViewerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CSGP_ModelViewerApp

BEGIN_MESSAGE_MAP(CSGP_ModelViewerApp, CWinAppEx)
END_MESSAGE_MAP()


// CSGP_ModelViewerApp 构造

CSGP_ModelViewerApp::CSGP_ModelViewerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif
	SetAppID(_T("ModelViewer.AppID.NoVersion"));
}

// 唯一的一个 CSGP_ModelViewerApp 对象

CSGP_ModelViewerApp theApp;

// CSGP_ModelViewerApp 初始化

BOOL CSGP_ModelViewerApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// init OLE library
	if (!AfxOleInit())
	{
		AfxMessageBox(_T("初始化OLE库失败!"));
		return FALSE;
	}
	AfxEnableControlContainer();
//	EnableTaskbarInteraction(FALSE);

	AfxInitRichEdit2();

	SetRegistryKey(_T("SGPEngine_ModelViewer 1.01"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)

//	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = FALSE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	return TRUE;
}


int CSGP_ModelViewerApp::ExitInstance()
{
//	AfxOleTerm(FALSE);
	
	return CWinAppEx::ExitInstance();
}
