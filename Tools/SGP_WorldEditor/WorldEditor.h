
// SGP_WorldEditor.h : SGP_WorldEditor Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CSGP_WorldEditorApp:
// �йش����ʵ�֣������ SGP_WorldEditor.cpp
//

class CSGP_WorldEditorApp : public CWinAppEx
{
public:
	CSGP_WorldEditorApp();

	HMODULE m_hDllModelViewer;
// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
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
