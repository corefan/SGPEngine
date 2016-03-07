#pragma once


// CResourceToolBar

class CResourceToolBar : public CMFCToolBar
{
	DECLARE_DYNAMIC(CResourceToolBar)

public:
	CResourceToolBar();
	virtual ~CResourceToolBar();
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}
	virtual BOOL AllowShowOnList() const { return TRUE; }

protected:
	DECLARE_MESSAGE_MAP()
};
