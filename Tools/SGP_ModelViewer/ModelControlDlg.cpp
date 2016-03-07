// ModelControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ModelControlDlg.h"
#include "afxdialogex.h"

// CModelControlDlg 对话框
#include "../SGPLibraryCode/SGPHeader.h"//engine header
using namespace sgp;
#include "ModelViewerRenderInterface.h"
#include "CommonMacroDef.h"

IMPLEMENT_DYNAMIC(CModelControlDlg, CDialogEx)

CModelControlDlg* CModelControlDlg::s_pModelCtrlDlg=NULL;

CModelControlDlg::CModelControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CModelControlDlg::IDD, pParent)
{
	m_nCurrentFrame=0;
	m_nStartFrame=0;
	m_nEndFrame=0;
	m_bPlayMode=FALSE;
	m_bNewAnim=true;
	m_nCurrActionSel=-1;
	m_bHaveMixedPlay=false;
}

CModelControlDlg::~CModelControlDlg()
{
}

void CModelControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDB_MIXED_PLAY_ANIMATION,m_MixedAnimationBtn);
	DDX_Control(pDX,IDC_UPPER_ANIM_CHOOSE,m_UpperAnimBox);
	DDX_Control(pDX,IDC_LOWER_ANIM_CHOOSE,m_LowerAnimBox);
}

BEGIN_MESSAGE_MAP(CModelControlDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_ANIM_CHOOSE_COMBOBOX,OnActionSelChanged)
	ON_BN_CLICKED(IDB_PLAY_ANIMATION,OnPlayAnimation)
	ON_BN_CLICKED(IDB_PLAY_PREVFRAME,OnPlayPrevFrame)
	ON_BN_CLICKED(IDB_PLAY_NEXTFRAME,OnPlayNextFrame)
	ON_BN_CLICKED(IDB_PLAY_PREVANIMATION,OnPlayPrevAnimation)
	ON_BN_CLICKED(IDB_PLAY_NEXTANIMATION,OnPlayNextAnimation)
	ON_BN_CLICKED(IDB_MIXED_PLAY_ANIMATION,OnPlayMixedAnimation)
	ON_WM_SIZE()
	ON_WM_NCHITTEST()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

// CModelControlDlg 消息处理程序
//Implement_DlgScroll(CModelControlDlg)

BOOL CModelControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

