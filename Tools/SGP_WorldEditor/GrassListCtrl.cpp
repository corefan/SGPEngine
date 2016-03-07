// GrassListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "GrassListCtrl.h"
#include "TerrainGrassDlg.h"

// CGrassListCtrl

IMPLEMENT_DYNAMIC(CGrassListCtrl, CListCtrl)

CGrassListCtrl::CGrassListCtrl()
{

}

CGrassListCtrl::~CGrassListCtrl()
{
}

BEGIN_MESSAGE_MAP(CGrassListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CGrassListCtrl 消息处理程序

void CGrassListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonDown(nFlags, point);

	UINT uSelectedCount=GetSelectedCount();
	if(uSelectedCount==1)
	{
		int nSelectedItem = GetNextItem(-1, LVNI_SELECTED);
		ASSERT(nSelectedItem != -1);

		CRect rc;
		if(GetItemRect(nSelectedItem,rc,LVIR_BOUNDS))
		{
			if(rc.PtInRect(point))
				((CTerrainGrassDlg*)GetParent())->SetTextureIndex(nSelectedItem);
			else
				((CTerrainGrassDlg*)GetParent())->SetTextureIndex(-1);
		}
	}
	else ((CTerrainGrassDlg*)GetParent())->SetTextureIndex(-1);
}
