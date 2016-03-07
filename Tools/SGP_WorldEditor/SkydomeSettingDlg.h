#pragma once

#include "..\SGP_ModelViewer\ExpandButton.h"

// CSkydomeSettingDlg �Ի���

class CSkydomeSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSkydomeSettingDlg)
	static CSkydomeSettingDlg* s_pSkydomeDlg;
public:
	CSkydomeSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSkydomeSettingDlg();
	static CSkydomeSettingDlg* GetInstance(){ASSERT(s_pSkydomeDlg!=NULL);return s_pSkydomeDlg;}
// �Ի�������
	enum { IDD = IDD_SKYDOME_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CExpandButton m_ExpandButton;
	CRect m_rc;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpand();
	afx_msg void OnChooseMF1();
	afx_msg void OnChooseCloudTexture();
	afx_msg void OnChooseNoiseTexture();
	afx_msg void OnApplySetting();
public:
	void SetExpand(BOOL bExpand);
	void InitDefaultValue();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void EnableAllCtrls(BOOL bEnable);
	void LoadAttributeFromMap();
};
