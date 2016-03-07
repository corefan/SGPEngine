#pragma once

#include "..\SGP_ModelViewer\ExpandButton.h"
#include "WaterImageCtrl.h"
// CTerrainWaterDlg 对话框

class CTerrainWaterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTerrainWaterDlg)
	static CTerrainWaterDlg* s_pWaterDlg;
public:
	CTerrainWaterDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTerrainWaterDlg();
	static CTerrainWaterDlg* GetInstance(){ASSERT(s_pWaterDlg!=NULL);return s_pWaterDlg;}

// 对话框数据
	enum { IDD = IDD_TERRAIN_WATER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedExpand();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnImportTexture();
	afx_msg void OnAddWater();
	afx_msg void OnDeleteWater();
	afx_msg void OnGenerateWater();
	afx_msg void OnApplySetting();
private:
	CExpandButton m_ExpandButton;
	CRect m_rc;
	CWaterImageCtrl m_WaterImgCtrl;
	CString m_TexturePath;
	CMFCButton m_Button[3];
public:
	void SetExpand(BOOL bExpand);
	void SetButtonState(int nSelID);
	void InitDefaultValue();
	void EnableAllCtrls(BOOL bEnable);

	void LoadAttributeFromMap();
};
