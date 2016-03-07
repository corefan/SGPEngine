// ParticleConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ParticleConfigDlg.h"
#include "afxdialogex.h"

#include "CommonMacroDef.h"
CFont* CParticleConfigDlg::s_EditFont=NULL;
// CParticleConfigDlg 对话框

IMPLEMENT_DYNAMIC(CParticleConfigDlg, CDialogEx)

CParticleConfigDlg::CParticleConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CParticleConfigDlg::IDD, pParent)
{

}

CParticleConfigDlg::~CParticleConfigDlg()
{
}

void CParticleConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CParticleConfigDlg, CDialogEx)
	ON_WM_VSCROLL()
	ON_WM_NCHITTEST()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

// CParticleConfigDlg 消息处理程序

BOOL CParticleConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set Vertical Scroll Info
/*	ModifyStyle(0,WS_VSCROLL|WS_HSCROLL);
	SCROLLINFO si;
	si.cbSize=sizeof(SCROLLINFO);
	si.fMask=SIF_POS;
	si.nPos=0;
	SetScrollInfo(SB_VERT,&si);
	SetScrollInfo(SB_HORZ,&si);
	*/
	// init font for CColorEdit
	m_EditFont.CreatePointFont(/*100*/94,"微软雅黑");
	s_EditFont=&m_EditFont;

	m_SystemDlg.Create(IDD_PARTICLE_SYSTEM,this);
	NotifySizeChanged();
	m_SystemDlg.ShowWindow(SW_SHOW);


	return TRUE;
}

void CParticleConfigDlg::NotifySizeChanged()
{
	SetCtrlPos();
}

CFont* CParticleConfigDlg::GetEditFont()
{
	return s_EditFont;
}

BOOL CParticleConfigDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN)
			return TRUE;
	}
	if(pMsg->message==WM_MOUSEWHEEL)
	{
	/*	short zDelta = GET_WHEEL_DELTA_WPARAM(pMsg->wParam);
		if(zDelta>0)
		{
			SendMessage(WM_VSCROLL,MAKEWPARAM(SB_LINEUP,0));
			SendMessage(WM_VSCROLL,MAKEWPARAM(SB_LINEUP,0));
		}
		else
		{
			SendMessage(WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,0));
			SendMessage(WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,0));
		}
		return TRUE;*/
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CParticleConfigDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

LRESULT CParticleConfigDlg::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(rc);
	ScreenToClient(&point);
	if(point.x>rc.right&&point.y<rc.bottom) return HTVSCROLL;
	if(point.y>rc.bottom&&point.x<rc.right) return HTHSCROLL;

	return CDialogEx::OnNcHitTest(point);
}

void CParticleConfigDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_SystemDlg.GetSafeHwnd()==NULL) return ;

	SetCtrlPos();

}

void CParticleConfigDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CParticleConfigDlg::SetCtrlPos()
{
	CRect rc;
	GetClientRect(rc);
	
	CRect rcSystem;
	m_SystemDlg.GetClientRect(rcSystem);
	if(m_SystemDlg.IsExpand())
		m_SystemDlg.MoveWindow(rc.left,rc.top,rc.Width(),rc.Height());
	else
		m_SystemDlg.MoveWindow(rc.left,rc.top,rc.Width(),rcSystem.Height());
/*	CRect rc;
	GetClientRect(rc);
	int cx=rc.Width(),cy=rc.Height();

	SCROLLINFO si;
	si.cbSize=sizeof(si);
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
	// horizontal scroll bar is no longer needed
	if(si.nMax<=(int)si.nPage)
	{
		si.nPos=0;
	}
	else if(si.nPos>si.nMax-(int)si.nPage)
	{
		si.nPos=si.nMax-si.nPage;
	}
	int xPos=si.nPos;
	si.fMask=SIF_PAGE|SIF_RANGE|SIF_POS;
	SetScrollInfo(SB_HORZ,&si);

	CRect rcSystem;
	m_SystemDlg.GetClientRect(rcSystem);
	// Set vertical scrollbar
	GetScrollInfo(SB_VERT,&si);
	si.nPage=cy;
	si.nMax=rcSystem.Height()+5;
	if((int)si.nPage>=si.nMax)
	{
		si.nPos=0;
	}
	else if(si.nPos>si.nMax-(int)si.nPage)
	{
		si.nPos=si.nMax-si.nPage;
	}
	int yPos=si.nPos;
	si.fMask=SIF_PAGE|SIF_RANGE|SIF_POS;
	SetScrollInfo(SB_VERT,&si);

	GetClientRect(rc);

	int width=cx>DLG_MINWIDTH?cx:DLG_MINWIDTH;
	m_SystemDlg.MoveWindow(-xPos,-yPos+5,width,rcSystem.Height());

	GetClientRect(rc);
	if(rc.Width()!=cx||rc.Height()!=cy)
	{
		SetCtrlPos();
	}*/
}

void CParticleConfigDlg::SetDlgItemFloat(HWND hwnd,int ctrlID,float data)
{
	char result[40];
	sprintf_s(result,"%.6f",data);
	int nLen=strlen(result);
	--nLen;
	while(result[nLen]=='0'&&result[nLen-1]!='.')
		result[nLen--]='\0';
	::SetDlgItemText(hwnd,ctrlID,result);
}

BOOL CParticleConfigDlg::GetDlgItemFloat(HWND hwnd,int ctrlID,float& data)
{
	char sResult[40];
	::GetDlgItemText(hwnd,ctrlID,sResult,40);
	data=(float)atof(sResult);
	//check validate

	return TRUE;
}