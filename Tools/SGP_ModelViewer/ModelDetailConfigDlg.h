#pragma once

#include "ExpandButton.h"

namespace sgp
{
	struct SGPMF1ConfigSetting;
}

#include <vector>
// CModelDetailConfigDlg 对话框

class CModelDetailConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModelDetailConfigDlg)

public:
	CModelDetailConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModelDetailConfigDlg();

// 对话框数据
	enum { IDD = /*IDD_MODELDETAIL_CONFIG*/7039 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CExpandButton m_ExpandButton;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void SetShowMode(BOOL bExpand);
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpand();
	void EnableWindows(BOOL bEnable=TRUE);
	afx_msg void OnConfigAdd();
	afx_msg void OnConfigSave();
	afx_msg void OnConfigDelete();
	afx_msg void OnConfigModify();
	afx_msg void OnConfigListSelChanged();
	BOOL JudgeValidate(BOOL bAvoidSelf);
	BOOL IsConfigSettingEmpty(sgp::SGPMF1ConfigSetting& setting);
	void LoadConfigSetting(int nConfigCount,sgp::SGPMF1ConfigSetting* pSetting);
	void PurgeConfigSetting();
	void ModelClosed();
private:
	std::vector<sgp::SGPMF1ConfigSetting*> m_ConfigSetVector;
	CRect m_rc;
public:
	// save operation
	unsigned int m_MF1ConfigCount;
	sgp::SGPMF1ConfigSetting* m_pMF1ConfigSetting;
	sgp::SGPMF1ConfigSetting* m_pTmpConfigSetting;
	void SaveConfigSetting();
	void AfterSaveConfigSetting();
	BOOL IsConfigSettingChanged();
};
