
// MainFrm.h : CMainFrame 类的接口
//
#pragma once

#define UM_RENDERINIT        WM_USER+0x02
#include ".\LogViewer Pane\LogViewerPane.h"
#include ".\Scene Explorer\SceneExplorerPane.h"
#include ".\Setting Pane\SettingPane.h"
#include "EditorPane.h"
#include "ObjAttributePane.h"
#include "FindResultPane.h"
#include <vector>

#define RECENT_FILES_START	3000
#define RECENT_FILES_END	3009

class CMainFrame : public CFrameWndEx
{
	static CMainFrame* s_pMainFrame;
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
/*	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCToolBar		  m_SceneToolBar;*/
	CMFCToolBarImages m_UserImages;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnModuleModelViewer();
	afx_msg void OnUpdateModuleModelViewer(CCmdUI* pCmdUI);
	afx_msg void OnMenuView(UINT nID);
	afx_msg void OnUpdateMenuView(CCmdUI *pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnFileClose();
	afx_msg void OnUpdateFileClose(CCmdUI *pCmdUI);
	afx_msg void OnToolsBuildLightmap();
	afx_msg void OnUpdateToolsBuildLightmap(CCmdUI *pCmdUI);
	afx_msg void OnToolsPackResForMultiPlatform();
	afx_msg void OnEditFind();

	afx_msg LRESULT InitRender(WPARAM wParam,LPARAM lParam);
	
	afx_msg void OnUpdateEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRecentFiles(CCmdUI *pCmdUI);
	afx_msg void OnRecentFiles(UINT nID);
	afx_msg void OnMenuSceneMove();
	afx_msg void OnUpdateMenuSceneMove(CCmdUI* pCmdUI);
	afx_msg void OnMenuSceneRotate();
	afx_msg void OnUpdateMenuSceneRotate(CCmdUI* pCmdUI);
	afx_msg void OnMenuSceneScale();
	afx_msg void OnUpdateMenuSceneScale(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusSelectedObjNum(CCmdUI* pCmdUI);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnShowPopupMenu(CMFCPopupMenu* pMenuPopup);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnViewFullscreen();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
public:
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCToolBar		  m_SceneToolBar;
	CMFCStatusBar m_StatusBar;
	CSceneExplorerPane m_SceneExplorerPane;
//	CTerrainEditorPane m_TerrainEditorPane;
	CLogViewerPane m_LogViewerPane;
	CSettingPane m_SettingPane;
	CEditorPane m_EditorPane;
	CObjAttributePane m_ObjAttrPane;
	CFindResultPane m_FindResultPane;
	CFont m_Font;
	HACCEL m_hAccel;
	std::vector<CString> m_RecentFilesVector;
public:
	void ReleaseAllStaticInstance();
	static void OpenModelViewer(CString modelPath="");
	static CMainFrame* GetInstance(){ASSERT(s_pMainFrame!=NULL);return s_pMainFrame;}
	void SetMapName(CString mapName);

	WINDOWPLACEMENT m_OldWndPlacement;
	BOOL m_bFullScreen;
	CRect m_FullScreenRect;
	void ExitFullScreen();
	void EnterFullScreen();
};
