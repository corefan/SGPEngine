#ifndef __SGP_MAX9CtrlPanel__H
#define __SGP_MAX9CtrlPanel__H

#include "SGP_MAX9Plugins.h"

class SGP_MaxInterface;


class CSGPMAX9CtrlPanel
{
public:
	CSGPMAX9CtrlPanel();
	~CSGPMAX9CtrlPanel();

	void					Create( HINSTANCE hInstance, HWND hPanel );
	void					ShutDown();
	SGP_MaxInterface*		GetMaxIP() { return m_pMaxInterface; }

	HWND					m_hCtrlPanel;
	bool					m_bRunning;

protected:
	// current selected node
	int						m_nSelId;
	SGP_MaxInterface*		m_pMaxInterface;
};

extern CSGPMAX9CtrlPanel g_SGPMax9CtrlPanel;

#endif		// __SGP_MAX9CtrlPanel__H