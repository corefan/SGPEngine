// ObjectEditorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "ObjectEditorDlg.h"
#include "afxdialogex.h"
#include "SceneEditorDlg.h"
#include "SceneObjectManager.h"
#include "SelectObjHandler.h"
#include "CommonObjDef.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "ObjAttributeOperation.h"
#include "OperationManager.h"
#include "HelpFunction.h"

// CObjectEditorDlg 对话框

IMPLEMENT_DYNAMIC(CObjectEditorDlg, CDialogEx)

CObjectEditorDlg::CObjectEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CObjectEditorDlg::IDD, pParent)
{
	
}

CObjectEditorDlg::~CObjectEditorDlg()
{
}

void CObjectEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDB_EXPAND,m_ExpandButton);

	DDX_Control(pDX,IDC_STEP_ROTATE_SPIN,m_StepRotatePin);
	DDX_Control(pDX,IDC_RANDOM_ROTATE_MINX_SPIN,m_RandomRotatePinMinX);
	DDX_Control(pDX,IDC_RANDOM_ROTATE_MAXX_SPIN,m_RandomRotatePinMaxX);
	DDX_Control(pDX,IDC_RANDOM_ROTATE_MINY_SPIN,m_RandomRotatePinMinY);
	DDX_Control(pDX,IDC_RANDOM_ROTATE_MAXY_SPIN,m_RandomRotatePinMaxY);
	DDX_Control(pDX,IDC_RANDOM_ROTATE_MINZ_SPIN,m_RandomRotatePinMinZ);
	DDX_Control(pDX,IDC_RANDOM_ROTATE_MAXZ_SPIN,m_RandomRotatePinMaxZ);

	DDX_Control(pDX,IDC_STEP_MOVE_SPIN,m_StepMovePin);
	DDX_Control(pDX,IDC_STEP_SCALE_SPIN,m_StepScalePin);
	DDX_Control(pDX,IDC_RANDOM_SCALE_MIN_SPIN,m_RandomScalePinMin);
	DDX_Control(pDX,IDC_RANDOM_SCALE_MAX_SPIN,m_RandomScalePinMax);
}

BEGIN_MESSAGE_MAP(CObjectEditorDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDB_EXPAND,OnExpand)
	ON_NOTIFY(UDN_DELTAPOS, IDC_STEP_MOVE_SPIN, &CObjectEditorDlg::OnDeltaposStepMoveSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_STEP_SCALE_SPIN, &CObjectEditorDlg::OnDeltaposStepScaleSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_RANDOM_SCALE_MIN_SPIN, &CObjectEditorDlg::OnDeltaposRandomScaleMinSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_RANDOM_SCALE_MAX_SPIN, &CObjectEditorDlg::OnDeltaposRandomScaleMaxSpin)
END_MESSAGE_MAP()

// CObjectEditorDlg 消息处理程序

BOOL CObjectEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetClientRect(m_rc);
	SetExpand(FALSE);

	SetDlgItemText(IDE_STEP_MOVE_VALUE,"1.000");
	SetDlgItemText(IDE_STEP_ROTATE_VALUE,"3");
	SetDlgItemText(IDE_STEP_SCALE_VALUE,"1.200");
	SetDlgItemText(IDE_RANDOM_ROTATE_MINX,"2");
	SetDlgItemText(IDE_RANDOM_ROTATE_MAXX,"5");
	SetDlgItemText(IDE_RANDOM_ROTATE_MINY,"30");
	SetDlgItemText(IDE_RANDOM_ROTATE_MAXY,"60");
	SetDlgItemText(IDE_RANDOM_ROTATE_MINZ,"120");
	SetDlgItemText(IDE_RANDOM_ROTATE_MAXZ,"140");
	SetDlgItemText(IDE_RANDOM_SCALE_MIN,"0.8");
	SetDlgItemText(IDE_RANDOM_SCALE_MAX,"1.2");

	m_StepRotatePin.SetRange(0,360);
	m_StepRotatePin.SetBuddy(GetDlgItem(IDE_STEP_ROTATE_VALUE));
	m_RandomRotatePinMinX.SetRange(0,360);
	m_RandomRotatePinMinX.SetBuddy(GetDlgItem(IDE_RANDOM_ROTATE_MINX));
	m_RandomRotatePinMaxX.SetRange(0,360);
	m_RandomRotatePinMaxX.SetBuddy(GetDlgItem(IDE_RANDOM_ROTATE_MAXX));
	m_RandomRotatePinMinY.SetRange(0,360);
	m_RandomRotatePinMinY.SetBuddy(GetDlgItem(IDE_RANDOM_ROTATE_MINY));
	m_RandomRotatePinMaxY.SetRange(0,360);
	m_RandomRotatePinMaxY.SetBuddy(GetDlgItem(IDE_RANDOM_ROTATE_MAXY));
	m_RandomRotatePinMinZ.SetRange(0,360);
	m_RandomRotatePinMinZ.SetBuddy(GetDlgItem(IDE_RANDOM_ROTATE_MINZ));
	m_RandomRotatePinMaxZ.SetRange(0,360);
	m_RandomRotatePinMaxZ.SetBuddy(GetDlgItem(IDE_RANDOM_ROTATE_MAXZ));

	m_StepMovePin.SetBuddy(GetDlgItem(IDE_STEP_MOVE_VALUE));
	m_StepScalePin.SetBuddy(GetDlgItem(IDE_STEP_SCALE_VALUE));
	m_RandomScalePinMin.SetBuddy(GetDlgItem(IDE_RANDOM_SCALE_MIN));
	m_RandomScalePinMax.SetBuddy(GetDlgItem(IDE_RANDOM_SCALE_MAX));

	return TRUE;
}

void CObjectEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ExpandButton.m_hWnd!=NULL)
		m_ExpandButton.MoveWindow(0,0,cx,20);
}

void CObjectEditorDlg::SetExpand(BOOL bExpand)
{
	m_ExpandButton.SetExpand(bExpand);
	if(bExpand)
		SetWindowPos(NULL,0,0,m_rc.Width(),m_rc.Height(),SWP_NOMOVE|SWP_SHOWWINDOW);
	else
		SetWindowPos(NULL,0,0,m_rc.Width(),20+1,SWP_NOMOVE|SWP_SHOWWINDOW);
}

void CObjectEditorDlg::OnExpand()
{
	SetExpand(!m_ExpandButton.m_bExpanded);
	((CSceneEditorDlg*)GetParent())->SetCtrlPos();
}

void CObjectEditorDlg::OnDeltaposStepMoveSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	CString str;
	GetDlgItemText(IDE_STEP_MOVE_VALUE,str);
	float data=(float)atof(str);

	if(pNMUpDown->iDelta==1) data-=0.1f;
	if(pNMUpDown->iDelta==-1) data+=0.1f;
	
	str.Format("%.3f",data);
	SetDlgItemText(IDE_STEP_MOVE_VALUE,str);

	*pResult = 0;
}

void CObjectEditorDlg::OnDeltaposStepScaleSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	CString str;
	GetDlgItemText(IDE_STEP_SCALE_VALUE,str);
	float data=(float)atof(str);

	if(pNMUpDown->iDelta==1) data-=0.1f;
	if(pNMUpDown->iDelta==-1) data+=0.1f;

	str.Format("%.3f",data);
	SetDlgItemText(IDE_STEP_SCALE_VALUE,str);

	*pResult = 0;
}

void CObjectEditorDlg::OnDeltaposRandomScaleMinSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	CString str;
	GetDlgItemText(IDE_RANDOM_SCALE_MIN,str);
	float data=(float)atof(str);

	if(pNMUpDown->iDelta==1) data-=0.1f;
	if(pNMUpDown->iDelta==-1) data+=0.1f;

	if(data<0.02f) data=0.02f;

	str.Format("%.3f",data);
	SetDlgItemText(IDE_RANDOM_SCALE_MIN,str);

	*pResult = 0;
}

void CObjectEditorDlg::OnDeltaposRandomScaleMaxSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	CString str;
	GetDlgItemText(IDE_RANDOM_SCALE_MAX,str);
	float data=(float)atof(str);

	if(pNMUpDown->iDelta==1) data-=0.1f;
	if(pNMUpDown->iDelta==-1) data+=0.1f;

	if(data<0.02f) data=0.02f;

	str.Format("%.3f",data);
	SetDlgItemText(IDE_RANDOM_SCALE_MAX,str);

	*pResult = 0;
}

float CObjectEditorDlg::GetDlgItemFloat(UINT nID)
{
	CString str;
	GetDlgItemText(nID,str);
	return (float)atof(str);
}

float CObjectEditorDlg::GetRotateValue(UINT nID)
{
	return (float)GetDlgItemInt(nID)*float_Pi/180.0f;
}

