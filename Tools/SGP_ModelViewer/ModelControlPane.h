#pragma once

#include "ModelControlDlg.h"
#include "CommonDockPane.h"
// CModelControlPane

class CModelControlPane : public /*CDockablePane*/CCommonDockPane
{
	DECLARE_DYNAMIC(CModelControlPane)

public:
	CModelControlPane();
	virtual ~CModelControlPane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	CModelControlDlg m_ModelControlDlg;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


