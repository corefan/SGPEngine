#pragma once

class CNewMapLoadingDlg;
// CNewMapConfigDlg 对话框

class CNewMapConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewMapConfigDlg)

public:
	CNewMapConfigDlg(CNewMapLoadingDlg* loadingDlg,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNewMapConfigDlg();

// 对话框数据
	enum { IDD = IDD_NEWMAP_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBrowseLayer0Texture();
	afx_msg void OnBrowseMapFolder();
	virtual void OnOK();
	void CreateMap();
private:
	CString m_Layer0TexturePath;
	CNewMapLoadingDlg* m_loadingDlg;
	
};
