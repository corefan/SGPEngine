#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ModelViewer_Interface.h"

#include "ModelViewerThread.h"
#include "MainFrame.h"
#include "ModelViewerView.h"
#include "ModelViewerDoc.h"
#include "ModelViewerConfig.h"
#include "ModelViewerRenderInterface.h"
#include "ResourceTreeCtrl.h"

ExportControlClass* ExportControlClass::s_Instance=NULL;

ExportControlClass* ExportControlClass::GetInstance()
{
	if(s_Instance==NULL)
		s_Instance=new ExportControlClass;
	return s_Instance;
}

void ExportControlClass::ReleaseInstance()
{
	if(s_Instance!=NULL)
	{
		delete s_Instance;
		s_Instance=NULL;
	}
}

void ExportControlClass::CreateThread()
{
/*	CModelViewerThread *pThread = new CModelViewerThread;*/
	CModelViewerThread* pThread = (CModelViewerThread*)AfxBeginThread(RUNTIME_CLASS(CModelViewerThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	m_ThreadVector.push_back(pThread);
	CDocManager::bStaticInit=FALSE;
	CSingleDocTemplate* pDocTemplate=new CSingleDocTemplate(IDR_MAINFRAME,\
		RUNTIME_CLASS(CModelViewerDoc),\
		RUNTIME_CLASS(CMainFrame),\
		RUNTIME_CLASS(CModelViewerView));
	m_DocTemplateVector.push_back(pDocTemplate);
	pThread->m_pDocTemplate=pDocTemplate;
//	pThread->CreateThread();
	pThread->ResumeThread();
}

void ExportControlClass::DestoryAllThread()
{
	int iCount=m_ThreadVector.size();
	for(int i=0;i<iCount;i++)
	{
		CModelViewerThread *pThread=m_ThreadVector[i];
		CMainFrame* pMainFrame=pThread->m_pFrame;
		if(pMainFrame!=NULL)
		{
		//	if(::IsWindow(pMainFrame->m_hWnd))
		//	if(pMainFrame->m_hWnd!=NULL)
		//		pMainFrame->SendMessage(WM_CLOSE);
		//	if(pMainFrame->GetSafeHwnd()!=NULL)
		//		::SendMessage(pMainFrame->GetSafeHwnd(),WM_CLOSE,0,0);
			pMainFrame=NULL;
		}

	/*	if(pThread!=NULL)
		{
			delete pThread;
			pThread=NULL;
		}*/
		pThread->PostThreadMessage(WM_QUIT,0,0);
		WaitForSingleObject(pThread->m_hThread,INFINITE);

		if(m_DocTemplateVector[i]!=NULL)
		{
			delete m_DocTemplateVector[i];
			m_DocTemplateVector[i]=NULL;
		}
	}
}

extern "C" __declspec(dllexport) void CreateModelViewer(CString strPath)
{
#ifdef _AFXDLL
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("ModelViewer:Using MFC in Dynamic DLL\n");
#else
	TRACE("Model Viewer:Using MFC in Static Library\n");
#endif

	ExportControlClass::GetInstance()->CreateThread();
	ModelViewerConfig::GetInstance()->m_InitModelPath = strPath;
}

extern "C" __declspec(dllexport) void DestroyAllModelViewer()
{
#ifdef _AFXDLL
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	ExportControlClass::GetInstance()->DestoryAllThread();
	ExportControlClass::ReleaseInstance();
	ModelViewerRenderInterface::ReleaseInstance();
}

/*extern "C" __declspec(dllexport) void SetInitialModel(CString strPath)
{
	ModelViewerConfig::GetInstance()->m_InitModelPath=strPath;
}*/
HWND g_Hwnd=NULL;

extern "C" __declspec(dllexport) void OpenModel(CString strPath)
{
/*	CString strName = strPath.Right(strPath.GetLength()-strPath.ReverseFind('\\')-1);
	CResourceTreeCtrl::OpenMF1File(strPath,strName);*/
	ModelViewerConfig::GetInstance()->m_InitModelPath = strPath;
	if(g_Hwnd!=NULL) ::SendMessage(g_Hwnd,UM_FRAMEUI_INIT,0,0);
}