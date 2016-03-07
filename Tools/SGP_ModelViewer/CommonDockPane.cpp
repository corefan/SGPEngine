// CommonDockPane.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "CommonDockPane.h"


// CCommonDockPane

IMPLEMENT_DYNAMIC(CCommonDockPane, CDockablePane)

CCommonDockPane::CCommonDockPane()
{

}

CCommonDockPane::~CCommonDockPane()
{
}


BEGIN_MESSAGE_MAP(CCommonDockPane, CDockablePane)
END_MESSAGE_MAP()

#define IDM_COMMON_FLOAT 30001
#define IDM_COMMON_DOCKING 30002
#define IDM_COMMON_AUTOHIDE	30003
#define IDM_COMMON_HIDE 30004
#define IDM_COMMON_TABBED 30005
// CCommonDockPane 消息处理程序

BOOL CCommonDockPane::OnShowControlBarMenu(CPoint point)
{
	if ((GetEnabledAlignment() & CBRS_ALIGN_ANY) == 0 && !CanFloat())
	{
		return FALSE;
	}

	CMenu menu;
	menu.CreatePopupMenu();

	CString strItem;

	ENSURE(strItem.LoadString(IDS_AFXBARRES_FLOATING));
	menu.AppendMenu(MF_STRING, IDM_COMMON_FLOAT, strItem);

	ENSURE(strItem.LoadString(IDS_AFXBARRES_DOCKING));
	menu.AppendMenu(MF_STRING, IDM_COMMON_DOCKING, strItem);

	ENSURE(strItem.LoadString(IDS_AFXBARRES_TABBED));
	menu.AppendMenu(MF_STRING,IDM_COMMON_TABBED , strItem);

	ENSURE(strItem.LoadString(IDS_AFXBARRES_AUTOHIDE));
	menu.AppendMenu(MF_STRING,IDM_COMMON_AUTOHIDE , strItem);

	ENSURE(strItem.LoadString(IDS_AFXBARRES_HIDE));
	menu.AppendMenu(MF_STRING,IDM_COMMON_HIDE , strItem);

	if (!CanFloat())
		menu.EnableMenuItem(IDM_COMMON_FLOAT, MF_GRAYED);

	if (!CanAutoHide() || GetParentMiniFrame() != NULL)
		menu.EnableMenuItem(IDM_COMMON_AUTOHIDE, MF_GRAYED);

	if (IsAutoHideMode())
	{
		menu.EnableMenuItem(IDM_COMMON_FLOAT, MF_GRAYED);
		menu.EnableMenuItem(IDM_COMMON_DOCKING, MF_GRAYED);
		menu.CheckMenuItem(IDM_COMMON_AUTOHIDE, MF_CHECKED);
		menu.EnableMenuItem(IDM_COMMON_HIDE, MF_GRAYED);
	}

	CMDIFrameWndEx* pFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetDockSiteFrameWnd());
	if (!CanBeTabbedDocument() || pFrame != NULL && pFrame->IsFullScreen())
	{
		menu.EnableMenuItem(IDM_COMMON_TABBED, MF_GRAYED);
	}

	if (IsMDITabbed())
	{
		menu.EnableMenuItem(IDM_COMMON_FLOAT, MF_GRAYED);
		menu.EnableMenuItem(IDM_COMMON_DOCKING, MF_GRAYED);
		menu.CheckMenuItem(IDM_COMMON_TABBED, MF_CHECKED);
	}

	if (IsFloating())
	{
		menu.CheckMenuItem(IDM_COMMON_FLOAT, MF_CHECKED);
	}
	else if (!IsAutoHideMode() && !IsMDITabbed())
	{
		menu.CheckMenuItem(IDM_COMMON_DOCKING, MF_CHECKED);
	}

	if ((GetEnabledAlignment() & CBRS_ALIGN_ANY) == 0)
	{
		menu.EnableMenuItem(IDM_COMMON_DOCKING, MF_GRAYED);
	}

	if (!CanBeClosed())
	{
		menu.EnableMenuItem(IDM_COMMON_HIDE, MF_GRAYED);
	}

	if (!OnBeforeShowPaneMenu(menu))
	{
		return FALSE;
	}

//	HWND hwndThis = GetSafeHwnd();

/*	int nMenuResult = afxContextMenuManager->TrackPopupMenu(
		menu, point.x, point.y, this);*/
	menu.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);

	HWND hwndThis = GetSafeHwnd();
	if (!::IsWindow(hwndThis))
	{
		return TRUE;
	}

//	if (!OnAfterShowPaneMenu(nMenuResult))
//	{
//		return TRUE;
//	}

	return TRUE;
}

BOOL CCommonDockPane::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDM_COMMON_FLOAT:
		{
			BOOL bWasFloated = FALSE;

			CBaseTabbedPane* pTabbedBar =DYNAMIC_DOWNCAST(CBaseTabbedPane,(IsTabbed() ? GetParentTabbedPane() : (CBaseTabbedPane*)this));
		/*	CBaseTabbedPane* pTabbedBar;
			if(IsTabbed()) pTabbedBar=(CBaseTabbedPane*)GetParentTabbedPane();
			else pTabbedBar=(CBaseTabbedPane*)this;*/

		//	CBaseTabbedPane* pTabbedBar=(CBaseTabbedPane*)AfxDynamicDownCast((RUNTIME_CLASS(CBaseTabbedPane)),IsTabbed()?GetParentTabbedPane():this);

			if (pTabbedBar != NULL)
			{
				ASSERT_VALID(pTabbedBar);

				CMFCBaseTabCtrl* pTabWnd = pTabbedBar->GetUnderlyingWindow();
				if (pTabWnd != NULL)
				{
					ASSERT_VALID(pTabWnd);

					const int nTabID = pTabWnd->GetActiveTab();
					CWnd* pWnd = pTabWnd->GetTabWnd(nTabID);

					if (pWnd != NULL && pTabWnd->IsTabDetachable(nTabID))
					{
						bWasFloated = pTabbedBar->DetachPane(pWnd, FALSE);
						if (bWasFloated)
						{
							if (pTabWnd->GetTabsNum() > 0 &&
								pTabWnd->GetVisibleTabsNum() == 0)
							{
								pTabbedBar->ShowPane(FALSE, FALSE, FALSE);
							}
						}
					}
				}
			}

			if (!bWasFloated)
			{
				FloatPane(m_recentDockInfo.m_rectRecentFloatingRect);
			}
		}
		break;
	case IDM_COMMON_DOCKING:
		if (IsFloating())
		{
			CPaneFrameWnd* pMiniFrame = GetParentMiniFrame();
			if (pMiniFrame != NULL)
			{
				pMiniFrame->OnDockToRecentPos();
			}
		}
		break;
	case IDM_COMMON_AUTOHIDE:
		ToggleAutoHide();
		break;
	case IDM_COMMON_HIDE:
		OnPressCloseButton();
		break;
	case IDM_COMMON_TABBED:
		if (IsMDITabbed())
		{
			CMDIChildWndEx* pMDIChild = DYNAMIC_DOWNCAST(CMDIChildWndEx, GetParent());
			if (pMDIChild == NULL)
			{
				ASSERT(FALSE);
				return FALSE;
			}

			CMDIFrameWndEx* pTabbedFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetDockSiteFrameWnd());
			if (pTabbedFrame == NULL)
			{
				ASSERT(FALSE);
				return FALSE;
			}

			pTabbedFrame->TabbedDocumentToControlBar(pMDIChild);
		}
		else
		{
			ConvertToTabbedDocument();
		}
		break;
	default:
		break;
	}

	return CDockablePane::OnCommand(wParam, lParam);
}