BOOL CObjectEditorDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int nID=LOWORD(wParam);

	switch(nID)
	{
	//Step Move
	case IDB_STEPMOVE_INCR_X:
		MoveObjects(GetDlgItemFloat(IDE_STEP_MOVE_VALUE),0,0);
		break;
	case IDB_STEPMOVE_DECR_X:
		MoveObjects(-GetDlgItemFloat(IDE_STEP_MOVE_VALUE),0,0);
		break;
	case IDB_STEPMOVE_INCR_Y:
		MoveObjects(0,GetDlgItemFloat(IDE_STEP_MOVE_VALUE),0);
		break;
	case IDB_STEPMOVE_DECR_Y:
		MoveObjects(0,-GetDlgItemFloat(IDE_STEP_MOVE_VALUE),0);
		break;
	case IDB_STEPMOVE_INCR_Z:
		MoveObjects(0,0,GetDlgItemFloat(IDE_STEP_MOVE_VALUE));
		break;
	case IDB_STEPMOVE_DECR_Z:
		MoveObjects(0,0,-GetDlgItemFloat(IDE_STEP_MOVE_VALUE));
		break;

	// Step Rotate
	case IDB_STEPROTATE_INCR_X:
		RotateObjects(GetRotateValue(IDE_STEP_ROTATE_VALUE),0,0);
		break;
	case IDB_STEPROTATE_DECR_X:
		RotateObjects(-GetRotateValue(IDE_STEP_ROTATE_VALUE),0,0);
		break;
	case IDB_STEPROTATE_INCR_Y:
		RotateObjects(0,GetRotateValue(IDE_STEP_ROTATE_VALUE),0);
		break;
	case IDB_STEPROTATE_DECR_Y:
		RotateObjects(0,-GetRotateValue(IDE_STEP_ROTATE_VALUE),0);
		break;
	case IDB_STEPROTATE_INCR_Z:
		RotateObjects(0,0,GetRotateValue(IDE_STEP_ROTATE_VALUE));
		break;
	case IDB_STEPROTATE_DECR_Z:
		RotateObjects(0,0,-GetRotateValue(IDE_STEP_ROTATE_VALUE));
		break;

	// Step Scale
	case IDB_STEPSCALE_INCR:
		ScaleObjects(GetDlgItemFloat(IDE_STEP_SCALE_VALUE));
		break;
	//case IDB_STEPSCALE_DECR:
	//	ScaleObjects(GetDlgItemFloat(IDE_STEP_SCALE_VALUE));

	// Random Rotate
	case IDB_RANDOM_ROTATE_UPDATE:
		RotateObjects(SPARK::random(GetRotateValue(IDE_RANDOM_ROTATE_MINX),GetRotateValue(IDE_RANDOM_ROTATE_MAXX)),\
			SPARK::random(GetRotateValue(IDE_RANDOM_ROTATE_MINY),GetRotateValue(IDE_RANDOM_ROTATE_MAXY)),\
			SPARK::random(GetRotateValue(IDE_RANDOM_ROTATE_MINZ),GetRotateValue(IDE_RANDOM_ROTATE_MAXZ)));
		break;
	// Random Scale
	case IDB_RANDOM_SCALE_UPDATE:
		ScaleObjects(SPARK::random(GetDlgItemFloat(IDE_RANDOM_SCALE_MIN),GetDlgItemFloat(IDE_RANDOM_SCALE_MAX)));
		break;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}

void CObjectEditorDlg::MoveObjects(float dx,float dy,float dz)
{
	if(dx==0.0f&&dy==0.0f&&dz==0.0f) return ;

	ObjAttributeOperation* pObjAttrOp=new ObjAttributeOperation;
	std::vector<CommonObject>& objVector=SelectObjHandler::GetInstance()->GetSelectedObj();
	pObjAttrOp->AddObjects(objVector);
	pObjAttrOp->SaveAllPrevState();
	SceneObjectManager::GetInstance()->MoveSceneObject(objVector,dx,dy,dz,FALSE);
	pObjAttrOp->SaveAllCurrState();
	OperationManager::GetInstance()->AddNewOperation(pObjAttrOp);
}

void CObjectEditorDlg::RotateObjects(float dx,float dy,float dz)
{
	if(dx==0.0f&&dy==0.0f&&dz==0.0f) return ;
	std::vector<CommonObject>& objVector=SelectObjHandler::GetInstance()->GetSelectedObj();
	if(!ContainMF1(objVector)) return ;

	ObjAttributeOperation* pObjAttrOp=new ObjAttributeOperation;
	pObjAttrOp->AddMF1Objects(objVector);
	pObjAttrOp->SaveAllPrevState();
	SceneObjectManager::GetInstance()->RotateSceneObject(objVector,dx,dy,dz,FALSE);
	pObjAttrOp->SaveAllCurrState();
	OperationManager::GetInstance()->AddNewOperation(pObjAttrOp);
}

void CObjectEditorDlg::ScaleObjects(float fScale)
{
	if(fScale==1.0f) return ;
	std::vector<CommonObject>& objVector=SelectObjHandler::GetInstance()->GetSelectedObj();
	if(!ContainMF1(objVector)) return ;

	ObjAttributeOperation* pObjAttrOp=new ObjAttributeOperation;
	pObjAttrOp->AddMF1Objects(objVector);
	pObjAttrOp->SaveAllPrevState();
	SceneObjectManager::GetInstance()->ScaleSceneObject(objVector,fScale,FALSE);
	pObjAttrOp->SaveAllCurrState();
	OperationManager::GetInstance()->AddNewOperation(pObjAttrOp);
}

void CObjectEditorDlg::EnableAllCtrls(BOOL bEnable)
{
	CWnd* pWnd = GetWindow(GW_CHILD);
	while(pWnd != NULL)
	{
		if(pWnd->GetDlgCtrlID()!=IDB_EXPAND&&pWnd->GetDlgCtrlID()!=IDC_OBJECTS_TREE)
			pWnd->EnableWindow(bEnable);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
}