
// SGP_WorldEditor.h : SGP_WorldEditor 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CSGP_WorldEditorApp:
// 有关此类的实现，请参阅 SGP_WorldEditor.cpp
//

class CSGP_WorldEditorApp : public CWinAppEx
{
public:
	CSGP_WorldEditorApp();

	HMODULE m_hDllModelViewer;
// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	BOOL CheckStartup();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(LONG lCount);
	BOOL IsRenderable();
private:
	ULONG_PTR m_Token;
};

extern CSGP_WorldEditorApp theApp;
