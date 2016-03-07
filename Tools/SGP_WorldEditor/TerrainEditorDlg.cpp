// TerrainEditorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "TerrainEditorDlg.h"
#include "afxdialogex.h"

#define DLG_MINWIDTH 260

// CTerrainEditorDlg �Ի���

IMPLEMENT_DYNAMIC(CTerrainEditorDlg, CDialogEx)

CTerrainEditorDlg::CTerrainEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTerrainEditorDlg::IDD, pParent)
{
	m_nButtonState=-1;
}

CTerrainEditorDlg::~CTerrainEditorDlg()
{
}

void CTerrainEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTerrainEditorDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

// CTerrainEditorDlg ��Ϣ�������


BOOL CTerrainEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_HeightDlg.Create(IDD_TERRAIN_HEIGHT,this);
	m_HeightDlg.ShowWindow(SW_SHOW);
	m_TerrainTexDlg.Create(IDD_TERRAIN_TEXTURE,this);
	m_TerrainTexDlg.ShowWindow(SW_SHOW);
	m_TerrainGrassDlg.Create(IDD_TERRAIN_GRASS,this);
	m_TerrainGrassDlg.ShowWindow(SW_SHOW);
	m_TerrainWaterDlg.Create(IDD_TERRAIN_WATER,this);
	m_TerrainWaterDlg.ShowWindow(SW_SHOW);

	// Init Scroll Info
	ModifyStyle(0,WS_VSCROLL|WS_HSCROLL);
	SCROLLINFO si;
	si.cbSize=sizeof(SCROLLINFO);
	si.fMask=SIF_POS;
	si.nPos=0;
	SetScrollInfo(SB_VERT,&si);
	SetScrollInfo(SB_HORZ,&si);

	EnableAllCtrls(FALSE);

	return TRUE;
}


BOOL CTerrainEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	if(pMsg->message == WM_MOUSEWHEEL&&pMsg->hwnd!=m_TerrainTexDlg.m_TextureList.m_hWnd)
	{
		short zDelta = GET_WHEEL_DELTA_WPARAM(pMsg->wParam);
		if(zDelta > 0)
		{
			SendMessage(WM_VSCROLL,MAKEWPARAM(SB_LINEUP,0));
			SendMessage(WM_VSCROLL,MAKEWPARAM(SB_LINEUP,0));
		}
		else
		{
			SendMessage(WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,0));
			SendMessage(WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,0));
		}
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CTerrainEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_HeightDlg.m_hWnd!=NULL&&m_TerrainTexDlg.m_hWnd!=NULL&&m_TerrainGrassDlg.m_hWnd!=NULL)
		SetCtrlPos();
}

void CTerrainEditorDlg::SetCtrlPos()
{
	CRect rcClient;
	GetClientRect(rcClient);
	int cx=rcClient.Width(),cy=rcClient.Height();

	SCROLLINFO si;
	si.cbSize=sizeof(si);

	// Set Horizontal Scrollbar Info
	GetScrollInfo(SB_HORZ,&si);
	si.nMin=0;
	if(cx<=DLG_MINWIDTH)
	{
		si.nPage=cx;
		si.nMax=DLG_MINWIDTH;
	}
	else
	{
		si.nPage=cx+1;
		si.nMax=cx;
	}
	if(si.nMax<=(int)si.nPage) si.nPos=0;
	else if(si.nPos>si.nMax-(int)si.nPage) si.nPos=si.nMax-si.nPage;
	int xPos=si.nPos;
	si.fMask=SIF_PAGE|SIF_RANGE|SIF_POS;
	SetScrollInfo(SB_HORZ,&si);

	// Get Sub Dialog Info
	int height=0;
	CRect rc;
	m_HeightDlg.GetClientRect(rc);
	height+=rc.Height()+1;
	m_TerrainTexDlg.GetClientRect(rc);
	height+=rc.Height()+1;
	m_TerrainGrassDlg.GetClientRect(rc);
	height+=rc.Height()+1;
	m_TerrainWaterDlg.GetClientRect(rc);
	height+=rc.Height()+1;

	// Set Vertical Scrollbar Info
	GetScrollInfo(SB_VERT,&si);
	si.nPage=cy;
	si.nMax=height;
	if((int)si.nPage>=si.nMax) si.nPos=0;
	else if(si.nPos>si.nMax-(int)si.nPage) si.nPos=si.nMax-si.nPage;
	int yPos=si.nPos;
	si.fMask=SIF_PAGE|SIF_RANGE|SIF_POS;
	SetScrollInfo(SB_VERT,&si);

	int width=cx>DLG_MINWIDTH?cx:DLG_MINWIDTH;

	GetClientRect(rc);
	if(rc.Width()!=cx||rc.Height()!=cy) SetCtrlPos();
	else
	{
		// Set Ctrl Pos
		int top=0;
		CRect rcCtrl;

		m_HeightDlg.GetClientRect(rc);
		rcCtrl.SetRect(0,top,width,rc.Height());
		rcCtrl.OffsetRect(-xPos,-yPos);
		m_HeightDlg.MoveWindow(rcCtrl);
		top+=rc.Height()+1;

		m_TerrainTexDlg.GetClientRect(rc);
		rcCtrl.SetRect(0,top,width,top+rc.Height());
		rcCtrl.OffsetRect(-xPos,-yPos);
		m_TerrainTexDlg.MoveWindow(rcCtrl);
		top+=rc.Height()+1;

		m_TerrainGrassDlg.GetClientRect(rc);
		rcCtrl.SetRect(0,top,width,top+rc.Height());
		rcCtrl.OffsetRect(-xPos,-yPos);
		m_TerrainGrassDlg.MoveWindow(rcCtrl);
		top+=rc.Height()+1;

		m_TerrainWaterDlg.GetClientRect(rc);
		rcCtrl.SetRect(0,top,width,top+rc.Height());
		rcCtrl.OffsetRect(-xPos,-yPos);
		m_TerrainWaterDlg.MoveWindow(rcCtrl);
		top+=rc.Height()+1;
	}
}

void CTerrainEditorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO si;
	si.cbSize=sizeof(si);
	GetScrollInfo(SB_HORZ,&si);
	int pos;
	switch(nSBCode)
	{
	case SB_LEFT:
		ScrollWindow(si.nPos-si.nMin,0);
		si.nPos=si.nMin;
		SetScrollInfo(SB_HORZ,&si);
		break;
	case SB_RIGHT:
		ScrollWindow(si.nPos-si.nMax+si.nPage,0);
		si.nPos=si.nMax-si.nPage;
		SetScrollInfo(SB_HORZ,&si);
		break;
	case SB_LINELEFT:
		pos=si.nPos;
		si.nPos-=10;
		if(si.nPos<si.nMin) si.nPos=si.nMin;
		ScrollWindow(pos-si.nPos,0);
		SetScrollInfo(SB_HORZ,&si);
		break;
	case SB_LINERIGHT:
		pos=si.nPos;
		si.nPos+=si.nPage;
		if(si.nPos>si.nMax-(int)si.nPage) si.nPos=si.nMax-si.nPage;
		ScrollWindow(pos-si.nPos,0);
		SetScrollInfo(SB_HORZ,&si);
		break;
	case SB_PAGELEFT:
		pos=si.nPos;
		si.nPos-=si.nPage;
		if(si.nPos<si.nMin) si.nPos=si.nMin;
		ScrollWindow(pos-si.nPos,0);
		SetScrollInfo(SB_HORZ,&si);
		break;
	case SB_PAGERIGHT:
		pos=si.nPos;
		si.nPos+=10;
		if(si.nPos>si.nMax-(int)si.nPage) si.nPos=si.nMax-si.nPage;
		ScrollWindow(pos-si.nPos,0);
		SetScrollInfo(SB_HORZ,&si);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		ScrollWindow(si.nPos-nPos,0);
		si.nPos=nPos;
		SetScrollInfo(SB_HORZ,&si);
		break;
	default:
		break;
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CTerrainEditorDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO si;
	si.cbSize=sizeof(si);
	GetScrollInfo(SB_VERT,&si);

	int pos;
	// ScrollWindow(0,prev pos-current pos);

	switch(nSBCode)
	{
	case SB_TOP:
		ScrollWindow(0,si.nPos-si.nMin);
		si.nPos=si.nMin;
		SetScrollInfo(SB_VERT,&si);
		break;
	case SB_BOTTOM:
		ScrollWindow(0,si.nPos-si.nMax+si.nPage);
		si.nPos=si.nMax-si.nPage;
		SetScrollInfo(SB_VERT,&si);
		break;
	case SB_LINEUP:
		if((int)si.nPage<si.nMax)
		{
			pos=si.nPos;
			si.nPos-=15;
			if(si.nPos<si.nMin) si.nPos=si.nMin;
			ScrollWindow(0,pos-si.nPos);
			SetScrollInfo(SB_VERT,&si);
		}
		break;
	case SB_LINEDOWN:
		if((int)si.nPage<si.nMax)
		{
			pos=si.nPos;
			si.nPos+=15;
			if(si.nPos>si.nMax-(int)si.nPage) si.nPos=si.nMax-(int)si.nPage;
			ScrollWindow(0,pos-si.nPos);
			SetScrollInfo(SB_VERT,&si);
		}
		break;
	case SB_PAGEUP:
		pos=si.nPos;
		si.nPos-=si.nPage;
		if(si.nPos<si.nMin) si.nPos=si.nMin;
		ScrollWindow(0,pos-si.nPos);
		SetScrollInfo(SB_VERT,&si);
		break;
	case SB_PAGEDOWN:
		pos=si.nPos;
		si.nPos+=si.nPage;
		if(si.nPos>si.nMax-(int)si.nPage) si.nPos=si.nMax-si.nPage;
		ScrollWindow(0,pos-si.nPos);
		SetScrollInfo(SB_VERT,&si);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		pos=(int)nPos;
		ScrollWindow(0,si.nPos-pos);
		si.nPos=pos;
		SetScrollInfo(SB_VERT,&si);
		break;
	default:
		break;
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


LRESULT CTerrainEditorDlg::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(rc);
	ScreenToClient(&point);
	if(point.x>rc.right&&point.y<rc.bottom) return HTVSCROLL;
	if(point.y>rc.bottom&&point.x<rc.right) return HTHSCROLL;

	return CDialogEx::OnNcHitTest(point);
}

void CTerrainEditorDlg::EnableAllCtrls(BOOL bEnable)
{
	m_HeightDlg.EnableAllCtrls(bEnable);
	m_TerrainTexDlg.EnableAllCtrls(bEnable);
	m_TerrainGrassDlg.EnableAllCtrls(bEnable);
	m_TerrainWaterDlg.EnableAllCtrls(bEnable);
}

void CTerrainEditorDlg::SetButtonState(int nSelID)
{
	if(m_nButtonState!=nSelID)
	{
		m_HeightDlg.SetButtonState(nSelID);
		m_TerrainTexDlg.SetButtonState(nSelID);
		m_TerrainGrassDlg.SetButtonState(nSelID);
		m_TerrainWaterDlg.SetButtonState(nSelID);
		m_nButtonState=nSelID;
	}
}