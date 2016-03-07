#pragma once

#include "LogViewerEdit.h"
// CLogViewerPane

class CLogViewerPane : public CCommonDockablePane
{
	DECLARE_DYNAMIC(CLogViewerPane)

public:
	CLogViewerPane();
	virtual ~CLogViewerPane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	CLogViewerEdit m_RichEdit;
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
};


