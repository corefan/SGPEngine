#pragma once

#include "ExpandButton.h"
// CSettingParticleDlg �Ի���

class CSettingParticleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingParticleDlg)

public:
	CSettingParticleDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSettingParticleDlg();

// �Ի�������
	enum { IDD = IDD_SETTING_PARTILCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
