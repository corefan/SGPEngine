#pragma once

#include "..\SGP_ModelViewer\ExpandButton.h"

// CObjectEditorDlg 对话框

class CObjectEditorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CObjectEditorDlg)

public:
	CObjectEditorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CObjectEditorDlg();

// 对话框数据
	enum { IDD = IDD_OBJECT_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CExpandButton m_ExpandButton;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void SetExpand(BOOL bExpand);
	void OnExpand();
	CRect m_rc;

	CSpinButtonCtrl m_StepRotatePin;
	CSpinButtonCtrl m_RandomRotatePinMinX;
	CSpinButtonCtrl m_RandomRotatePinMaxX;
	CSpinButtonCtrl m_RandomRotatePinMinY;
	CSpinButtonCtrl m_RandomRotatePinMaxY;
	CSpinButtonCtrl m_RandomRotatePinMinZ;
	CSpinButtonCtrl m_RandomRotatePinMaxZ;

	CSpinButtonCtrl m_StepMovePin;
	CSpinButtonCtrl m_StepScalePin;
	CSpinButtonCtrl m_RandomScalePinMin;
	CSpinButtonCtrl m_RandomScalePinMax;

	afx_msg void OnDeltaposStepMoveSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposStepScaleSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposRandomScaleMinSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposRandomScaleMaxSpin(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	void MoveObjects(float dx,float dy,float dz);
	void RotateObjects(float dx,float dy,float dz);
	void ScaleObjects(float fScale);
	float GetDlgItemFloat(UINT nID);
	float GetRotateValue(UINT nID);

	void EnableAllCtrls(BOOL bEnable);
};
