// NewMapLoadingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "NewMapLoadingDlg.h"
#include "afxdialogex.h"

#include "..\SGPLibraryCode\SGPHeader.h"
#include "WorldMapManager.h"

// CNewMapLoadingDlg 对话框

IMPLEMENT_DYNAMIC(CNewMapLoadingDlg, CDialogEx)

CNewMapLoadingDlg::CNewMapLoadingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNewMapLoadingDlg::IDD, pParent)
{

}

CNewMapLoadingDlg::~CNewMapLoadingDlg()
{
}

void CNewMapLoadingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNewMapLoadingDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_MESSAGE(WM_KICKIDLE,OnKickIdle)
END_MESSAGE_MAP()

// CNewMapLoadingDlg 消息处理程序

BOOL CNewMapLoadingDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

HBRUSH CNewMapLoadingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	int nID=pWnd->GetDlgCtrlID();
	if(nID==IDC_TITLE)
	{
		pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkMode(TRANSPARENT);
		if(m_BorderBrush.m_hObject==NULL)
			m_BorderBrush.CreateSolidBrush(RGB(73,170,249));
		return m_BorderBrush;
	}

	return hbr;
}

void CNewMapLoadingDlg::OnPaint()
{
	CDialogEx::OnPaint();

	COLORREF borderColor=RGB(73,170,249);

	CDC* pDC=GetDC();
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc.left,rc.top,rc.Width(),rc.Height(),borderColor);
	ReleaseDC(pDC);
}

LRESULT CNewMapLoadingDlg::OnKickIdle(WPARAM wParam,LPARAM lParam)
{
	WorldMapManager::GetInstance()->CreateMap(m_MapName,m_TerrainSize,m_bUsePerlinNoise,m_TerrainMaxHeight,m_Layer0TexPath);
	SendMessage(WM_CLOSE);
	return TRUE;
}
