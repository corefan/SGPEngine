#pragma once

#include "ParticleConfigDlg.h"
#include "CommonDockPane.h"
#include "ParticleToolbar.h"

namespace sgp
{
	struct ParticleSystemParam;
}

// CParticleConfigPane

class CParticleConfigPane : public /*CDockablePane*/CCommonDockPane
{
	DECLARE_DYNAMIC(CParticleConfigPane)

public:
	CParticleConfigPane();
	virtual ~CParticleConfigPane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnParticleApply(bool* bShowFlag=NULL);

	void OnCloseParticlePane(BOOL bHide=TRUE);
	void LoadFromData(const sgp::ParticleSystemParam& data);
	void SetCtrlPos();
	void SetParticleIndex(int index){m_Index=index;}
	void CheckSaveOperation();
private:
	int m_Index;
	CParticleConfigDlg* m_pConfigDlg;
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
