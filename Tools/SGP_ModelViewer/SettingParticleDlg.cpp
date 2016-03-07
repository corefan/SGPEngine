// SettingParticleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "SettingParticleDlg.h"
#include "afxdialogex.h"

#include "SettingDlg.h"
#include "CommonMacroDef.h"
//engine header
#include "../SGPLibraryCode/SGPHeader.h"
// CSettingParticleDlg 对话框

IMPLEMENT_DYNAMIC(CSettingParticleDlg, CDialogEx)

CSettingParticleDlg::CSettingParticleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingParticleDlg::IDD, pParent)
{

}

CSettingParticleDlg::~CSettingParticleDlg()
{
}

void CSettingParticleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSettingParticleDlg, CDialogEx)
	ON_BN_CLICKED(IDC_STEPMODE_REAL,OnStepModeChanged)
	ON_BN_CLICKED(IDC_STEPMODE_CONSTANT,OnStepModeChanged)
	ON_BN_CLICKED(IDC_STEPMODE_ADAPTIVE,OnStepModeChanged)
	ON_BN_CLICKED(IDB_USE_CLAMPSTEP,OnClampStepModeChanged)
	ON_BN_CLICKED(IDB_APPLY_STEPMODE,OnApplyStepMode)
	ON_BN_CLICKED(IDB_STEPMODE_RESET,OnResetStepMode)
	ON_BN_CLICKED(IDB_EXPAND,OnButtonExpand)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CSettingParticleDlg 消息处理程序

BOOL CSettingParticleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetClientRect(m_rc);
	m_ExpandButton.SubclassDlgItem(IDB_EXPAND,this);
	OnResetStepMode();

//	SetShowMode(FALSE);

	return TRUE;
}

void CSettingParticleDlg::OnStepModeChanged()
{
	if(IsDlgButtonChecked(IDC_STEPMODE_REAL)==BST_CHECKED)
	{
		GetDlgItem(IDE_CONSTANT_STEP)->EnableWindow(FALSE);
		GetDlgItem(IDE_ADAPTIVE_MINSTEP)->EnableWindow(FALSE);
		GetDlgItem(IDE_ADAPTIVE_MAXSTEP)->EnableWindow(FALSE);
		return ;
	}

	if(IsDlgButtonChecked(IDC_STEPMODE_CONSTANT)==BST_CHECKED)
	{
		GetDlgItem(IDE_CONSTANT_STEP)->EnableWindow(TRUE);
		GetDlgItem(IDE_ADAPTIVE_MINSTEP)->EnableWindow(FALSE);
		GetDlgItem(IDE_ADAPTIVE_MAXSTEP)->EnableWindow(FALSE);
		return ;
	}

	if(IsDlgButtonChecked(IDC_STEPMODE_ADAPTIVE)==BST_CHECKED)
	{
		GetDlgItem(IDE_ADAPTIVE_MINSTEP)->EnableWindow(TRUE);
		GetDlgItem(IDE_ADAPTIVE_MAXSTEP)->EnableWindow(TRUE);
		GetDlgItem(IDE_CONSTANT_STEP)->EnableWindow(FALSE);
		return ;
	}
}

void CSettingParticleDlg::OnClampStepModeChanged()
{
	if(IsDlgButtonChecked(IDB_USE_CLAMPSTEP)==BST_CHECKED)
		GetDlgItem(IDE_CLAMP_STEP)->EnableWindow(TRUE);
	else
		GetDlgItem(IDE_CLAMP_STEP)->EnableWindow(FALSE);
}

void CSettingParticleDlg::OnApplyStepMode()
{
	ApplySettting();
}

void CSettingParticleDlg::OnResetStepMode()
{
	CheckDlgButton(IDB_USE_CLAMPSTEP,BST_UNCHECKED);
	OnClampStepModeChanged();
	CheckDlgButton(IDC_STEPMODE_REAL,BST_CHECKED);
	CheckDlgButton(IDC_STEPMODE_CONSTANT,BST_UNCHECKED);
	CheckDlgButton(IDC_STEPMODE_ADAPTIVE,BST_UNCHECKED);
	OnStepModeChanged();

	ApplySettting();
}

