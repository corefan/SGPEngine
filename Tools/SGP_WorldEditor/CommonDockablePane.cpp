// CommonDockablePane.cpp : 实现文件
//

#include "stdafx.h"
#include "CommonDockablePane.h"

// CCommonDockablePane

IMPLEMENT_DYNAMIC(CCommonDockablePane, CDockablePane)

CCommonDockablePane::CCommonDockablePane()
{

}

CCommonDockablePane::~CCommonDockablePane()
{
}

BEGIN_MESSAGE_MAP(CCommonDockablePane, CDockablePane)
END_MESSAGE_MAP()

// CCommonDockablePane 消息处理程序

void CCommonDockablePane::EnterFullScreen()
{
	if(IsVisible())
	{
		ShowPane(FALSE,FALSE,FALSE);
		m_PrevShowMode=ShowMode_Show;
	}
	else if(IsAutoHideMode())
	{
		OnPressCloseButton();
		m_PrevShowMode=ShowMode_AutoHide;
	}
	else m_PrevShowMode=ShowMode_Hide;
}

void CCommonDockablePane::ExitFullScreen()
{
	if(m_PrevShowMode==ShowMode_Show) ShowPane(TRUE,FALSE,FALSE);
	else if(m_PrevShowMode==ShowMode_AutoHide)
	{
		ShowPane(TRUE,FALSE,FALSE);
	//	ToggleAutoHide();
	}
}