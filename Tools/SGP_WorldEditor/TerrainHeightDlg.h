#pragma once

#include "..\SGP_ModelViewer\ExpandButton.h"

#include "FocusButton.h"

#define MIN_CIRCLE_RADIUS 2
#define MAX_CIRCLE_RADIUS 20
#define SLIDER_LENGTH 1000

extern int button_count;

// CTerrainHeightDlg 对话框

class CTerrainHeightDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTerrainHeightDlg)

public:
	CTerrainHeightDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTerrainHeightDlg();

// 对话框数据
	enum { IDD = IDD_TERRAIN_HEIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedExpand();
	afx_msg void OnWorldHeightUp();
	afx_msg void OnWorldHeightDown();
	afx_msg void OnChunkCellHeightSet();
	afx_msg void OnChunkCellHeightUp();
	afx_msg void OnChunkCellHeightDown();
	afx_msg void OnVertexHeightSetValueChanged();
	void SetExpand(BOOL bExpand);
private:
	CExpandButton m_ExpandButton;
	CRect m_rc;
	CMFCButton m_Button[9];
public:
	void SetButtonState(int nSelID);
	void SetInnerSize();
	void SetOuterSize();
	void SetIntensity();
	void EnableAllCtrls(BOOL bEnable);
};
