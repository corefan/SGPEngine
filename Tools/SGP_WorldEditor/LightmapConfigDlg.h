#pragma once
#include "afxwin.h"

#define DEFAULT_LIGHTMAP_MINSIZE 32
#define DEFAULT_LIGHTMAP_MAXSIZE 1024

// CLightmapConfigDlg 对话框

class CLightMapBuildDlg;

class CLightmapConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLightmapConfigDlg)
	static CLightmapConfigDlg* s_pLightmapConfigDlg;
public:
	CLightmapConfigDlg(CLightMapBuildDlg* pBuildDlg,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLightmapConfigDlg();
	static CLightmapConfigDlg* GetInstance(){ASSERT(s_pLightmapConfigDlg!=NULL);return s_pLightmapConfigDlg;}
// 对话框数据
	enum { IDD = IDD_LIGHTMAP_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBrowseTexFolder();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDefaultTexSize();
	afx_msg void OnCustomTexSize();
public:
	CLightMapBuildDlg* m_pBuildDlg;
	CComboBox m_CustomTexSizeCombo;
};
