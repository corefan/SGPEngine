#pragma once

#include "FindResultList.h"

// CFindResultPane

class CFindResultPane : public CCommonDockablePane
{
	DECLARE_DYNAMIC(CFindResultPane)

public:
	CFindResultPane();
	virtual ~CFindResultPane();

protected:
	DECLARE_MESSAGE_MAP()
private:
	CFindResultList m_ResultList;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void FindObjects();
};
