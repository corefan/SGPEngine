// SGP_ModelViewer.h : SGP_ModelViewer DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CSGP_ModelViewerApp
// 有关此类实现的信息，请参阅 SGP_ModelViewer.cpp
//

class CSGP_ModelViewerApp : public CWinAppEx
{
	friend class CMainFrame;
public:
	CSGP_ModelViewerApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
private:
	UINT  m_nAppLook;
};

extern CSGP_ModelViewerApp theApp;