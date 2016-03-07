// SettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "SettingDlg.h"
#include "afxdialogex.h"

#include "ModelViewerConfig.h"
#include "CommonMacroDef.h"

#define SETTINGDLG_WIDTH 255
// CSettingDlg 对话框
IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingDlg::IDD, pParent)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDB_SET_BKGND_COLOR,OnSetBkgndColor)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

// CSettingDlg 消息处理程序

BOOL CSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ModelViewerConfig *pConfig=ModelViewerConfig::GetInstance();
	CheckDlgButton(IDC_SHOW_GROUND,pConfig->m_bShowGround);
	CheckDlgButton(IDC_SHOW_BOUNDINGBOX,pConfig->m_bShowBoundingBox);
	CheckDlgButton(IDC_SHOW_WIREFEAME,pConfig->m_bShowWireframe);
	CheckDlgButton(IDC_SHOW_BONES,pConfig->m_bShowBones);
	CheckDlgButton(IDC_SHOW_NORMAL,pConfig->m_bShowNormalLines);
	CheckDlgButton(IDC_SHOW_ATTACHMENT,pConfig->m_bShowAttachment);
	CheckDlgButton(IDC_SHOW_MESH,pConfig->m_bShowMesh);
	CheckDlgButton(IDC_SHOW_PARTICLE_EMITTER,pConfig->m_bShowParticleEmitter);

	m_ColorButton.SubclassDlgItem(IDB_SET_BKGND_COLOR,this);
	m_ColorButton.SetColor(pConfig->m_BkgndColor);
	m_DefaultColBtn.SubclassDlgItem(IDB_SET_DEFAULT_BKGND_COLOR,this);
	m_DefaultColBtn.SetColor(pConfig->m_DefaultBkgndColor);

	// Show ParticleDlg
	m_ParticleDlg.Create(IDD_SETTING_PARTILCE,this);
	CRect rcSeparate;
	GetDlgItem(IDC_SEPARATE)->GetWindowRect(rcSeparate);
	ScreenToClient(rcSeparate);
	m_CommonHeight=rcSeparate.bottom;
	CRect rcParticle;
	m_ParticleDlg.GetClientRect(rcParticle);
	CRect rc=rcParticle;
	rc.top=rcSeparate.bottom;
	rc.bottom=rc.top+rcParticle.Height();
	m_ParticleDlg.MoveWindow(rc);
	m_ParticleDlg.ShowWindow(SW_SHOW);

	InitScrollBar();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSettingDlg::OnSetBkgndColor()
{
	ModelViewerConfig *pConfig=ModelViewerConfig::GetInstance();
	CColorDialog dlg(RGB((BYTE)(pConfig->m_BkgndColor[0]*255),\
		(BYTE)(pConfig->m_BkgndColor[1]*255),(BYTE)(pConfig->m_BkgndColor[2]*255)),CC_FULLOPEN,this);
	if(IDOK==dlg.DoModal())
	{
		COLORREF col=dlg.GetColor();
		pConfig->m_BkgndColor[0]=(float)(GetRValue(col)/255.0);
		pConfig->m_BkgndColor[1]=(float)(GetGValue(col)/255.0);
		pConfig->m_BkgndColor[2]=(float)(GetBValue(col)/255.0);
		m_ColorButton.SetColor(pConfig->m_BkgndColor);
	}
}

BOOL CSettingDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	ModelViewerConfig *pConfig=ModelViewerConfig::GetInstance();
	switch(LOWORD(wParam))
	{
	case IDC_SHOW_BOUNDINGBOX:
		pConfig->m_bShowBoundingBox=IsDlgButtonChecked(IDC_SHOW_BOUNDINGBOX);
		break;
	case IDC_SHOW_WIREFEAME:
		pConfig->m_bShowWireframe=IsDlgButtonChecked(IDC_SHOW_WIREFEAME);
		break;
	case IDC_SHOW_GROUND:
		pConfig->m_bShowGround=IsDlgButtonChecked(IDC_SHOW_GROUND);
		break;
	case IDC_SHOW_BONES:
		pConfig->m_bShowBones=IsDlgButtonChecked(IDC_SHOW_BONES);
		break;
	case IDC_SHOW_NORMAL:
		pConfig->m_bShowNormalLines=IsDlgButtonChecked(IDC_SHOW_NORMAL);
		break;
	case IDC_SHOW_ATTACHMENT:
		pConfig->m_bShowAttachment=IsDlgButtonChecked(IDC_SHOW_ATTACHMENT);
		break;
	case IDC_SHOW_MESH:
		pConfig->m_bShowMesh=IsDlgButtonChecked(IDC_SHOW_MESH);
		break;
	case IDC_SHOW_PARTICLE_EMITTER:
		pConfig->m_bShowParticleEmitter=IsDlgButtonChecked(IDC_SHOW_PARTICLE_EMITTER);
		break;
	case IDB_SET_DEFAULT_BKGND_COLOR:
		pConfig->m_BkgndColor[0]=pConfig->m_DefaultBkgndColor[0];
		pConfig->m_BkgndColor[1]=pConfig->m_DefaultBkgndColor[1];
		pConfig->m_BkgndColor[2]=pConfig->m_DefaultBkgndColor[2];
		m_ColorButton.SetColor(pConfig->m_BkgndColor);
		break;
	default:
		break;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}


void CSettingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ParticleDlg.m_hWnd!=NULL)
	{
		int width=(cx>SETTINGDLG_WIDTH)?cx:SETTINGDLG_WIDTH;
		CRect rcParticle;
		m_ParticleDlg.GetClientRect(rcParticle);
		m_ParticleDlg.SetWindowPos(NULL,0,0,width,rcParticle.Height(),SWP_NOMOVE);

		SetScrollBar();
	}

}

BOOL CSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN)
			return TRUE;
	}

	if(pMsg->message==WM_MOUSEWHEEL)
	{
		short zDelta = GET_WHEEL_DELTA_WPARAM(pMsg->wParam);
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
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CSettingDlg::NotifySizeChanged()
{
	CRect rcSeparate;
	GetDlgItem(IDC_SEPARATE)->GetWindowRect(rcSeparate);
	ScreenToClient(rcSeparate);
	CRect rcParticle;
	m_ParticleDlg.GetWindowRect(rcParticle);
	ScreenToClient(rcParticle);
	CRect rc=rcParticle;
	rc.top=rcSeparate.bottom;
	rc.bottom=rc.top+rcParticle.Height();
	m_ParticleDlg.MoveWindow(rc);

	SetScrollBar();
}

void CSettingDlg::InitScrollBar()
{
	ModifyStyle(0,WS_HSCROLL|WS_VSCROLL);

	CRect rcClient;
	GetClientRect(rcClient);

	SCROLLINFO si;
	si.cbSize=sizeof(si);
	si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
	si.nPos=0;
	si.nMin=0;
	si.nMax=SETTINGDLG_WIDTH;
	si.nPage=rcClient.Width();
	SetScrollInfo(SB_HORZ,&si);

	CRect rcParticle;
	m_ParticleDlg.GetClientRect(rcParticle);
	si.cbSize=sizeof(si);
	si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
	si.nPos=0;
	si.nMin=0;
	si.nMax=rcParticle.Height()+m_CommonHeight;
	si.nPage=rcClient.Height();
	SetScrollInfo(SB_VERT,&si);
}

void CSettingDlg::SetScrollBar()
{
	CRect rcClient;
	GetClientRect(rcClient);

	CRect rcSeparate;
	GetDlgItem(IDC_SEPARATE)->GetWindowRect(rcSeparate);
	ScreenToClient(rcSeparate);

	// Horizontal ScrollBar
	SCROLLINFO si;
	si.cbSize=sizeof(si);
	GetScrollInfo(SB_HORZ,&si);
	si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
	si.nPage=rcClient.Width();
	if(si.nMax<=(int)si.nPage)
	{
	//	ScrollWindow(si.nPos,0);	
		ScrollWindow(-rcSeparate.left,0);
		si.nPos=0;
	}
	else if(si.nPos>si.nMax-(int)si.nPage)
	{
		ScrollWindow(si.nPos+si.nPage-si.nMax,0);
		si.nPos=si.nMax-si.nPage;
	}
	SetScrollInfo(SB_HORZ,&si);

	// Vertical ScrollBar
	si.cbSize=sizeof(si);
	GetScrollInfo(SB_VERT,&si);
	si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
	si.nPage=rcClient.Height();
	CRect rcParticle;
	m_ParticleDlg.GetClientRect(rcParticle);
	si.nMax=m_CommonHeight+rcParticle.Height();
	if(si.nMax<=(int)si.nPage)
	{
	//	ScrollWindow(0,si.nPos);
		ScrollWindow(0,-rcSeparate.top);
		si.nPos=0;
	}
	else if(si.nPos>si.nMax-(int)si.nPage)
	{
		ScrollWindow(0,si.nPos+si.nPage-si.nMax);
		si.nPos=si.nMax-si.nPage;
	}
	SetScrollInfo(SB_VERT,&si);

	CRect rcNow;
	GetClientRect(rcNow);
	if(rcNow.Width()!=rcClient.Width()||rcNow.Height()!=rcClient.Height())
	{
		SetScrollBar();
	}
}

void CSettingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CSettingDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

LRESULT CSettingDlg::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(rc);
	ScreenToClient(&point);
	if(point.x>rc.right&&point.y<rc.bottom) return HTVSCROLL;
	if(point.y>rc.bottom&&point.x<rc.right) return HTHSCROLL;

	return CDialogEx::OnNcHitTest(point);
}

