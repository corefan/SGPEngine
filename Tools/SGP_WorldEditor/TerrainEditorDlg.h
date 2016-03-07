#pragma once

#include "TerrainHeightDlg.h"
#include "TerrainTextureDlg.h"
#include "TerrainGrassDlg.h"
#include "TerrainWaterDlg.h"

// CTerrainEditorDlg 对话框

class CTerrainEditorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTerrainEditorDlg)
public:
	CTerrainEditorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTerrainEditorDlg();

// 对话框数据
	enum { IDD = IDD_TERRAIN_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnNcHitTest(CPoint point);
private:
	CTerrainHeightDlg m_HeightDlg;
	CTerrainTextureDlg m_TerrainTexDlg;
	CTerrainGrassDlg m_TerrainGrassDlg;
	CTerrainWaterDlg m_TerrainWaterDlg;
public:
	void SetCtrlPos();
	void EnableAllCtrls(BOOL bEnable);
	void SetButtonState(int nSelID);
	int m_nButtonState;
};
