#pragma once

// CLightMapBuildDlg 对话框

#define UM_UPDATE_OBJ_TEXTURE WM_USER+0x01
#define UM_UPDATE_TERRAIN_TEXTURE WM_USER+0x02

namespace sgp
{
	struct ISGPObject;
}

class CLightMapBuildDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLightMapBuildDlg)

public:
	CLightMapBuildDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLightMapBuildDlg();

// 对话框数据
	enum { IDD = IDD_LIGHTMAP_BUILD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	float m_ProcessRatio;
	CString m_CurrProcessInfo;
	CProgressCtrl m_Progress;
	bool m_bReady;

	void SetMode(bool bReady);
	void SetProcessInfo(CString str){m_CurrProcessInfo=str;}
	void UpdateCtrls();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnUpdateTerrainTexture(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUpdateObjTexture(WPARAM wParam,LPARAM lParam);
public:
	bool m_bIncludeTerrain;
	bool m_bIncludeSeledObjects;
	uint32 m_TextureWidth;
	uint32 m_TextureHeight;
	uint32 m_SampleCount;
	float m_fCollisionOffset;
	float m_fAODistance;
	CString m_TextureFolder;
	bool m_bDefaultLightmapSize;

	void GenerateLightmapTexture(uint32 width,uint32 height,uint32* lMap,CString strPath);
	uint32 GetObjLightmapSize(CommonObject& obj);
};
