// ModelViewerThread.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ModelViewerThread.h"

#include "MainFrame.h"
#include "ModelViewerDoc.h"

#include "ModelViewerRenderInterface.h"
#include "ModelViewer_Interface.h"

// CModelViewerThread

IMPLEMENT_DYNCREATE(CModelViewerThread, CWinThread)

CModelViewerThread::CModelViewerThread()
{
}

CModelViewerThread::~CModelViewerThread()
{
}

BOOL CModelViewerThread::InitInstance()
{
	//init Gdiplus library
	GdiplusStartupInput input;
	GdiplusStartup(&m_GdiplusToken,&input,NULL);

	m_pDoc=(CModelViewerDoc*)(m_pDocTemplate->CreateNewDocument());
	m_pFrame=(CMainFrame*)(m_pDocTemplate->CreateNewFrame(m_pDoc,NULL));
	m_pDocTemplate->InitialUpdateFrame(m_pFrame,m_pDoc);

	m_pMainWnd=m_pFrame;
	m_pFrame->ShowWindow(SW_SHOW);
	m_pFrame->CenterWindow();
	m_pFrame->UpdateWindow();

	return TRUE;
}

int CModelViewerThread::ExitInstance()
{
	// shut down Gdiplus
	GdiplusShutdown(m_GdiplusToken);

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CModelViewerThread, CWinThread)
END_MESSAGE_MAP()

// CModelViewerThread 消息处理程序

BOOL CModelViewerThread::OnIdle(LONG lCount)
{
	//if window is minimized,render don't occur
/*	if(m_pMainWnd!=NULL&&m_pMainWnd->m_hWnd!=NULL)
	{
		if(!::IsIconic(m_pMainWnd->m_hWnd))
			ModelViewerRenderInterface::GetInstance()->Render();
	}*/	
	if(IsRenderable())
	{
		ModelViewerRenderInterface::GetInstance()->Render();

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
		return CWinThread::OnIdle(lCount);
	}
	
}

BOOL CModelViewerThread::IsRenderable()
{
	if(m_pMainWnd==NULL)
	{
		return FALSE;
	}
	// whether minimize
	if(::IsIconic(m_pMainWnd->m_hWnd))
	{
		return FALSE;
	}
	
	return TRUE;
/*	CWnd* pWndTop=CWnd::GetForegroundWindow();
	if(pWndTop==NULL)
	{
		return FALSE;
	}

	if(pWndTop->m_hWnd==m_pMainWnd->m_hWnd)
	{
		return TRUE;
	}

	CRect rcTop;
	pWndTop->GetWindowRect(rcTop);

	CRect rcView;
	CView* pView=((CMainFrame*)m_pMainWnd)->GetActiveView();
	pView->GetWindowRect(rcView);

	if(rcTop.left<=rcView.left&&rcTop.right>=rcView.right&&\
		rcTop.top<=rcView.top&&rcTop.bottom>=rcView.bottom)
	{
		return FALSE;
	}

	return TRUE;*/
}

BOOL CModelViewerThread::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(!::IsWindow(pMsg->hwnd))
	{
	//	MessageBox(NULL,"Invalidate hwnd!","Info",MB_OK);
		return FALSE;
	}

	return CWinThread::PreTranslateMessage(pMsg);
}
