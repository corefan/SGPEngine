#pragma once

#include "..\SGP_ModelViewer\ExpandButton.h"
#include "afxcmn.h"
#include "ImageCtrl.h"
#include "TextureListCtrl.h"
#include <vector>
// CTerrainTexureDlg 对话框

#define MIN_TEXCIRCLE_RADIUS 1
#define MAX_TEXCIRCLE_RADIUS 20
#define TEX_SLIDER_LENGTH 1000

class CTerrainTextureDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTerrainTextureDlg)
	friend class CTerrainEditorDlg;
	friend class CTerrainTextureLoadDlg;
	friend class CTextureListCtrl;
	static CTerrainTextureDlg* s_pTextureDlg;
public:
	CTerrainTextureDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTerrainTextureDlg();

	static CTerrainTextureDlg* GetInstance(){ASSERT(s_pTextureDlg!=NULL);return s_pTextureDlg;}
// 对话框数据
	enum { IDD = IDD_TERRAIN_TEXTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedExpand();
	afx_msg void OnTextureResourceRefresh();
	afx_msg void OnBrowseFolder();
public:
	void SetInnerSize();
	void SetOuterSize();
	void SetTextureAlpha();
	void SetCurrentLayer(int i);
private:
	CTextureListCtrl m_TextureList;
	CImageList m_ImageList;
	CImageCtrl m_LayerImageCtrl[4];
	CImageCtrl m_ChunkImageCtrl[7];
	CExpandButton m_ExpandButton;
	CRect m_rc;
	CMFCButton m_Button[3];
	int m_nCurrLayer;
	BOOL m_bInitialized;
public:
	void SetExpand(BOOL bExpand);
	void SetButtonState(int nSelID);
	void InitTextureResourceList();
	void SetLayerImage(CString strName);
	void SetLayerImage(int nLayer,CString sTextureName,CString sImageName);
	void SetChunkImage(int index,CString sTextureName,CString sImageName);
	void UpdateSelectedChunkInfo(const std::vector<int>& chunkVector);
	void EnableAllCtrls(BOOL bEnable);
	BOOL IsLayerClosed(int nLayer);
};
