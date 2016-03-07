// ModelControlPane.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ModelControlPane.h"


// CModelControlPane

IMPLEMENT_DYNAMIC(CModelControlPane, /*CDockablePane*/CCommonDockPane)

CModelControlPane::CModelControlPane()
{

}

CModelControlPane::~CModelControlPane()
{
}


BEGIN_MESSAGE_MAP(CModelControlPane, /*CDockablePane*/CCommonDockPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CModelControlPane ��Ϣ�������




int CModelControlPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (/*CDockablePane*/CCommonDockPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	m_ModelControlDlg.Create(IDD_MODELCONTROL_DLG,this);
	m_ModelControlDlg.ShowWindow(SW_SHOW);

	return 0;
}


void CModelControlPane::OnSize(UINT nType, int cx, int cy)
{
	/*CDockablePane*/CCommonDockPane::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	CRect rect;
	GetClientRect(rect);
	if(m_ModelControlDlg.m_hWnd!=NULL) m_ModelControlDlg.MoveWindow(rect);
}
