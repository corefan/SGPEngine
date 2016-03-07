#pragma once

#include "..\SGP_ModelViewer\ExpandButton.h"
#include "GrassListCtrl.h"

#define MIN_GRASSCIRCLE_RADIUS 0.5f
#define MAX_GRASSCIRCLE_RADIUS 5.0f
#define GRASS_SLIDER_LENGTH 1000

// CTerrainGrassDlg 对话框

class CTerrainGrassDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTerrainGrassDlg)
	static CTerrainGrassDlg* s_pGrassDlg;
public:
	CTerrainGrassDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTerrainGrassDlg();
	static CTerrainGrassDlg* GetInstance(){ASSERT(s_pGrassDlg!=NULL);return s_pGrassDlg;}
// 对话框数据
	enum { IDD = IDD_TERRAIN_GRASS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedExpand();
	afx_msg void OnImportTexture();
	afx_msg void OnSettingApply();
	afx_msg void OnGenerateTextureList();
	afx_msg void OnPreviewTexture();
private:
	CExpandButton m_ExpandButton;
	CRect m_rc;
	CMFCButton m_Button[2];
	CString m_TexturePath;
	CImageList m_ImageList;
	CGrassListCtrl m_GrassTexList;
	int m_nGrassIndex;
public:
	void SetExpand(BOOL bExpand);
	void SetButtonState(int nSelID);
	void SetGrassMinSize();
	void SetGrassMaxSize();
	void SetGrassDensity();
	void SetWindOffsetX();
	void SetWindOffsetZ();
	void SetBrushSize();
	void SetTextureIndex(int index);
	void EnableAllCtrls(BOOL bEnable);

	void UpdateGrassList(CString strPath,int xTile,int yTile);

	void InitDefaultValue();
	void LoadAttributeFromMap();
};
