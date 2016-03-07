#pragma once


// CNewMapLoadingDlg 对话框

class CNewMapLoadingDlg : public CDialogEx
{
	friend class CNewMapConfigDlg;
	friend class WorldMapManager;
	DECLARE_DYNAMIC(CNewMapLoadingDlg)

public:
	CNewMapLoadingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNewMapLoadingDlg();

// 对话框数据
	enum { IDD = IDD_NEWMAP_LOADING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg LRESULT OnKickIdle(WPARAM wParam,LPARAM lParam);
private:
	CBrush m_BorderBrush;

	CString m_MapName;
	CString m_MapFolder;
	int m_TerrainSize;
	int m_TerrainMaxHeight;
	bool m_bUsePerlinNoise;
	CString m_Layer0TexPath;
};
