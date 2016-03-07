// ImageShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "ImageShowDlg.h"
#include "afxdialogex.h"

#include "TextureInfoDlg.h"
// CImageShowDlg 对话框

IMPLEMENT_DYNAMIC(CImageShowDlg, CDialogEx)

CImageShowDlg::CImageShowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageShowDlg::IDD, pParent)
{
	m_bInitialized=FALSE;
	m_PicWidth=-1;
	m_PicHeight=-1;
}

CImageShowDlg::~CImageShowDlg()
{
}

void CImageShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageShowDlg, CDialogEx)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CImageShowDlg 消息处理程序

BOOL CImageShowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ModifyStyle(0,WS_VSCROLL|WS_HSCROLL);

	m_Image.SubclassDlgItem(IDC_IMAGE_STATIC,this);
	CRect rc;
	GetClientRect(rc);
	m_Image.MoveWindow(rc);

	return TRUE;
}

void CImageShowDlg::SetScrollSize(int width,int height)
{
	if(m_PicWidth==width&&m_PicHeight==height)
	{
		Invalidate();
		m_Image.Invalidate();
		return ;
	}

	if(m_bInitialized)
	{
		OnHScroll(SB_LEFT,0,NULL);
		OnVScroll(SB_TOP,0,NULL);
	}

	CRect rc;
	GetClientRect(rc);
	SCROLLINFO si;
	si.cbSize=sizeof(si);
	si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
	si.nMin=0;
	si.nPos=0;
	// horizontal scrollbar
	si.nMax=width;
	si.nPage=rc.Width();
	SetScrollInfo(SB_HORZ,&si,TRUE);
	// vertical scrollbar
	si.nMax=height;
	si.nPage=rc.Height();
	SetScrollInfo(SB_VERT,&si,TRUE);

	CRect rc1;
	GetClientRect(rc1);
	if(rc.Width()!=rc1.Width()||rc.Height()!=rc1.Height())
	{
		si.fMask=SIF_PAGE;
		if(rc1.Width()!=rc.Width())
		{
			si.nPage=rc1.Width();
			SetScrollInfo(SB_HORZ,&si,TRUE);
		}
		if(rc1.Height()!=rc.Height())
		{
			si.nPage=rc1.Height();
			SetScrollInfo(SB_VERT,&si,TRUE);
		}

		CRect rc2;
		GetClientRect(rc2);
		if(rc2.Width()!=rc1.Width()||rc2.Height()!=rc1.Height())
		{
			if(rc2.Width()!=rc1.Width())
			{
				si.nPage=rc2.Width();
				SetScrollInfo(SB_HORZ,&si,TRUE);
			}
			if(rc2.Height()!=rc1.Height())
			{
				si.nPage=rc2.Height();
				SetScrollInfo(SB_VERT,&si,TRUE);
			}
		}
	}
	
	m_bInitialized=TRUE;

	Invalidate();
	m_Image.MoveWindow(0,0,width,height);
	m_Image.Invalidate();

	// store current picture size
	m_PicWidth=width;
	m_PicHeight=height;
}


void CImageShowDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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


void CImageShowDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
		pos=si.nPos;
		si.nPos-=10;
		if(si.nPos<si.nMin) si.nPos=si.nMin;
		ScrollWindow(0,pos-si.nPos);
		SetScrollInfo(SB_VERT,&si);
		break;
	case SB_LINEDOWN:
		pos=si.nPos;
		si.nPos+=10;
		if(si.nPos>si.nMax-(int)si.nPage) si.nPos=si.nMax-si.nPage;
		ScrollWindow(0,pos-si.nPos);
		SetScrollInfo(SB_VERT,&si);
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

LRESULT CImageShowDlg::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(rc);
	CPoint pt=point;
	ScreenToClient(&pt);

	// vertical scrollbar
	if(pt.x>rc.Width()&&pt.y<rc.Height()) return HTVSCROLL;
	// horizontal scrollbar
	if(pt.x<rc.Width()&&pt.y>rc.Height()) return HTHSCROLL;

	return CDialogEx::OnNcHitTest(point);
}

void CImageShowDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	GetParent()->PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CImageShowDlg::SliderScroll(int cx,int cy)
{
	SCROLLINFO si;
	si.cbSize=sizeof(si);	
	GetScrollInfo(SB_HORZ,&si);
	
	if(si.nMax>(int)si.nPage)
	{
		si.cbSize=sizeof(si);
		si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
		int prevX=si.nPos;
		si.nPos-=cx;
		if(si.nPos<si.nMin) si.nPos=si.nMin;
		if(si.nPos>si.nMax-(int)si.nPage) si.nPos=si.nMax-(int)si.nPage;
		ScrollWindow(prevX-si.nPos,0);
		SetScrollInfo(SB_HORZ,&si);
	}
	
	GetScrollInfo(SB_VERT,&si);
	if(si.nMax>(int)si.nPage)
	{
		si.cbSize=sizeof(si);
		si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
		int prevY=si.nPos;
		si.nPos-=cy;
		if(si.nPos<si.nMin) si.nPos=si.nMin;
		if(si.nPos>si.nMax-(int)si.nPage) si.nPos=si.nMax-(int)si.nPage;
		ScrollWindow(0,prevY-si.nPos);
		SetScrollInfo(SB_VERT,&si);
	}
}

