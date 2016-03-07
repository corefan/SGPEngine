// TerrainHeightDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "WorldEditor.h"
#include "TerrainHeightDlg.h"

#include "TerrainEditorDlg.h"
#include "TerrainHeightHandler.h"
#include "TerrainHeightOperation.h"
#include "OperationManager.h"
#include "MouseMsgInterface.h"

#include "./Render Interface/WorldEditorRenderInterface.h"

static int button_id1[]={\
	IDB_VERTEX_HEIGHT_ADJUST,\
	IDB_VERTEX_HEIGHT_UP,\
	IDB_VERTEX_HEIGHT_DOWN,\
	IDB_VERTEX_HEIGHT_SMOOTH,\
	IDB_VERTEX_HEIGHT_NOISE,\
	IDB_VERTEX_HEIGHT_FLATTEN,\
	IDB_VERTEX_HEIGHT_SET,\
	IDB_MODE_CHUNK_ADJUST,\
	IDB_MODE_CELL_ADJUST
};

// CTerrainHeightDlg 对话框

IMPLEMENT_DYNAMIC(CTerrainHeightDlg, CDialogEx)

CTerrainHeightDlg::CTerrainHeightDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTerrainHeightDlg::IDD, pParent)
{

}

CTerrainHeightDlg::~CTerrainHeightDlg()
{
}

void CTerrainHeightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTerrainHeightDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDB_EXPAND, &CTerrainHeightDlg::OnBnClickedExpand)
	ON_BN_CLICKED(IDB_WORLD_HEIGHT_UP,OnWorldHeightUp)
	ON_BN_CLICKED(IDB_WORLD_HEIGHT_DOWN,OnWorldHeightDown)
	ON_BN_CLICKED(IDB_CHUNK_CELL_SET,OnChunkCellHeightSet)
	ON_BN_CLICKED(IDB_CHUNK_CELL_UP,OnChunkCellHeightUp)
	ON_BN_CLICKED(IDB_CHUNK_CELL_DOWN,OnChunkCellHeightDown)
	ON_EN_CHANGE(IDE_VERTEX_HEIGHT_SETVALUE,OnVertexHeightSetValueChanged)
END_MESSAGE_MAP()

// CTerrainHeightDlg 消息处理程序


BOOL CTerrainHeightDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ExpandButton.SubclassDlgItem(IDB_EXPAND,this);
	m_ExpandButton.m_bExpanded=FALSE;
	GetClientRect(m_rc);
	SetExpand(FALSE);

	for(int i=0;i<sizeof(button_id1)/sizeof(int);++i)
		m_Button[i].SubclassDlgItem(button_id1[i],this);

	CSliderCtrl* pInnerCircle=(CSliderCtrl*)GetDlgItem(IDC_INNER_CIRCLE_SIZE);
	pInnerCircle->SetRange(0,SLIDER_LENGTH);
	CSliderCtrl* pOuterCircle=(CSliderCtrl*)GetDlgItem(IDC_OUTER_CIRCLE_SIZE);
	pOuterCircle->SetRange(0,SLIDER_LENGTH);
	CSliderCtrl* pIntensity=(CSliderCtrl*)GetDlgItem(IDC_INTENSITY);
	pIntensity->SetRange(1,100);

	// Initialize Pos
	pInnerCircle->SetPos(400);
	SetInnerSize();
	pOuterCircle->SetPos(800);
	SetOuterSize();
	pIntensity->SetPos(40);
	SetIntensity();

	return TRUE;
}

BOOL CTerrainHeightDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	int nID=::GetDlgCtrlID(pMsg->hwnd);
	if(nID==IDC_INNER_CIRCLE_SIZE)
	{
		if(pMsg->message==WM_LBUTTONDOWN||pMsg->message==WM_MOUSEMOVE)
			SetInnerSize();
	}

	if(nID==IDC_OUTER_CIRCLE_SIZE)
	{
		if(pMsg->message==WM_LBUTTONDOWN||pMsg->message==WM_MOUSEMOVE)
			SetOuterSize();
	}

	if(nID==IDC_INTENSITY)
	{
		if(pMsg->message==WM_LBUTTONDOWN||pMsg->message==WM_MOUSEMOVE)
			SetIntensity();
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTerrainHeightDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ExpandButton.m_hWnd!=NULL)
		m_ExpandButton.MoveWindow(0,0,cx,20);
}

