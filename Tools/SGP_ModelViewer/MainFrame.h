#pragma once

#include "ResourcePane.h"
#include "ModelDetailPane.h"
#include "ModelControlPane.h"
#include "LogViewerPane.h"
#include "SettingPane.h"
#include "ParticleConfigPane.h"
// CMainFrame 框架
#define UM_RENDERINIT        WM_USER+0x02
#define UM_FRAMEUI_INIT      WM_USER+0x01

class CMainFrame : public CFrameWndEx
{
	DECLARE_DYNCREATE(CMainFrame)
public:
	CMainFrame();           // 动态创建所使用的受保护的构造函数
	virtual ~CMainFrame();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMenuView(UINT nID);
	afx_msg void OnUpdateMenuView(CCmdUI *pCmdUI);
	afx_msg void OnMenuSetBkgndColor();
	afx_msg void OnViewApplicationLook(UINT nID);
	afx_msg void OnUpdateApplicationLook(CCmdUI *pCmdUI);
	afx_msg LRESULT InitRender(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT InitFrameUI(WPARAM wParam,LPARAM lParam);
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileClose();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileClose(CCmdUI *pCmdUI);
public:
	CMFCMenuBar m_MenuBar;//menu
	CResourcePane m_ResourcePane;//resource pane
	CModelDetailPane m_ModelDetailPane;//model detail pane
	CModelControlPane m_ModelControlPane;//model control pane
	CLogViewerPane m_LogViewerPane;//log viewer pane
	CSettingPane m_SettingPane;//setting pane
	CParticleConfigPane m_ParticlePane;

	CMFCStatusBar m_StatusBar;
//	CMFCRibbonStatusBar m_StatusBar;

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnDestroy();

	CFont m_Font;
	int m_nFlashCount;

	BOOL GetSelectedResourceTexture(CString& texturePath);
	afx_msg void OnClose();

	BOOL m_bModified;

	void SetModifiedFlag(){m_bModified=TRUE;}
	void ClearModifiedFlag(){m_bModified=FALSE;}
	virtual BOOL OnCloseDockingPane(CDockablePane* pPane);
	virtual BOOL OnCloseMiniFrame(CPaneFrameWnd* pWnd);
	CString m_MF1Path;
	CString m_MF1FileName;
	void SetMF1File(CString strPath,CString strName){m_MF1Path=strPath;m_MF1FileName=strName;}
	void SaveMF1File(CString strPath,CString strName);
	void PreSaveMF1File();
	void PreCloseModel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};