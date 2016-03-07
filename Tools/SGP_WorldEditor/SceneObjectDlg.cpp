// SceneObjectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "SceneObjectDlg.h"
#include "SceneEditorDlg.h"
#include "afxdialogex.h"

#include "WorldEditorConfig.h"
#include "MouseMsgInterface.h"
#include "AddObjectHandler.h"
#include "AddLightObjectHandler.h"
#include "AddObjOperation.h"
#include "SelectObjHandler.h"
#include "CommonObjDef.h"
#include "SceneObjectManager.h"

// CSceneObjectDlg 对话框

CSceneObjectDlg* CSceneObjectDlg::s_pSceneObjDlg=NULL;

IMPLEMENT_DYNAMIC(CSceneObjectDlg, CDialogEx)

CSceneObjectDlg::CSceneObjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSceneObjectDlg::IDD, pParent)
{

}

CSceneObjectDlg::~CSceneObjectDlg()
{
}

void CSceneObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDB_EXPAND,m_ExpandButton);
	//	DDX_Control(pDX,IDC_OBJECTS_TREE,m_ObjTreeCtrl);
	DDX_Control(pDX, IDC_OBJECTS_TREE, m_ObjTreeCtrl);
}

BEGIN_MESSAGE_MAP(CSceneObjectDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDB_EXPAND,OnExpand)
	ON_BN_CLICKED(IDB_DELETE_OBJECT,OnDeleteObject)
	ON_BN_CLICKED(IDB_ADD_OBJECT,OnAddObject)
	ON_BN_CLICKED(IDB_ADD_LIGHT,OnAddLightObj)
END_MESSAGE_MAP()

// CSceneObjectDlg 消息处理程序

BOOL CSceneObjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	s_pSceneObjDlg=this;

	GetClientRect(m_rc);
	m_ObjTreeCtrl.SetPath(WorldEditorConfig::GetInstance()->GetDataFileDir());
	m_ObjTreeCtrl.Init();

	SetExpand(FALSE);

	SetDlgItemText(IDE_OBJ_SCALE,"1.0");

	return TRUE;
}

void CSceneObjectDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ExpandButton.m_hWnd!=NULL)
		m_ExpandButton.MoveWindow(0,0,cx,20);
}

void CSceneObjectDlg::SetExpand(BOOL bExpand)
{
	m_ExpandButton.SetExpand(bExpand);
	if(bExpand)
		SetWindowPos(NULL,0,0,m_rc.Width(),m_rc.Height(),SWP_NOMOVE|SWP_SHOWWINDOW);
	else
		SetWindowPos(NULL,0,0,m_rc.Width(),20+1,SWP_NOMOVE|SWP_SHOWWINDOW);
}

void CSceneObjectDlg::OnExpand()
{
	SetExpand(!m_ExpandButton.m_bExpanded);
	((CSceneEditorDlg*)GetParent())->SetCtrlPos();
}

void CSceneObjectDlg::OnDeleteObject()
{
	SceneObjectManager::GetInstance()->DeleteSelectedObjOperation();
}

BOOL CSceneObjectDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CSceneObjectDlg::OnAddObject()
{
	CString mf1Path=m_ObjTreeCtrl.GetMF1FilePath();
	if(!mf1Path.IsEmpty())
	{
		CString sScale;
		GetDlgItemText(IDE_OBJ_SCALE,sScale);
		float fScale=(float)atof(sScale);
		if(fScale<0.0001f) fScale=1.0f;
		MouseMsgInterface::GetInstance()->SwitchToAddObjEditor();
		AddObjectHandler::GetInstance()->SetObjectAttribute(mf1Path,fScale);
		AddObjectHandler::GetInstance()->BeginAdd();
	}
	else
	{
		MessageBox("please choose a mf1 file first!","Scene Object",MB_OK|MB_ICONSTOP);
	}
}

void CSceneObjectDlg::EnableAllCtrls(BOOL bEnable)
{
	CWnd* pWnd = GetWindow(GW_CHILD);
	while(pWnd != NULL)
	{
		if(pWnd->GetDlgCtrlID()!=IDB_EXPAND&&pWnd->GetDlgCtrlID()!=IDC_OBJECTS_TREE)
			pWnd->EnableWindow(bEnable);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
}

void CSceneObjectDlg::EnableObjectCtrls(BOOL bEnable)
{
	CWnd* pWnd = GetWindow(GW_CHILD);
	while(pWnd != NULL)
	{
		int nID=pWnd->GetDlgCtrlID();
		if(nID!=IDB_EXPAND&&nID!=IDC_OBJECTS_TREE&&nID!=IDB_ADD_LIGHT)
			pWnd->EnableWindow(bEnable);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
}

void CSceneObjectDlg::EnableLightCtrls(BOOL bEnable)
{
	GetDlgItem(IDB_ADD_LIGHT)->EnableWindow(bEnable);
}

void CSceneObjectDlg::OnAddLightObj()
{
	MouseMsgInterface::GetInstance()->SwitchToAddLightEditor();
	AddLightObjHandler::GetInstance()->BeginAdd();
}