void CTerrainHeightDlg::SetExpand(BOOL bExpand)
{
	m_ExpandButton.SetExpand(bExpand);
	if(bExpand)
		SetWindowPos(NULL,0,0,m_rc.Width(),m_rc.Height(),SWP_NOMOVE);
	else
		SetWindowPos(NULL,0,0,m_rc.Width(),20+1,SWP_NOMOVE);
}

void CTerrainHeightDlg::OnBnClickedExpand()
{
	SetExpand(!m_ExpandButton.m_bExpanded);
	((CTerrainEditorDlg*)GetParent())->SetCtrlPos();
}

BOOL CTerrainHeightDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int nID=LOWORD(wParam);
	TerrainHeightHandler* pHeightHandler=TerrainHeightHandler::GetInstance();

	switch(nID)
	{
	case IDB_VERTEX_HEIGHT_ADJUST:
		MouseMsgInterface::GetInstance()->SwitchToHeightEditor();
		pHeightHandler->SetBrushType(HeightType_Adjust);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(IDB_VERTEX_HEIGHT_ADJUST);
		break;
	case IDB_VERTEX_HEIGHT_UP:
		MouseMsgInterface::GetInstance()->SwitchToHeightEditor();
		pHeightHandler->SetBrushType(HeightType_Up);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(IDB_VERTEX_HEIGHT_UP);
		break;
	case IDB_VERTEX_HEIGHT_DOWN:
		MouseMsgInterface::GetInstance()->SwitchToHeightEditor();
		pHeightHandler->SetBrushType(HeightType_Down);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(IDB_VERTEX_HEIGHT_DOWN);
		break;
	case IDB_VERTEX_HEIGHT_SMOOTH:
		MouseMsgInterface::GetInstance()->SwitchToHeightEditor();
		pHeightHandler->SetBrushType(HeightType_Smooth);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(IDB_VERTEX_HEIGHT_SMOOTH);
		break;
	case IDB_VERTEX_HEIGHT_NOISE:
		MouseMsgInterface::GetInstance()->SwitchToHeightEditor();
		pHeightHandler->SetBrushType(HeightType_Noise);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(IDB_VERTEX_HEIGHT_NOISE);
		break;
	case IDB_VERTEX_HEIGHT_FLATTEN:
		MouseMsgInterface::GetInstance()->SwitchToHeightEditor();
		pHeightHandler->SetBrushType(HeightType_Flatten);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(IDB_VERTEX_HEIGHT_FLATTEN);
		break;
	case IDB_VERTEX_HEIGHT_SET:
		MouseMsgInterface::GetInstance()->SwitchToHeightEditor();
		pHeightHandler->SetBrushType(HeightType_Set);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(IDB_VERTEX_HEIGHT_SET);
		break;
	case IDB_MODE_CHUNK_ADJUST:
		MouseMsgInterface::GetInstance()->SwitchToHeightEditor();
		pHeightHandler->SetBrushType(HeightType_ChunkAdjust);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(IDB_MODE_CHUNK_ADJUST);
		break;
	case IDB_MODE_CELL_ADJUST:
		MouseMsgInterface::GetInstance()->SwitchToHeightEditor();
		pHeightHandler->SetBrushType(HeightType_CellAdjust);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(IDB_MODE_CELL_ADJUST);
		break;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}

void CTerrainHeightDlg::OnWorldHeightUp()
{
	CString str;
	GetDlgItemText(IDE_WORLD_AJUST_HEIGHT,str);
	int data=atoi((LPCTSTR)str);
	if(data==0) return ;

	TerrainHeightHandler::GetInstance()->AdjustWorldHeight(data);
}

void CTerrainHeightDlg::OnWorldHeightDown()
{
	CString str;
	GetDlgItemText(IDE_WORLD_AJUST_HEIGHT,str);
	int data=atoi((LPCTSTR)str);
	if(data==0) return ;

	TerrainHeightHandler::GetInstance()->AdjustWorldHeight(-data);
}

