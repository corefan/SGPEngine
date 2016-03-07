#pragma once


// CObjLibToolbar

class CObjLibToolbar : public CMFCToolBar
{
	DECLARE_DYNAMIC(CObjLibToolbar)

public:
	CObjLibToolbar();
	virtual ~CObjLibToolbar();

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}
	virtual BOOL AllowShowOnList() const { return TRUE; }
protected:
	DECLARE_MESSAGE_MAP()
};
