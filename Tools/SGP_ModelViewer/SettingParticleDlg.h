#pragma once

#include "ExpandButton.h"
// CSettingParticleDlg 对话框

class CSettingParticleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingParticleDlg)

public:
	CSettingParticleDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSettingParticleDlg();

// 对话框数据
	enum { IDD = IDD_SETTING_PARTILCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CExpandButton m_ExpandButton;
	afx_msg void OnStepModeChanged();
	afx_msg void OnClampStepModeChanged();
	afx_msg void OnApplyStepMode();
	afx_msg void OnResetStepMode();
	afx_msg void OnButtonExpand();
	void SetShowMode(BOOL bExpand);
	CRect m_rc;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ApplySettting();
};
