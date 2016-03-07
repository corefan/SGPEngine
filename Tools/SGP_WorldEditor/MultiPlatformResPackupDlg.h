#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CMultiPlatformResPackupDlg 对话框
class CPackupResProgressDlg;

class CMultiPlatformResPackupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMultiPlatformResPackupDlg)

public:
	CMultiPlatformResPackupDlg(CPackupResProgressDlg* pPackupResDlg,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMultiPlatformResPackupDlg();

// 对话框数据
	enum { IDD = IDD_PACKUP_MULTI_PLATFORM_RES };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
private:
	CPackupResProgressDlg* m_pPackupResDlg;
	CComboBox m_PlatformCombo;
	CComboBox m_PixelFormatCombo;
	CComboBox m_CompressQualityCombo;
	CTabCtrl m_TargetTab;
	CString m_RelativeMapPath;
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBrowseFolder();
	afx_msg void OnBrowseMap();
	afx_msg void OnPackupResource();
	afx_msg void OnBrowseResFolder();
	afx_msg void OnTcnSelchangePackupTargetTab(NMHDR *pNMHDR, LRESULT *pResult);
	void ChangeTab();
	afx_msg void OnRunEmulation();
};
