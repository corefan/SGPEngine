#pragma once

enum PaneShowMode
{
	ShowMode_Show,
	ShowMode_AutoHide,
	ShowMode_Hide
};

// CCommonDockablePane

class CCommonDockablePane : public CDockablePane
{
	DECLARE_DYNAMIC(CCommonDockablePane)

public:
	CCommonDockablePane();
	virtual ~CCommonDockablePane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	void EnterFullScreen();
	void ExitFullScreen();
private:
	PaneShowMode m_PrevShowMode;
};