void CTerrainHeightDlg::SetButtonState(int nSelID)
{
	BOOL bFind = FALSE;
	COLORREF normalColor=RGB(0,0,0),focusColor=RGB(255,0,255);
	for(int i=0;i<sizeof(button_id1)/sizeof(int);++i)
	{
		if(nSelID==button_id1[i])
		{
			m_Button[i].SetTextColor(focusColor);
			bFind = TRUE;
		}
		else m_Button[i].SetTextColor(normalColor);
		m_Button[i].Invalidate();
	}
	if(!bFind) TerrainHeightHandler::GetInstance()->SetBrushType(HeightType_None);
}

void CTerrainHeightDlg::OnChunkCellHeightSet()
{
	CString str;
	GetDlgItemText(IDE_CHUNK_CELL_VALUE,str);
	int data=atoi((LPCTSTR)str);
	if(data==0) return ;

	TerrainHeightHandler::GetInstance()->AdjustChunkCellHeight(data,true);
}

void CTerrainHeightDlg::OnChunkCellHeightUp()
{
	CString str;
	GetDlgItemText(IDE_CHUNK_CELL_VALUE,str);
	int data=atoi((LPCTSTR)str);
	if(data==0) return ;

	TerrainHeightHandler::GetInstance()->AdjustChunkCellHeight(data,false);
}

void CTerrainHeightDlg::OnChunkCellHeightDown()
{
	CString str;
	GetDlgItemText(IDE_CHUNK_CELL_VALUE,str);
	int data=atoi((LPCTSTR)str);
	if(data==0) return ;

	TerrainHeightHandler::GetInstance()->AdjustChunkCellHeight(-data,false);
}

void CTerrainHeightDlg::OnVertexHeightSetValueChanged()
{
	int settedHeight=GetDlgItemInt(IDE_VERTEX_HEIGHT_SETVALUE);
	TerrainHeightHandler::GetInstance()->ChangeSettedVertexHeightVal(settedHeight);
}

void CTerrainHeightDlg::SetInnerSize()
{
	CSliderCtrl* pInnerCircle=(CSliderCtrl*)GetDlgItem(IDC_INNER_CIRCLE_SIZE);
	int pos=pInnerCircle->GetPos();
	float ratio=(float)pos/SLIDER_LENGTH;
	float innerRadius=(float)MIN_CIRCLE_RADIUS+(float)(MAX_CIRCLE_RADIUS-MIN_CIRCLE_RADIUS)*ratio;
	TerrainHeightHandler::GetInstance()->SetInnerRadius(innerRadius);

	CSliderCtrl* pOuterCircle=(CSliderCtrl*)GetDlgItem(IDC_OUTER_CIRCLE_SIZE);
	if(pos>pOuterCircle->GetPos())
	{
		pOuterCircle->SetPos(pos);
		TerrainHeightHandler::GetInstance()->SetOuterRadius(innerRadius);
	}
}

void CTerrainHeightDlg::SetOuterSize()
{
	CSliderCtrl* pOuterCircle=(CSliderCtrl*)GetDlgItem(IDC_OUTER_CIRCLE_SIZE);
	int pos=pOuterCircle->GetPos();
	float ratio=(float)pos/SLIDER_LENGTH;
	float outerRadius=(float)MIN_CIRCLE_RADIUS+(float)(MAX_CIRCLE_RADIUS-MIN_CIRCLE_RADIUS)*ratio;
	TerrainHeightHandler::GetInstance()->SetOuterRadius(outerRadius);

	CSliderCtrl* pInnerCircle=(CSliderCtrl*)GetDlgItem(IDC_INNER_CIRCLE_SIZE);
	if(pos<=pInnerCircle->GetPos())
	{
		pInnerCircle->SetPos(pos);
		TerrainHeightHandler::GetInstance()->SetInnerRadius(outerRadius);
	}
}

void CTerrainHeightDlg::SetIntensity()
{
	CSliderCtrl* pIntensity=(CSliderCtrl*)GetDlgItem(IDC_INTENSITY);
	TerrainHeightHandler::GetInstance()->SetIntensity(pIntensity->GetPos());
}

void CTerrainHeightDlg::EnableAllCtrls(BOOL bEnable)
{
	CWnd* pWnd = GetWindow(GW_CHILD);
	while(pWnd != NULL)
	{
		if(pWnd->GetDlgCtrlID()!=IDB_EXPAND)
		pWnd->EnableWindow(bEnable);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
}