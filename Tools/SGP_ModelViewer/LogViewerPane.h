#pragma once


// CLogViewerPane
#include "LogViewerToolBar.h"
#include "LogViewerEdit.h"
#include "CommonDockPane.h"

class CLogViewerPane : public /*CDockablePane*/CCommonDockPane
{
	DECLARE_DYNAMIC(CLogViewerPane)

public:
	CLogViewerPane();
	virtual ~CLogViewerPane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	CLogViewerToolBar m_ToolBar;
	CLogViewerEdit m_RichEdit;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


