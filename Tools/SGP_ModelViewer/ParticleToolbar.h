#pragma once


// CParticleToolbar

class CParticleToolbar : public CMFCToolBar
{
	DECLARE_DYNAMIC(CParticleToolbar)

public:
	CParticleToolbar();
	virtual ~CParticleToolbar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
		//	CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), FALSE);
	}
	virtual BOOL AllowShowOnList() const { return TRUE; }
};


