// SGP_ModelViewer.cpp : ���� DLL �ĳ�ʼ�����̡�
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
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CSGP_ModelViewerApp

BEGIN_MESSAGE_MAP(CSGP_ModelViewerApp, CWinAppEx)
END_MESSAGE_MAP()


// CSGP_ModelViewerApp ����

CSGP_ModelViewerApp::CSGP_ModelViewerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���:
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif
	SetAppID(_T("ModelViewer.AppID.NoVersion"));
}

// Ψһ��һ�� CSGP_ModelViewerApp ����

CSGP_ModelViewerApp theApp;

// CSGP_ModelViewerApp ��ʼ��

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
		AfxMessageBox(_T("��ʼ��OLE��ʧ��!"));
		return FALSE;
	}
	AfxEnableControlContainer();
//	EnableTaskbarInteraction(FALSE);

	AfxInitRichEdit2();

	SetRegistryKey(_T("SGPEngine_ModelViewer 1.01"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)

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
