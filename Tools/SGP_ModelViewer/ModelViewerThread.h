#pragma once



// CModelViewerThread
class CMainFrame;
class CModelViewerDoc;

class CModelViewerThread : public CWinThread
{
	DECLARE_DYNCREATE(CModelViewerThread)
	friend class ExportControlClass;
protected:
	CModelViewerThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CModelViewerThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnIdle(LONG lCount);
	BOOL IsRenderable();// whether should render
private:
	CMainFrame* m_pFrame;
	CModelViewerDoc* m_pDoc;
	CSingleDocTemplate* m_pDocTemplate;

	ULONG_PTR m_GdiplusToken;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


