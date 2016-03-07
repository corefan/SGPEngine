#pragma once

// CCommonDockPane

class CCommonDockPane : public CDockablePane
{
	DECLARE_DYNAMIC(CCommonDockPane)

public:
	CCommonDockPane();
	virtual ~CCommonDockPane();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnShowControlBarMenu(CPoint point);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

};
