// SceneEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "SceneEditorDlg.h"
#include "afxdialogex.h"

#define DLG_MINWIDTH 260

// CSceneEditorDlg 对话框

IMPLEMENT_DYNAMIC(CSceneEditorDlg, CDialogEx)

CSceneEditorDlg::CSceneEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSceneEditorDlg::IDD, pParent)
{

}

CSceneEditorDlg::~CSceneEditorDlg()
{
}

void CSceneEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSceneEditorDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

// CSceneEditorDlg 消息处理程序

BOOL CSceneEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ObjectDlg.Create(IDD_SCENE_OBJECT,this);
	m_ObjectDlg.ShowWindow(SW_SHOW);
	m_SkydomeDlg.Create(IDD_SKYDOME_SETTING,this);
	m_SkydomeDlg.ShowWindow(SW_SHOW);
	m_ObjEditorDlg.Create(IDD_OBJECT_EDITOR,this);
	m_ObjEditorDlg.ShowWindow(SW_SHOW);

	// Init Scroll Info
	ModifyStyle(0,WS_VSCROLL|WS_HSCROLL);
	SCROLLINFO si;
	si.cbSize=sizeof(SCROLLINFO);
	si.fMask=SIF_POS;
	si.nPos=0;
	SetScrollInfo(SB_VERT,&si);
	SetScrollInfo(SB_HORZ,&si);

	return TRUE;
}

void CSceneEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ObjectDlg.m_hWnd!=NULL&&m_SkydomeDlg.m_hWnd!=NULL)
	{
		SetCtrlPos();
	}
}

void CSceneEditorDlg::SetCtrlPos()
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
	m_SkydomeDlg.GetClientRect(rc);
	height+=rc.Height()+1;
	m_ObjectDlg.GetClientRect(rc);
	height+=rc.Height()+1;
	m_ObjEditorDlg.GetClientRect(rc);
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

		m_SkydomeDlg.GetClientRect(rc);
		rcCtrl.SetRect(0,top,width,rc.Height());
		rcCtrl.OffsetRect(-xPos,-yPos);
		m_SkydomeDlg.MoveWindow(rcCtrl);
		top+=rc.Height()+1;

		m_ObjectDlg.GetClientRect(rc);
		rcCtrl.SetRect(0,top,width,top+rc.Height());
		rcCtrl.OffsetRect(-xPos,-yPos);
		m_ObjectDlg.MoveWindow(rcCtrl);
		top+=rc.Height()+1;

		m_ObjEditorDlg.GetClientRect(rc);
		rcCtrl.SetRect(0,top,width,top+rc.Height());
		rcCtrl.OffsetRect(-xPos,-yPos);
		m_ObjEditorDlg.MoveWindow(rcCtrl);
		top+=rc.Height()+1;
	}
}

void CSceneEditorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CSceneEditorDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

BOOL CSceneEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	if(pMsg->message == WM_MOUSEWHEEL)
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

void CSceneEditorDlg::EnableAllCtrls(BOOL bEnable)
{
	m_SkydomeDlg.EnableAllCtrls(bEnable);
	m_ObjectDlg.EnableAllCtrls(bEnable);
	m_ObjEditorDlg.EnableAllCtrls(bEnable);
}