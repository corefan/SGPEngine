// SGP_ModelViewer.h : SGP_ModelViewer DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSGP_ModelViewerApp
// �йش���ʵ�ֵ���Ϣ������� SGP_ModelViewer.cpp
//

class CSGP_ModelViewerApp : public CWinAppEx
{
	friend class CMainFrame;
public:
	CSGP_ModelViewerApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
private:
	UINT  m_nAppLook;
};

extern CSGP_ModelViewerApp theApp;