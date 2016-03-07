#pragma once


// CLogViewerToolBar

class CLogViewerToolBar : public CMFCToolBar
{
	DECLARE_DYNAMIC(CLogViewerToolBar)

public:
	CLogViewerToolBar();
	virtual ~CLogViewerToolBar();
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}
	virtual BOOL AllowShowOnList() const { return TRUE; }
protected:
	DECLARE_MESSAGE_MAP()
};