//	DlgScroll_OnInit()
	ModifyStyle(0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	m_Font.CreatePointFont(100,_T("微软雅黑"));
	GetDlgItem(IDE_CURRENT_FRAME)->SetFont(&m_Font);
	GetDlgItem(IDE_ANIMATION_SPEED)->SetFont(&m_Font);
	GetDlgItem(IDC_STARTFRAME_STATIC)->SetFont(&m_Font);
	GetDlgItem(IDC_ENDFRAME_STATIC)->SetFont(&m_Font);
	GetDlgItem(IDC_ANIM_CHOOSE_STATIC)->SetFont(&m_Font);

	m_ComboBox.SubclassDlgItem(IDC_ANIM_CHOOSE_COMBOBOX,this);
	m_ProgressBar.SubclassDlgItem(IDC_ANIM_PROGRESSBAR,this);
	m_PlayBtn.SubclassDlgItem(IDB_PLAY_ANIMATION,this);
	m_PlayBtn.m_nImageType=PlayRoundButton;
	m_PrevFrameBtn.SubclassDlgItem(IDB_PLAY_PREVFRAME,this);
	m_PrevFrameBtn.m_nImageType=PrevFrameButton;
	m_NextFrameBtn.SubclassDlgItem(IDB_PLAY_NEXTFRAME,this);
	m_NextFrameBtn.m_nImageType=NextFrameButton;
	m_PrevAnimationBtn.SubclassDlgItem(IDB_PLAY_PREVANIMATION,this);
	m_PrevAnimationBtn.m_nImageType=PrevAnimationButton;
	m_NextAnimationBtn.SubclassDlgItem(IDB_PLAY_NEXTANIMATION,this);
	m_NextAnimationBtn.m_nImageType=NextAnimationButton;

	m_MixedAnimationBtn.m_nImageType=PlayRoundButton;

	m_CurrFrameEdit.SubclassDlgItem(IDE_CURRENT_FRAME,this);
	m_AnimSpeedEdit.SubclassDlgItem(IDE_ANIMATION_SPEED,this);

	EnableAllWindows(FALSE);

	SetDlgItemText(IDE_ANIMATION_SPEED,"1.0");

	s_pModelCtrlDlg=this;

	GetClientRect(m_rc);

	InitScrollBar();

	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.SetDelayTime(5000,50);
	m_ToolTip.AddTool(GetDlgItem(IDB_PLAY_PREVANIMATION),"Prev Animation");
	m_ToolTip.AddTool(GetDlgItem(IDB_PLAY_NEXTANIMATION),"Next Animation");
	m_ToolTip.AddTool(GetDlgItem(IDB_PLAY_PREVFRAME),"Prev Frame");
	m_ToolTip.AddTool(GetDlgItem(IDB_PLAY_NEXTFRAME),"Next Frame");
	m_ToolTip.AddTool(GetDlgItem(IDB_PLAY_ANIMATION),"Play/Pause");
	m_ToolTip.AddTool(GetDlgItem(IDB_MIXED_PLAY_ANIMATION),"Play/Stop");

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CModelControlDlg::NotifyModelChanged(BOOL bDynamic)
{
	if(s_pModelCtrlDlg!=NULL)
		s_pModelCtrlDlg->ModelChanged(bDynamic);
}

void CModelControlDlg::ModelChanged(BOOL bDynamic)
{
	m_ComboBox.ResetContent();
	m_UpperAnimBox.ResetContent();
	m_LowerAnimBox.ResetContent();
	m_nCurrActionSel=-1;
	m_bHaveMixedPlay=false;
	m_MixedAnimationBtn.m_nImageType=PlayRoundButton;
	if(bDynamic)// Dynamic Model
	{
		CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
		if(pModelMF1!=NULL)
		{
			uint32 actionCount=pModelMF1->m_Header.m_iNumActionList;
			SGPMF1ActionList *pList=pModelMF1->m_pActionLists;
			if(actionCount!=0)
			{
				m_bHaveMixedPlay=(pModelMF1->m_Header.m_iBipBoneID != -1);
				
			//	m_ComboBox.EnableWindow(TRUE);
				m_ComboBox.AddString("<ALL>");
				for(uint32 i=0;i<actionCount;++i)
				{
					CString actionName=pList[i].m_cName;
					m_ComboBox.AddString(actionName);
					m_UpperAnimBox.AddString(actionName);
					m_LowerAnimBox.AddString(actionName);
				}
				m_ComboBox.SetCurSel(0);
				m_UpperAnimBox.SetCurSel(0);
				m_LowerAnimBox.SetCurSel(0);

				OnActionSelChanged();
			}
			else// the model don't have any actions
			{
				EnableAllWindows(FALSE);
				SetDlgItemText(IDE_CURRENT_FRAME,"");
				m_ProgressBar.SetCurrPos((float)0.0);
			}
		}
	}
	else// Static Model
	{
		EnableAllWindows(FALSE);
		SetDlgItemText(IDE_CURRENT_FRAME,"");
		m_ProgressBar.SetCurrPos((float)0.0);
	}

	if(m_bHaveMixedPlay) EnableMixPlay(TRUE);
	else EnableMixPlay(FALSE);
}

BOOL CModelControlDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONUP)
		m_ToolTip.RelayEvent(pMsg);

	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
		return TRUE;

	if(pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
	{
		if(GetFocus()==GetDlgItem(IDE_CURRENT_FRAME))
		{
			int nFrame=GetDlgItemInt(IDE_CURRENT_FRAME);
			if(nFrame<m_nStartFrame) nFrame=m_nStartFrame;
			if(nFrame>m_nEndFrame) nFrame=m_nEndFrame;
			m_nCurrentFrame=nFrame;
			m_ProgressBar.SetCurrPos((float)(m_nCurrentFrame-m_nStartFrame)/(m_nEndFrame-m_nStartFrame));
			SetCurrentFrame(nFrame);
		}
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

//the action is changed
void CModelControlDlg::OnActionSelChanged()
{
	//get current selected action
	int index=m_ComboBox.GetCurSel();
	if(index!=m_nCurrActionSel)
	{
		m_nCurrActionSel=index;
		m_bNewAnim=true;
	}
//	int index=0;
	CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
	if(pModelMF1!=NULL)
	{
		SGPMF1ActionList *pList=pModelMF1->m_pActionLists;		
		if(index==0)// all action
		{
			int actionCount=pModelMF1->m_Header.m_iNumActionList;
			m_nStartFrame=pList[0].m_iActionStart;
			m_nEndFrame=pList[0].m_iActionEnd;
			for(int i=1;i<actionCount;++i)
			{
				int startframe=pList[i].m_iActionStart;
				int endframe=pList[i].m_iActionEnd;
				if(startframe<m_nStartFrame) m_nStartFrame=startframe;
				if(endframe>m_nEndFrame) m_nEndFrame=endframe;
			}
		}
		else
		{
			m_nStartFrame=pList[index-1].m_iActionStart;
			m_nEndFrame=pList[index-1].m_iActionEnd;
		}
		m_nCurrentFrame=m_nStartFrame;

		//set the action information
		SetDlgItemInt(IDC_STARTFRAME_STATIC,m_nStartFrame);
		SetDlgItemInt(IDC_ENDFRAME_STATIC,m_nEndFrame);
		SetDlgItemInt(IDE_CURRENT_FRAME,m_nCurrentFrame);
		m_ProgressBar.SetCurrPos((float)0.0);//set the processbar position
	//	SetDlgItemText(IDE_ANIMATION_SPEED,"1.0");//set the animation speed
		
		//set the "play" button
		m_PlayBtn.m_nImageType=PlayRoundButton;

		m_bPlayMode=FALSE;

		if(m_bNewAnim)
			ModelViewerRenderInterface::GetInstance()->PlayAnimation(1.0f,m_nCurrentFrame,m_nCurrentFrame,false,true);

		EnableAllWindows(TRUE);
	}
}

void CModelControlDlg::SetCurrentFrame(int nFrame)
{
	m_nCurrentFrame = nFrame;
	SetDlgItemInt(IDE_CURRENT_FRAME,m_nCurrentFrame);
	ModelViewerRenderInterface::GetInstance()->PlayAnimation(1.0f,m_nCurrentFrame,m_nCurrentFrame,true,false);
}

void CModelControlDlg::NotifyCurrentFrame(float fPos)
{
	m_nCurrentFrame=(int)((m_nEndFrame-m_nStartFrame)*fPos+m_nStartFrame);
	SetCurrentFrame(m_nCurrentFrame);
}

void CModelControlDlg::OnPlayAnimation()
{
	if(m_PlayBtn.m_nImageType==PlayRoundButton)
	{
		//do play action
		ModelViewerRenderInterface::GetInstance()->PlayAnimation(GetPlaySpeed(),m_nStartFrame,m_nEndFrame,true,m_bNewAnim);
		m_PlayBtn.m_nImageType=PauseRoundButton;
		EnableWindowInPlayMode(FALSE);
		m_bPlayMode=TRUE;
	}
	else
	{
		//do pause action
		ModelViewerRenderInterface::GetInstance()->PlayAnimation(1.0f,m_nCurrentFrame,m_nCurrentFrame,true,false);
		m_bNewAnim=false;
		m_PlayBtn.m_nImageType=PlayRoundButton;
		EnableWindowInPlayMode(TRUE);
		m_bPlayMode=FALSE;
	}
	m_PlayBtn.Invalidate();
}

void CModelControlDlg::OnPlayPrevFrame()
{
	--m_nCurrentFrame;
	if(m_nCurrentFrame<m_nStartFrame)
		m_nCurrentFrame=m_nEndFrame;
	m_ProgressBar.SetCurrPos((float)(m_nCurrentFrame-m_nStartFrame)/(m_nEndFrame-m_nStartFrame));
	SetCurrentFrame(m_nCurrentFrame);
}

void CModelControlDlg::OnPlayNextFrame()
{
	++m_nCurrentFrame;
	if(m_nCurrentFrame>m_nEndFrame)
		m_nCurrentFrame=m_nStartFrame;
	m_ProgressBar.SetCurrPos((float)(m_nCurrentFrame-m_nStartFrame)/(m_nEndFrame-m_nStartFrame));
	SetCurrentFrame(m_nCurrentFrame);
}

void CModelControlDlg::OnPlayPrevAnimation()
{
	int index=m_ComboBox.GetCurSel();
	if(index>0)
	{
		m_ComboBox.SetCurSel(index-1);
		OnActionSelChanged();
	}
}

void CModelControlDlg::OnPlayNextAnimation()
{
	int index=m_ComboBox.GetCurSel();
	if(index<m_ComboBox.GetCount()-1)
	{
		m_ComboBox.SetCurSel(index+1);
		OnActionSelChanged();
	}
}

void CModelControlDlg::EnableAllWindows(BOOL bEnable)
{
	CWnd* pWnd=GetWindow(GW_CHILD);
	while(pWnd!=NULL)
	{
		pWnd->EnableWindow(bEnable);
		pWnd->Invalidate();
		pWnd=pWnd->GetWindow(GW_HWNDNEXT);
	}
}

void CModelControlDlg::EnableWindowInPlayMode(BOOL bEnable)
{
	m_ComboBox.EnableWindow(bEnable);
	m_CurrFrameEdit.EnableWindow(bEnable);
	m_AnimSpeedEdit.EnableWindow(bEnable);
	m_PrevFrameBtn.EnableWindow(bEnable);
	m_NextFrameBtn.EnableWindow(bEnable);
	m_PrevAnimationBtn.EnableWindow(bEnable);
	m_NextAnimationBtn.EnableWindow(bEnable);
	m_ProgressBar.EnableWindow(bEnable);

	GetDlgItem(IDC_UPPER_ANIM_CHOOSE)->EnableWindow(bEnable);
	GetDlgItem(IDC_LOWER_ANIM_CHOOSE)->EnableWindow(bEnable);
	GetDlgItem(IDB_MIXED_PLAY_ANIMATION)->EnableWindow(bEnable);
}

float CModelControlDlg::GetPlaySpeed()
{
	char strSpeed[20];
	GetDlgItemText(IDE_ANIMATION_SPEED,strSpeed,20);
	double speed=atof(strSpeed);
	if(speed<1e-8) speed=1.0;
	CString str;
	str.Format(_T("%f"),speed);
	str.TrimRight('0');
	if(str.Right(1)==".") str+="0";
	SetDlgItemText(IDE_ANIMATION_SPEED,str);

	return (float)speed;
}

void CModelControlDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(!m_rc.IsRectEmpty())
	{
		SetScrollBar();
	}
}

void CModelControlDlg::NotifyPlayFrameChanged(int nFrame)
{
	if(s_pModelCtrlDlg!=NULL)
		s_pModelCtrlDlg->FramePlayChanged(nFrame);
}

void CModelControlDlg::FramePlayChanged(int nFrame)
{
	if(m_nCurrentFrame!=nFrame&&m_bPlayMode)
	{
		m_nCurrentFrame=nFrame;
		SetDlgItemInt(IDE_CURRENT_FRAME,m_nCurrentFrame);
		m_ProgressBar.SetCurrPos((float)(m_nCurrentFrame-m_nStartFrame)/(m_nEndFrame-m_nStartFrame));
	}
}

LRESULT CModelControlDlg::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(rc);
	ScreenToClient(&point);
	if(point.x>rc.right&&point.y<rc.bottom) return HTVSCROLL;
	if(point.y>rc.bottom&&point.x<rc.right) return HTHSCROLL;

	return CDialogEx::OnNcHitTest(point);
}

void CModelControlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CModelControlDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CModelControlDlg::InitScrollBar()
{
	ModifyStyle(0,WS_HSCROLL|WS_VSCROLL);

	CRect rcClient;
	GetClientRect(rcClient);

	SCROLLINFO si;
	si.cbSize=sizeof(si);
	si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
	si.nPos=0;
	si.nMin=0;
	si.nMax=m_rc.Width();
	si.nPage=rcClient.Width();
	SetScrollInfo(SB_HORZ,&si);

	si.cbSize=sizeof(si);
	si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
	si.nPos=0;
	si.nMin=0;
	si.nMax=m_rc.Height();
	si.nPage=rcClient.Height();
	SetScrollInfo(SB_VERT,&si);
}

void CModelControlDlg::SetScrollBar()
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
	if(si.nMax<=(int)si.nPage)
	{
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

void CModelControlDlg::ModelClosed()
{
	EnableAllWindows(FALSE);
	m_ComboBox.ResetContent();
	SetDlgItemText(IDC_STARTFRAME_STATIC,"");
	SetDlgItemText(IDC_ENDFRAME_STATIC,"");
	SetDlgItemText(IDE_CURRENT_FRAME,"");
}

void CModelControlDlg::EnableMixPlay(BOOL bEnable)
{
	GetDlgItem(IDC_UPPER_ANIM_CHOOSE)->EnableWindow(bEnable);
	GetDlgItem(IDC_LOWER_ANIM_CHOOSE)->EnableWindow(bEnable);
	GetDlgItem(IDB_MIXED_PLAY_ANIMATION)->EnableWindow(bEnable);
}

void CModelControlDlg::EnableWindowInMixPlayMode(BOOL bEnable)
{
	GetDlgItem(IDC_ANIM_CHOOSE_COMBOBOX)->EnableWindow(bEnable);
	GetDlgItem(IDE_CURRENT_FRAME)->EnableWindow(bEnable);
	GetDlgItem(IDC_ANIM_PROGRESSBAR)->EnableWindow(bEnable);
	GetDlgItem(IDB_PLAY_PREVANIMATION)->EnableWindow(bEnable);
	GetDlgItem(IDB_PLAY_PREVFRAME)->EnableWindow(bEnable);
	GetDlgItem(IDB_PLAY_ANIMATION)->EnableWindow(bEnable);
	GetDlgItem(IDB_PLAY_NEXTFRAME)->EnableWindow(bEnable);
	GetDlgItem(IDB_PLAY_NEXTANIMATION)->EnableWindow(bEnable);
	GetDlgItem(IDE_ANIMATION_SPEED)->EnableWindow(bEnable);
	GetDlgItem(IDC_UPPER_ANIM_CHOOSE)->EnableWindow(bEnable);
	GetDlgItem(IDC_LOWER_ANIM_CHOOSE)->EnableWindow(bEnable);
}

void CModelControlDlg::OnPlayMixedAnimation()
{
	if(m_MixedAnimationBtn.m_nImageType==PlayRoundButton)
	{
		CSGPModelMF1* pModelMF1=ModelViewerRenderInterface::GetInstance()->GetModelMF1();
		if(pModelMF1!=NULL)
		{
			uint32 upperStart,upperEnd,lowerStart,lowerEnd;
			SGPMF1ActionList *pList=pModelMF1->m_pActionLists;		

			int upperIndex=m_UpperAnimBox.GetCurSel();
			int lowerIndex=m_LowerAnimBox.GetCurSel();
			upperStart=pList[upperIndex].m_iActionStart;
			upperEnd=pList[upperIndex].m_iActionEnd;
			lowerStart=pList[lowerIndex].m_iActionStart;
			lowerEnd=pList[lowerIndex].m_iActionEnd;
			ModelViewerRenderInterface::GetInstance()->PlayMixedAnimation(upperStart,upperEnd,lowerStart,lowerEnd);
		}
		m_MixedAnimationBtn.m_nImageType=StopRoundButton;
		m_ProgressBar.SetCurrPos((float)0.0);
		EnableWindowInMixPlayMode(FALSE);
	}
	else
	{
		EnableWindowInMixPlayMode(TRUE);
		m_MixedAnimationBtn.m_nImageType=PlayRoundButton;
		m_bNewAnim=true;
		OnActionSelChanged();
	}
}
