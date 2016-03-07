#pragma once

#define UM_UPDATE_TEXTURE_LIST WM_USER+1
#include "ThumbnailGenerator.h"
#include "afxcmn.h"
// CTerrainTextureLoadDlg 对话框
class CTerrainTextureDlg;

class CTerrainTextureLoadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTerrainTextureLoadDlg)

public:
	CTerrainTextureLoadDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTerrainTextureLoadDlg();

// 对话框数据
	enum { IDD = IDD_TERRAIN_TEXTURE_LOADING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	int m_FileCount;
	CTerrainTextureDlg* m_pTextureDlg;
	CFileFind m_Finder;
	BOOL m_bExist;
	int m_index;
	ThumbnailGenerator m_ThumbnailGenerator;
	CProgressCtrl m_Progress;
	CBrush m_BorderBrush;

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT OnKickIdle(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUpdateTextureList(WPARAM wParam,LPARAM lParam);

	void ClearNoUseThumbnail();
	BOOL CreateThumbnails();
	void UpdateDlgCtrls();
};
