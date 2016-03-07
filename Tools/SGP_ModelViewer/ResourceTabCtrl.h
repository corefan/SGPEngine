#pragma once


// CResourceTabCtrl
#include <vector>
#include "ResourceTreeCtrl.h"

class CResourceTabCtrl : public CMFCTabCtrl
{
	DECLARE_DYNAMIC(CResourceTabCtrl)

public:
	CResourceTabCtrl();
	virtual ~CResourceTabCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	std::vector<CString> m_PathVector;
	std::vector<CResourceTreeCtrl*> m_TreeVector;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


