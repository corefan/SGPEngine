
// SGP_WorldEditor.cpp : ����Ӧ�ó��������Ϊ��
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
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSGP_WorldEditorApp ����

CSGP_WorldEditorApp::CSGP_WorldEditorApp()
{
	m_hDllModelViewer=NULL;
	m_bHiColorIcons = TRUE;

	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���:
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("SGP_WorldEditor.AppID.1.0.0.1"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CSGP_WorldEditorApp ����

CSGP_WorldEditorApp theApp;


// CSGP_WorldEditorApp ��ʼ��

BOOL CSGP_WorldEditorApp::InitInstance()
{
	// check whether application can startup
	if(!CheckStartup()) return FALSE;

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// ��ʼ�� OLE ��
/*	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}*/

	AfxEnableControlContainer();

	AfxInitRichEdit2();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("SGPEngine_WorldEditor 1.0.1.1"));
	LoadStdProfileSettings(5);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


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
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSGP_WorldEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CSGP_WorldEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����

	return TRUE;
}

int CSGP_WorldEditorApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
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

// CSGP_WorldEditorApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CSGP_WorldEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSGP_WorldEditorApp �Զ������/���淽��

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

// CSGP_WorldEditorApp ��Ϣ�������

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