void CSettingParticleDlg::ApplySettting()
{
	if(IsDlgButtonChecked(IDC_STEPMODE_REAL)==BST_CHECKED)
	{
		SPARK::System::useRealStep();
	}
	else if(IsDlgButtonChecked(IDC_STEPMODE_CONSTANT)==BST_CHECKED)
	{
		char data[20];
		GetDlgItemText(IDE_CONSTANT_STEP,data,20);
		float constantStep=(float)atof(data);
		if(constantStep<=0)
		{
			MessageBox("ConstantStep value must be positive!\n\nPlease set it correctly and try again!","ModelViewer",MB_OK|MB_ICONERROR);
			GetDlgItem(IDE_CONSTANT_STEP)->SetFocus();
			((CEdit*)GetDlgItem(IDE_CONSTANT_STEP))->SetSel(0,-1);
			return;
		}
		SPARK::System::useConstantStep(constantStep);
	}
	else
	{
		char dataMin[20],dataMax[20];
		GetDlgItemText(IDE_ADAPTIVE_MINSTEP,dataMin,20);
		GetDlgItemText(IDE_ADAPTIVE_MAXSTEP,dataMax,20);
		float minStep=(float)atof(dataMin);
		float maxStep=(float)atof(dataMax);
		if(minStep<=0)
		{
			MessageBox("MinStep value must be positive!\n\nPlease set it correctly and try again!","ModelViewer",MB_OK|MB_ICONERROR);
			GetDlgItem(IDE_ADAPTIVE_MINSTEP)->SetFocus();
			((CEdit*)GetDlgItem(IDE_ADAPTIVE_MINSTEP))->SetSel(0,-1);
			return;
		}
		if(maxStep<=0)
		{
			MessageBox("MaxStep value must be positive!\n\nPlease set it correctly and try again!","ModelViewer",MB_OK|MB_ICONERROR);
			GetDlgItem(IDE_ADAPTIVE_MAXSTEP)->SetFocus();
			((CEdit*)GetDlgItem(IDE_ADAPTIVE_MAXSTEP))->SetSel(0,-1);
			return;
		}
		if(minStep>maxStep)
		{
			MessageBox("MinStep value must be less than MaxStep!\n\nPlease set it correctly and try again!","ModelViewer",MB_OK|MB_ICONERROR);
			return ;
		}
		SPARK::System::useAdaptiveStep(minStep,maxStep);
	}

	if(IsDlgButtonChecked(IDB_USE_CLAMPSTEP)==BST_CHECKED)
	{
		char sClamp[20];
		GetDlgItemText(IDE_CLAMP_STEP,sClamp,20);
		float fClamp=(float)atof(sClamp);
		if(fClamp<=0)
		{
			MessageBox("ClampStep value must be positive!\n\nPlease set it correctly and try again!","ModelViewer",MB_OK|MB_ICONERROR);
			GetDlgItem(IDE_CLAMP_STEP)->SetFocus();
			((CEdit*)GetDlgItem(IDE_CLAMP_STEP))->SetSel(0,-1);
			return ;
		}
		SPARK::System::setClampStep(true,fClamp);
	}
	else
	{
		SPARK::System::setClampStep(false,1.0f);
	}
}

void CSettingParticleDlg::SetShowMode(BOOL bExpand)
{
	CRect rc;
	GetClientRect(rc);
	if(bExpand)
	{
		m_ExpandButton.m_bExpanded=TRUE;
		SetWindowPos(NULL,0,0,rc.Width(),m_rc.Height(),SWP_NOMOVE);
	}
	else
	{
		GetClientRect(m_rc);
		m_ExpandButton.m_bExpanded=FALSE;
		SetWindowPos(NULL,0,0,m_rc.Width(),BUTTON_HEIGHT+1,SWP_NOMOVE);
	}
}

void CSettingParticleDlg::OnButtonExpand()
{
	SetShowMode(!m_ExpandButton.m_bExpanded);
	((CSettingDlg*)GetParent())->NotifySizeChanged();
}

void CSettingParticleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ExpandButton.m_hWnd!=NULL)
	{
		m_ExpandButton.MoveWindow(0,0,cx,BUTTON_HEIGHT);
	}
}

BOOL CSettingParticleDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN)
			return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
